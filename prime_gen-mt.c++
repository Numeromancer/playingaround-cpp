#include <memory>
#include <iostream>
#include <functional>
#include <vector>
#include <queue>
#include <algorithm>
#include <string>
#include <cassert>
#include <chrono>
#include <thread>
#include <mutex>
#include <condition_variable>

template<typename T>
struct Channel
{
    std::queue<T> m_q;
    std::mutex m_mutex;
    std::condition_variable data_ready;

    friend T receive(Channel<T> & c) {
        std::unique_lock<std::mutex> lk(c.m_mutex);
        c.data_ready.wait(lk, [&c]{return !c.m_q.empty();});
        T res = c.m_q.front();
        c.m_q.pop();
        return res;
    }

    friend void send(Channel<T> & c, const T & t) {
        std::lock_guard<std::mutex> lk(c.m_mutex);
        c.m_q.push(t);
        c.data_ready.notify_one();
    }

    friend size_t backlog(Channel<T> & c) {
        return c.m_q.size();
    }
};


using namespace std;
using namespace std::literals;

using Generator = function<unsigned int(void)>;
using Filter = function<bool(unsigned int)>;

using UIChannel = shared_ptr<Channel<unsigned int>>;

template<typename T, typename... Args>
inline auto make_channel(Args&&... args) {
    return make_shared<Channel<T>>(args...);
}


auto
make_all_ns() {
    unsigned int n = 1;
    auto c = make_channel<unsigned int>();
    thread t{ [=]()mutable {
        while (n++) {
            this_thread::sleep_for(chrono::milliseconds(backlog(*c)));
            send(*c, n);
        }
    } };
    t.detach();
    return c;
}


auto
make_next_channel(UIChannel s, unsigned int p) {
    unsigned int n{};
    auto c = make_channel<unsigned int>();
    thread t{ [=]()mutable {
        while (true) {
            n = receive(*s);  
            if (n % p) {
                this_thread::sleep_for(chrono::milliseconds(backlog(*c)));
                send(*c, n);
            }
        }
    } };
    t.detach();
    return c;
}


auto
make_prime_generator() {
    auto all_ns = make_all_ns();
    auto next = all_ns;
    auto c = make_channel<unsigned int>();
    thread t { [=]()mutable {
        while (true) {
            auto n = receive(*next);
            next = make_next_channel(next, n);
            send(*c, n);
        }
    } };
    t.detach();
    return c;
}


vector<unsigned int>
take_while(UIChannel r, Filter f) {
    vector<unsigned int> res;
    auto n = receive(*r);
    while (f(n)) {
        res.push_back(n);
        n = receive(*r);
    }
    return res;
}

vector<unsigned int>
take_n(UIChannel r, unsigned int n) {
    unsigned int i = 0;
    auto cond = [=](auto x)mutable { return i++ <= n; };
    return take_while(r, cond);
}

vector<unsigned int>
take_after(UIChannel r, unsigned int m, unsigned int n) {
    unsigned int i = 0;
    auto upto = [=](auto)mutable { return i++ <= m; };
    auto when = [=](auto)mutable { return i++ <= n; };
    take_while(r, upto);
    auto res = take_while(r, when);
    return res;
}

template<typename Sequence>
Sequence concat(const Sequence & v1, const Sequence & v2) {
    Sequence res{};
    res.reserve(v1.size() + v2.size());
    copy(begin(v1), end(v1), back_inserter(res));
    copy(begin(v2), end(v2), back_inserter(res));
    return res;
}

unsigned int
take_first_that(UIChannel r, Filter f) {
    auto n = receive(*r);
    while ( ! f(n) ) {
        n = receive(*r);
    }
    return n;
}

vector<unsigned int>
take_between(UIChannel r, unsigned int m, unsigned int n) {
    auto when = [=](auto x)mutable { return x <= n; };
    vector<unsigned int> res = { take_first_that(r, [=](auto x){ return x >=  m; }) };
    res = concat(res, take_while(r, when));
    return res;
}


void show(const vector<unsigned int> & v) {
    cout << "[ ";
    for_each(begin(v), end(v), [](auto x){ cout << x << ' '; });
    cout << ']';
}


int
main(int argc, char ** argv) {
    assert(argc == 3);
    auto m = stoul(argv[1]);
    auto n = stoul(argv[2]);
    auto v = take_between(make_prime_generator(), m, n);
    show(v);
    cout << endl;
    return 0;
}

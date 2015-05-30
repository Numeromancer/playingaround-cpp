#include <thread>
#include <iostream>
#include <chrono>

using namespace std;

struct thread_guard {
    thread & t;
    explicit thread_guard(thread & t_) : t(t_) {}
    ~thread_guard() {
        if (t.joinable()) {
            t.join();
        }
    }
    thread_guard(thread_guard const &) = delete;
    thread_guard& operator=(thread_guard const &) = delete;
};


struct scoped_thread {
private:
    thread t;
public:
    explicit scoped_thread(thread t_)
        : t(move(t_))
    {
        if ( ! t.joinable() ) {
            throw logic_error("No thread");
        }
    }
    ~scoped_thread() { t.join(); }
    scoped_thread(scoped_thread const &) = delete;
    scoped_thread& operator=(scoped_thread const &) = delete;
};


void some_function(int wait_s) {
    this_thread::sleep_for(chrono::seconds(wait_s));
    cout << "some function" << endl;
}

void some_other_function(int wait_s) {
    this_thread::sleep_for(chrono::seconds(wait_s));
    cout << "some other function" << endl;
}

int main() {
    thread t1(some_function, 0);
    thread t2 = move(t1);
    t1 = thread(some_other_function, 1);
    scoped_thread t4(thread(some_other_function, 10));
    thread t3;
    t3 = move(t2);
    //t1 = move(t3);
    thread_guard g3{t3};
    thread_guard g1{t1};
    return 0;
}

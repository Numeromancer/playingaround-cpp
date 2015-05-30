#include <iostream>
#include <functional>
#include <vector>
#include <algorithm>
#include <string>
#include <cassert>

using namespace std;

using Generator = function<unsigned int(void)>;
using Filter = function<bool(unsigned int)>;

Generator
make_all_ns() {
    unsigned int n = 2;
    return [=]()mutable{ auto oldn = n; n += 1; return oldn; };
}

Generator
filter(Generator & gen, Filter filter) {
    return [=]() {
        while (true) {
            auto n = gen();  
            if (filter(n)) {
                return n;
            }
        }
    };
}

Generator
make_next_filter(Generator & gen, unsigned int p) {
    return filter(gen, [=](auto n){ return ( (n % p) != 0 ); });
}


Generator
make_prime_generator() {
    auto all_ns = make_all_ns();
    auto next = all_ns;
    return [=]() mutable {
        auto n = next();
        next = make_next_filter(next, n);
        return n;
    };
}


vector<unsigned int>
take_while(Generator & gen, Filter f) {
    vector<unsigned int> res;
    auto n = gen();
    while (f(n)) {
        res.push_back(n);
        n = gen();
    }
    return res;
}

unsigned int
take_first_that(Generator & gen, Filter f) {
    auto n = gen();
    while ( ! f(n) ) {
        n = gen();
    }
    return n;
}

vector<unsigned int>
take_n(Generator && gen, unsigned int n) {
    unsigned int i = 0;
    auto cond = [=](auto x)mutable { return i++ <= n; };
    return take_while(gen, cond);
}

vector<unsigned int>
take_after(Generator && gen, unsigned int m, unsigned int n) {
    unsigned int i = 0;
    auto upto = [=, &i](auto)mutable { return i++ < m; };
    auto when = [=, &i](auto)mutable { return i++ <= ( m + n ); };
    take_while(gen, upto);
    auto res = take_while(gen, when);
    return res;
}

template<typename Sequence>
Sequence concat(const Sequence & v1, const Sequence & v2) {
    Sequence res{};
    copy(begin(v1), end(v1), back_inserter(res));
    copy(begin(v2), end(v2), back_inserter(res));
    return res;
}

vector<unsigned int>
take_between(Generator && gen, unsigned int m, unsigned int n) {
    auto when = [=](auto x)mutable { return x <= n; };
    vector<unsigned int> res = { take_first_that(gen, [=](auto x){ return x >=  m; }) };
    res = concat(res, take_while(gen, when));
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
    {
        auto v = take_between(make_prime_generator(), m, n);
        show(v);
        cout << endl;
    }
       /*
          {
          auto v = take_after(make_prime_generator(), m, n);
          show(v);
          cout << endl;
          }
          */
    return 0;
}


#include <thread>
#include <condition_variable>
#include <memory>
#include <iostream>
#include <mutex>
#include <vector>
#include <ratio>

using namespace std;

mutex m;
condition_variable v_condition;
bool v_is_full = false;

vector<int> v;

void f(vector<int> & v)
{
    unique_lock<mutex> lck{m};
    cout << "Hello from f" << endl;
    while( ! v_is_full ) v_condition.wait(lck);
    for (auto & el : v) {
        el = el + 5;
    }
}

template<typename T>
struct F {
    using type = T;
    vector<type> & v;
    F(vector<type> & vv) : v(vv) { }
    void operator()() {
        { 
            unique_lock<mutex> lck{m};
            cout << "Hello from F" << endl;
        }
        for (int i = 0 ; i < 1000 ; i++) {
            unique_lock<mutex> lck{m};
            v.push_back(type());
        }
        v_is_full = true;
        v_condition.notify_one();
    }
};

template<intmax_t N, intmax_t D>
ostream & operator<<(ostream & os, const ratio<N,D> & r) {
    os << N << "/" << D;
    return os;
}

void user()
{
    auto t1 = thread{f,ref(v)};
    auto t2 = thread{F<int>{v}};
    using one_thirtythird = ratio<1,33>;

    t1.join();
    t2.join();
    cout << "v has " << v.size() << " elements." << endl;
    cout << "v[0] = " << v[0] << endl;
    cout << ratio<2,14>::type{} << endl;
}


int
main()
{
    user();
    return 0;
}

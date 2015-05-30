#include <iostream>
#include <vector>

using namespace std;


struct S {
    int x = 99;
    constexpr S() {
        // cout << " default ctor\n";
    }
    constexpr S(int _x) : x(_x) {
        // cout << " int ctor\n";
    }
    S(S const &) { cout << " copy ctor\n"; }
    S(S &&) { cout << " move ctor\n"; }
    ~S() { cout << " dtor\n"; }
    void operator =(S const &) { cout << " copy assignment\n"; }
    void operator =(S &&) { cout << " move assignment\n"; }
};


const S x_s44{44};
const S x_s{};

int main() {
    cout << x_s.x << endl;
    cout << x_s44.x << endl;
    {
        cout << "A1:\n";
        vector<S> s { S(), S(), S() };
    }
    {
        cout << "A2:\n";
        vector<S> s { {}, {}, {} };
    }
    {
        cout << "A3:\n";
        vector<S> s { 0, 0, 0 };
    }
    {
        cout << "B1:\n";
        vector<S> s;
        s.reserve(3);
        s.push_back(S());
        s.push_back(S());
        s.push_back(S());
    }
    {
        cout << "B2:\n";
        vector<S> s;
        s.reserve(3);
        s.push_back({});
        s.push_back({});
        s.push_back({});
    }
    {
        cout << "B3:\n";
        vector<S> s;
        s.reserve(3);
        s.push_back(0);
        s.push_back(0);
        s.push_back(0);
    }
    {
        cout << "C1:\n";
        vector<S> s;
        s.reserve(3);
        s.emplace_back(S());
        s.emplace_back(S());
        s.emplace_back(S());
    }
    // {
    //  cout << "C2:\n";
    //  vector<S> s;
    //  s.reserve(3);
    //  s.emplace_back({});
    //  s.emplace_back({});
    //  s.emplace_back({});
    // }
    {
        cout << "C3:\n";
        vector<S> s;
        s.reserve(3);
        s.emplace_back(0);
        s.emplace_back(0);
        s.emplace_back(0);
    }
}


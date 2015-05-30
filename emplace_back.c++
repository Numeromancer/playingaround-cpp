#include <vector>
#include <iostream>
#include <sstream>
#include <string>

using namespace std;

struct S {
    string s_;
    S(int i, const string & s, double d) {
        ostringstream oss;
        oss << "i=" << i << ", s=“" << s << "”, d=" << d;
        s_ = oss.str();
    }
    string s() {
        return s_;
    }
};

int
main(int argc, char ** argv) {
    vector<S> v;
    string s(argv[1] ? argv[1] : "(no arg)");
    for (int i = 12 ; i < 40; i++) {
        v.emplace_back(i, s, 2.8*i);
    }
    for (auto s : v) {
        cout << s.s() << endl;
    }
    return 0;
}

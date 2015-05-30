
#include <utility>
#include <iostream>

using namespace std;

struct S {};

void process (S & s) {
    cout << "process L value" << endl;
}

void process (S&& s) {
    cout << "process R value" << endl;
}


template<typename T>
void
indirect_process(T&& t) {
    cout << "Indirectly:";
    process(forward<T>(t));
}

int
main()
{
    S s;
    indirect_process(s);
    indirect_process(move(s));
}

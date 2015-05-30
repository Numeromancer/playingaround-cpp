
#include <utility>
#include <functional>
#include <algorithm>
#include <iterator>

template <class InputIterator, class BinaryPredicate>
inline auto
find_duplicate(InputIterator first, InputIterator last, BinaryPredicate pred)
    -> std::pair<InputIterator, InputIterator>
{
    using namespace std;
    using namespace std::placeholders;
    while(first != last) {
        auto pdup = find_if(next(first), last, bind(pred, *first, _1));
        if ( pdup != last) {
            return make_pair(first, pdup);
        }
        advance(first,1);
    }
    return make_pair(last, last);
}


#include <set>
#include <iostream>

using namespace std;

int main()
{
    using StringPair = pair<string, string>;
    set<StringPair> c = {
        { "ssid_0_8901234567890123456789012",
            "psk4567890123456789012345678901234567890123456789012345678901234" }, 
        { "ssid_1_8901234567890123456789012",
            "psk4567890123456789012345678901234567890123456789012345678901234" }, 
        { "ssid_2_8901234567890123456789012",
            "psk4567890123456789012345678901234567890123456789012345678901234" }, 
        { "ssid_3_8901234567890123456789012",
            "psk4567890123456789012345678901234567890123456789012345678901234" }, 
        { "ssid_4_8901234567890123456789012",
            "psk4567890123456789012345678901234567890123456789012345678901234" }, 
        { "ssid_5_8901234567890123456789012",
            "psk4567890123456789012345678901234567890123456789012345678901234" }, 
        { "ssid_6_8901234567890123456789012",
            "ssid_6_8901234567890123456789012" }, 
        { "ssid_6_8901234567890123456789012",
            "ssid_7_8901234567890123456789012" }, 
    };

    auto dup = find_duplicate(begin(c), end(c),
                            [](const StringPair & v1, const StringPair & v2)
                            {
                                return v1.first == v2.first;
                            });
    if (dup.first == end(c)) {
        cout << "no duplicate" << endl;
        cout << "dup.first == end(c) ? " << (dup.first == end(c)) << endl;
    } else {
        cout << "duplicate: " << dup.first->first << " = " << dup.second->first << endl;
        cout << "dup.first == end(c) ? " << (dup.first == end(c)) << endl;
    }
    return 0;
}

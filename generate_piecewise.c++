
#include <cstdint>
#include <string>
#include <iostream>
#include <algorithm>
#include <list>
#include <vector>
#include <type_traits>
#include <iterator>


template<
class InputIterator
, class OutputIterator
, class UnaryOperation
, class InputType = typename std::iterator_traits<InputIterator>::value_type
, class OutputType = typename std::iterator_traits<OutputIterator>::value_type
>
inline OutputIterator
generate_piecewise(InputIterator first, InputIterator last,
                   OutputIterator out, UnaryOperation gen)
{
    using namespace std;
    for_each(first, last, [&](const InputType & item)
    {
        auto res = gen(item);
        move(begin(res), end(res), out);
    });
    return out;
}


int main()
{
    using namespace std;
    auto hexify = [](const uint8_t * p, size_t sz) {
        string s;
        generate_piecewise(p, p+sz, back_inserter(s),
           [](uint8_t b) {
               static const char hex[17] = "0123456789ABCDEF";
               char bs[2];
               bs[0] = hex[(b>>4) & 0xF];
               bs[1] = hex[b & 0xF];
               return string(bs, 2);
        });
        return s;
    };
    auto make_x_xs = [](int x) {
        list<int> l;
        fill_n(back_inserter(l), x, x);
        x += 5;
        return l;
    };
            
    uint8_t data[] = "01234567890123456789";
    cout << hexify(data, sizeof(data)) << endl;

    int xs[] = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 100};
    vector<unsigned int> v;
    generate_piecewise(begin(xs), end(xs), back_inserter(v), make_x_xs);
    for (auto e : xs) cout << e << " "; cout << endl;
    for (auto e : v) cout << e << " "; cout << endl;
    return 0;
}

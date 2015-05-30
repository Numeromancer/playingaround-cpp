// Compiled with:
// clang-3.6 -std=c++14 -Wall roman-numeral-kata.c++ -o roman-numeral-kata

#include <iostream>
#include <functional>
#include <map>
#include <string>
#include <regex>
#include <cassert>

using namespace std;
using namespace std::literals;

// "'M' can be repeated at most 3 times in a row"
// So the largest is MMMCMXCIX
constexpr int MaxRomanNumeral = 3000 + 900 + 90 + 9;

// For coverting Roman to & from arabic, I use an intermidiary synax where
// there are only I, X, C and M repeated. This syntax is more regular (I don't
// mean that in the strict sense. I probably is, but I haven't verified it),
// and easier to convert to and from. I then collapse this syntax into the
// usual one.
// 
// We don't actually deal with arabic form directly, of course; we just use an
// integer and let the standard formatters do the work when we output.


// Collapse the intermediary syntax into the common form.
//
// Don't need a regex here, but std::string doesn't have the kind of replace
// function I need.
static auto
collapse(const string & s) {
    string ret = s;
    ret = regex_replace(ret , regex{"CCCCCCCCC"} , "CM");
    ret = regex_replace(ret , regex{"CCCCC"}     , "D");
    ret = regex_replace(ret , regex{"CCCC"}      , "CD");
    ret = regex_replace(ret , regex{"XXXXXXXXX"} , "XC");
    ret = regex_replace(ret , regex{"XXXXX"}     , "L");
    ret = regex_replace(ret , regex{"XXXX"}      , "XL");
    ret = regex_replace(ret , regex{"IIIIIIIII"} , "IX");
    ret = regex_replace(ret , regex{"IIIII"}     , "V");
    ret = regex_replace(ret , regex{"IIII"}      , "IV");
    return ret;
}

// Expand away the prefix numbers and the half-decades
// The order of the replacements matters.
static auto
expand(const string & s) {
    string ret = s;
    ret = regex_replace(ret , regex{"CM"} , "CCCCCCCCC");
    ret = regex_replace(ret , regex{"CD"} , "CCCC");
    ret = regex_replace(ret , regex{"XC"} , "XXXXXXXXX");
    ret = regex_replace(ret , regex{"XL"} , "XXXX");
    ret = regex_replace(ret , regex{"IX"} , "IIIIIIIII");
    ret = regex_replace(ret , regex{"IV"} , "IIII");
    ret = regex_replace(ret , regex{"D"}  , "CCCCC");
    ret = regex_replace(ret , regex{"L"}  , "XXXXX");
    ret = regex_replace(ret , regex{"V"}  , "IIIII");
    return ret;
}


// Convert an integer into a string with the intermediary Roman numeral syntax
// described above.
static auto
convert(unsigned int n) {
    if (n > MaxRomanNumeral) {
        ostringstream oss;
        oss << n << " is larger than "
            << MaxRomanNumeral << ", the largest possible Roman numeral";
        throw invalid_argument(oss.str());
    }
    unsigned int ms = n / 1000;
    n %= 1000;
    unsigned int cs = n / 100;
    n %=100;
    unsigned int xs = n / 10;
    n %= 10;
    unsigned int is = n;
    return string(ms, 'M') + string(cs, 'C') + string(xs, 'X') + string(is, 'I');
}

const map<char, unsigned int> RNValue = {
    { 'I', 1 }, { 'X', 10 }, { 'C', 100 }, { 'M', 1000 }
};

// Convert from intermediary syntax to unsigned int.
static auto
unconvert(string s) {
    unsigned int n = 0;
    for (auto c : s) {
        try {
            n += RNValue.at(c);
        } catch(out_of_range & oor) {
            ostringstream oss;
            oss << s << " is not a valid Roman numeral ";
            throw invalid_argument(oss.str());
        }
    }
    return n;
}

static string
upcase(const string & s) {
    string ret;
    transform(begin(s), end(s), back_inserter(ret),
        [](const char c) {
            return ::toupper(c);
        });
    return ret;
}

int
main(int argc, char ** argv) {
    {
        assert(argc > 1);
        argv++; argc--;
        auto toRoman = [](string s) {
            try {
                auto rn = collapse(convert(stoul(s)));
                cout << rn << endl;
            } catch(invalid_argument & ia) {
                cerr << ia.what() << endl;
                exit(1);
            }
        };
        auto fromRoman = [](string s) {
            try {
                cout << unconvert(expand(upcase(s))) << endl;
            } catch(invalid_argument & ia) {
                cerr << ia.what() << endl;
                exit(1);
            }
        };
        auto test = [](string s) {
            try {
                auto rn = collapse(convert(stoul(s)));
                cout << s << "->" << rn << "->" << unconvert(expand(rn)) << endl;
            } catch(invalid_argument & ia) {
                cerr << ia.what() << endl;
                exit(1);
            }
        };
        function<void(string)> action = toRoman;

        // pass -t as the first arg to show conversion and unconversion
        if ("-t"s == *argv) {
            argv++; argc--;
            action = test;
        } else if ("-u"s == *argv) {
            argv++; argc--;
            // unconvert: Roman Numberal to Arabic
            action = fromRoman;
        }
        while(argc) {
            action(*argv);
            argv++; argc--;
        }
        return 0;
    }
}

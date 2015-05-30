
#include <string>
#include <cassert>
#include <tuple>
#include <iostream>

extern "C" {
#include <lua.h>
#include <lauxlib.h>
#include <lualib.h>
}

using namespace std;

namespace sel {

class State {
private:
    lua_State * _l;
public:
    State() : _l(nullptr) { _l = luaL_newstate(); }
    State(const State & other) = delete;
    State & operator=(const State & other) = delete;
    State(State && other) : _l(other._l) { other._l = nullptr; }
    ~State() { if (_l != nullptr) lua_close(_l); }

    void Load(const std::string & scriptPath) {
        luaL_dofile(_l, scriptPath.c_str());
    }

    void Push(const int i) { lua_pushinteger(_l, i); }
    void Push(const bool q) { lua_pushboolean(_l, q); }
    void Push(const double x) { lua_pushnumber(_l, x); }
    void Push() {}

    template<typename T, typename... Ts>
        void Push(const T value, const Ts... values) {
            Push(value);
            Push(values...);
        }

    template<typename T> T Read(int index) const;

    template <size_t, typename... Ts>
        struct _pop {
            typedef std::tuple<Ts...> type;

            template<typename T>
                static std::tuple<T> _readTuple(const State & s, const int index) {
                    return std::make_tuple(s.Read<T>(index));
                }

            template<typename T1, typename T2, typename... Rest>
                static std::tuple<T1, T2, Rest...>
                _readTuple(const State & s, const int index) {
                    std::tuple<T1> head = std::make_tuple(s.Read<T1>(index));
                    return std::tuple_cat(head, _readTuple<T2, Rest...>(s, index + 1));
                }
            static type apply(State &s) {
                auto ret = _readTuple<Ts...>(s, 1);
                lua_pop(s._l, int(sizeof...(Ts)));
                return ret;
            }
        };

    template <typename T>
        struct _pop<1, T> {
            typedef std::tuple<T> type;
            static type apply(State &s) {
                T ret = s.Read<T>(-1);
                lua_pop(s._l, 1);
                return ret;
            }
        };

    template <typename... Ts>
        struct _pop<0, Ts...> {
            typedef void type;
            static type apply(State &s) { }
        };

    template<typename... Ts>
        typename _pop<sizeof...(Ts), Ts...>::type
        Pop() {
            return _pop<sizeof...(Ts), Ts...>::apply(*this);
        }
 
    template <typename... Ret, typename... Args>
        typename _pop<sizeof...(Ret), Ret...>::type
        Call(const std::string &funcName, const Args&... args)
        {
            lua_getglobal(_l, funcName.c_str());

            const int num_args = sizeof...(Args);
            const int num_return_values = sizeof...(Ret);

            Push(args...);

            lua_call(_l, num_args, num_return_values);
            return Pop<Ret...>();
        }
};

template<>
inline int
State::
Read(int index) const {
    return lua_tointeger(_l, index);
}

template<>
inline float
State::
Read(int index) const {
    return lua_tonumber(_l, index);
}
template<>
inline bool
State::
Read(int index) const {
    return lua_toboolean(_l, index);
}
template<>
inline string
State::
Read(int index) const {
    return std::string(lua_tostring(_l, index));
}

}


int main()
{
    sel::State state;
    state.Load("example.lua");

    // Stack is 1-indexed from the bottom
    int diff;
    string greeting;

    // Push function name onto the stack
    tie(diff, greeting) = state.Call<int, string>("subtract_and_hello", 1, 3);
    assert(diff == -2  &&  greeting == "hello");

    int xs[4];
    tie(xs[0], xs[1], xs[2], xs[3]) =
        state.Call<int, int, int, int>("get_and_send4", 4, 55, 666, 777);
    for (auto x : xs) cout << x << " "; cout << endl;
}

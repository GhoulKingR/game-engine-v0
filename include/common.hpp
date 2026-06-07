#pragma once

#include <cstddef>
#include <type_traits>

namespace engine {
    template<typename T>
    concept ScalarType = std::is_scalar_v<T>;

    template<ScalarType T>
    struct vec2{
        T &x = _internal[0];
        T &y = _internal[1];
        T *data() { return _internal; }
        vec2(T x = 0, T y = 0) {
            _internal[0] = x;
            _internal[1] = y;
        }

        vec2(const vec2<T>& _v) {
            x = _v.x;
            y = _v.y;
        }
        vec2 operator=(const vec2<T>& _v) {
            x = _v.x;
            y = _v.y;
            return *this;
        }

        template<ScalarType Tp>
        vec2 operator*(const Tp &val) const {
            return vec2(x * val, y * val);
        }

        vec2 operator-(const vec2<T> &val) const {
            return vec2(x - val.x, y - val.y);
        }

        vec2 operator-=(const vec2<T> &val) {
            auto n = vec2(x - val.x, y - val.y);
            x = n.x; y = n.y;
            return n;
        }

        vec2 operator+(const vec2<T> &val) const {
            return vec2(x + val.x, y + val.y);
        }

        vec2 operator+=(const vec2<T> &val) {
            auto n = vec2(x + val.x, y + val.y);
            x = n.x; y = n.y;
            return n;
        }

    private:
        T _internal[2];
    };
}

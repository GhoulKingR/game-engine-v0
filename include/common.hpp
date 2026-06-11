#pragma once

#include <cstddef>
#include <type_traits>

namespace engine {
    template<typename T>
    concept ScalarType = std::is_scalar_v<T>;

    template<ScalarType T>
    class vec2{
        T _internal[2];

    public:
        T *data() { return _internal; }

        vec2(T x = 0, T y = 0) {
            _internal[0] = x;
            _internal[1] = y;
        }

        vec2(const vec2<T>& _v) {
            _internal[0] = _v._internal[0];
            _internal[1] = _v._internal[1];
        }
        vec2& operator=(const vec2<T>& _v) {
            _internal[0] = _v._internal[0];
            _internal[1] = _v._internal[1];
            return *this;
        }

        const T& x() const { return _internal[0]; }
        const T& y() const { return _internal[1]; }
        T& x() { return _internal[0]; }
        T& y() { return _internal[1]; }

        template<ScalarType Tp>
        vec2 operator*(const Tp &val) const {
            return {
                _internal[0] * val,
                _internal[1] * val
            };
        }

        template<ScalarType Tp>
        vec2 operator-(const vec2<Tp> &_v) const {
            return {
                _internal[0] - _v._internal[0],
                _internal[1] - _v._internal[1],
            };
        }

        template<ScalarType Tp>
        vec2& operator-=(const vec2<Tp> &_v) {
            _internal[0] -= _v._internal[0];
            _internal[1] -= _v._internal[1];
            return *this;
        }

        template<ScalarType Tp>
        vec2 operator+(const vec2<Tp> &_v) const {
            return {
                _internal[0] + _v._internal[0],
                _internal[1] + _v._internal[1],
            };
        }

        template<ScalarType Tp>
        vec2& operator+=(const vec2<Tp> &_v) {
            _internal[0] += _v._internal[0];
            _internal[1] += _v._internal[1];
            return *this;
        }
    };
}

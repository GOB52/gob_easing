/*!
  @file gob_easing.hpp
  @brief Main header for gob_easing

  @mainpage gob_easing
  Easing functions that allowed compile-time calculation with constexpr (C++ 11 or later)

  @sa https://easings.net/
  
  @author GOB / GitHub:<a href="https://github.com/GOB52/">GOB52</a> / X(Twitter):<a href="https://twitter.com/gob_52_gob">@GOB_52_GOB</a>

  @copyright 2024 GOB
  @copyright Licensed under the MIT license. See LICENSE file in the project root for full license information.
 */
#ifndef GOB_EASING_HPP
#define GOB_EASING_HPP

#include <cstdint>
#include <cmath>
#include <type_traits>

// Define if you are forced to use own math functions
//#define GOBLIB_EASING_USING_FORCE_OWN_MATH

// Some arithmetic functions are constexpr in GCC version 4.6.1 or later.
#if !defined(__clang__) && (defined(__GNUC__) && defined(__GNUC_MINOR__) && defined(__GNUC_PATCHLEVEL__))
# define GOBLIB_EASING_GCC_VERSION (__GNUC__ * 10000 + __GNUC_MINOR__ * 100 + __GNUC_PATCHLEVEL__)
#else
# define GOBLIB_EASING_GCC_VERSION 0
#endif

#if GOBLIB_EASING_GCC_VERSION < 40601 || defined(GOBLIB_EASING_USING_FORCE_OWN_MATH)
#include <limits>
# define GOBLIB_EASING_USING_OWN_MATH
#endif

/*!
  @namespace goblib
  @brief Top level namespace of mine
 */
namespace goblib
{
/*!
  @namespace easing
  @brief For easing
*/
namespace easing
{

/*!
  @namespace constants
  @brief Constants for easing
*/
namespace constants
{
///@cond 0
template<typename T> constexpr T pi() noexcept { return T{M_PI}; }
template<typename T> constexpr T half_pi() noexcept { return  pi<T>() * T{0.5}; }
template<typename T> constexpr T pi2() noexcept { return pi<T>() * T{2.0}; }
template<typename T> constexpr T e() noexcept { return T{2.71828182845904523536}; }

template<typename T> constexpr T back_factor() noexcept { return T{1.70158}; }
template<typename T> constexpr T back_factor2() noexcept { return back_factor<T>() * T{1.525}; }
template<typename T> constexpr T elastic_factor() noexcept { return pi2<T>() / T{3.0}; }
template<typename T> constexpr T elastic_factor2() noexcept { return pi2<T>() / T{4.5}; }
template<typename T> constexpr T bounce_factor() noexcept { return T{2.75}; }
template<typename T> constexpr T bounce_factor2() noexcept { return T{7.5625}; }
///@endcond
}//

/*!
  @namespace math
  @brief Use constexpr arithmetic functions if available, or use own implementation if not.
  @warning Constexpr arithmetic functions for easing.
  @warning Note that they are not universally usable.
*/
namespace math
{
///@cond 0
template<typename T> constexpr T abs(const T x) { return x >= T{0} ? x : -x; }
template<typename T> constexpr bool equal_fp(const T x, const T y)
{
    return abs(x - y) <= std::numeric_limits<T>::epsilon();
}

#if defined(GOBLIB_EASING_USING_OWN_MATH)
# pragma message "Using uniquely implemented arithmetic functions"

// sqrt(fp)
template<typename T> constexpr T sqrt_impl(const T x, const T curr, const T prev)
{
    static_assert(std::is_arithmetic<T>::value, "x must be arithmetic type");
    return equal_fp(curr, prev) ? curr : sqrt_impl(x, T{0.5} * (curr + x / curr), curr);
}
template<typename T> constexpr T sqrt(const T x)
{
    static_assert(std::is_arithmetic<T>::value, "x must be arithmetic type");
    return (x >= T{0} && x < std::numeric_limits<T>::infinity())
            ? sqrt_impl(x, x, T{0})
            : (x < 0) ? std::numeric_limits<T>::quiet_NaN() : std::numeric_limits<T>::infinity();
}

// factorial(unsigned int)
template<typename T> constexpr T factorial(T n)
{
    static_assert(std::is_unsigned<T>::value && std::is_integral<T>::value, "n must be unsigned integral type");
    return (n == 0) ? 1 : n * factorial(n - 1);
}

// exp(fp)
template <typename T> constexpr T exp_impl(T x, T sum, T n, int i, T t)
{
    static_assert(std::is_arithmetic<T>::value, "x must be arithmetic type");
    return equal_fp(sum, sum + t/n) ? sum : exp_impl(x, sum + t/n, n * i, i+1, t * x);
}
template <typename T> constexpr T exp(T x)
{
    static_assert(std::is_floating_point<T>::value, "x must be floating point number");
    return exp_impl(x, T{1}, T{1}, 2, x);
}

// log(fp,fp)
template <typename T> constexpr T log_iter(T x, T y)
{
    static_assert(std::is_arithmetic<T>::value, "x must be arithmetic type");
    return y + T{2} * (x - exp(y)) / (x + exp(y));
}

template <typename T> constexpr T log(T x, T y)
{
    static_assert(std::is_arithmetic<T>::value, "x must be arithmetic type");
    return equal_fp(y, log_iter(x, y)) ? y : log(x, log_iter(x, y));
}

// pow(fp, integer) pow(fp, fp)
template <typename T, typename U,
          typename std::enable_if< std::is_integral<U>::value, std::nullptr_t>::type = nullptr>
constexpr T pow(const T x, U const y)
{
    static_assert(std::is_arithmetic<T>::value, "x must be arithmetic type");
    return (y == 0) ? T{1} :
            y == 1 ? x :
            y > 1 ?
            ((y & 1) ? x * pow(x, y - 1) : pow(x, y / 2) * pow(x, y / 2)) :
            T{1} / pow(x, -y);
}

template <typename T,
          typename std::enable_if< std::is_floating_point<T>::value, std::nullptr_t>::type = nullptr>
constexpr T pow(const T x, const T y)
{
    static_assert(std::is_arithmetic<T>::value, "x must be arithmetic type");
    return (y == std::numeric_limits<T>::infinity()) ? std::numeric_limits<T>::infinity() :
            (y == -std::numeric_limits<T>::infinity()) ? T{0} : exp(log(x, T{constants::e<T>()}) * y);
}

// sin(fp)
template <typename T> constexpr T sincos_impl(T x, T sum, T n, int i, int s, T t)
{
    return equal_fp(sum ,sum + t*s/n) ? sum :
            sincos_impl(x, sum + t*s/n, n*i*(i+1), i+2, -s, t*x*x);
}

template <typename T> constexpr T sin(const T x)
{
    return sincos_impl(x, x, T{6}, 4, -1, x*x*x);
}

template <typename T> constexpr T cos(const T x)
{
    return sincos_impl(x, T{1}, T{2}, 3, -1, x*x);
}

#else

# pragma message "Using GCC-extended arithmetic functions"
template<typename T> constexpr T sqrt(const T x) { return std::sqrt(x); }
template<typename T> constexpr T pow(const T x, const T y) { return std::pow(x, y); }
template<typename T> constexpr T pow(const T x, const int y) { return std::pow(x, y); }
template<typename T> constexpr T sin(const T x) { return std::sin(x); }
template<typename T> constexpr T cos(const T x) { return std::cos(x); }

#endif
///@endcond
}//

///@name Easing behavior
///@note Argument t [0.0 ~ 1.0]
///@warning No range check of values is performed, so check on the side to be passed on.
///@{

/// @brief Linear
template<typename T> constexpr T linear(const T t)
{
    static_assert(std::is_floating_point<T>::value, "t must be floating point number");
    return t;
}

/// @brief Ease in sinusoidal
/// @sa https://easings.net/#easeInSine
template<typename T> constexpr T inSinusoidal(const T t)
{
    static_assert(std::is_floating_point<T>::value, "t must be floating point number");
    return -math::cos(t * constants::half_pi<T>()) + T{1};
}

/// @brief Ease out sinusoidal
/// @sa https://easings.net/#easeOutSine
template<typename T> constexpr T outSinusoidal(const T t)
{
    static_assert(std::is_floating_point<T>::value, "t must be floating point number");
    return math::sin(t * constants::half_pi<T>());
}

/// @brief Ease inout sinusoidal
/// @sa https://easings.net/#easeInOutSine
template<typename T> constexpr T inOutSinusoidal(const T t)
{
    static_assert(std::is_floating_point<T>::value, "t must be floating point number");
    return -T{0.5} * (math::cos(t * constants::pi<T>()) - T{1});
}

/// @brief Ease in quadratic
/// @sa https://easings.net/#easeInQuad
template<typename T> constexpr T inQuadratic(const T t)
{
    static_assert(std::is_floating_point<T>::value, "t must be floating point number");
    return t * t;
}

/// @brief Ease out quadratic
/// @sa https://easings.net/#easeOutQuad
template<typename T> constexpr T outQuadratic(const T t)
{
    static_assert(std::is_floating_point<T>::value, "t must be floating point number");
    return -t * (t - T{2.0});
}

/// @brief Ease inout quadratic
/// @sa https://easings.net/#easeInOutQuad
template<typename T> constexpr T inOutQuadratic(const T t)
{
    static_assert(std::is_floating_point<T>::value, "t must be floating point number");
    return ((t * T{2}) < T{1.0}) ?
            (T{0.5} * (t * T{2}) * (t * T{2})) :
            -T{0.5} * (((t * T{2}) - T{1}) * (((t * T{2}) - T{1}) - T{2}) - T{1});
}

/// @brief Ease in cubic
/// @sa https://easings.net/#easeInCubic
template<typename T> constexpr T inCubic(const T t)
{
    static_assert(std::is_floating_point<T>::value, "t must be floating point number");
    return t * t * t;
}

/// @brief Ease out cubic
/// @sa https://easings.net/#easeOutCubic
template<typename T> constexpr T outCubic(const T t)
{
    static_assert(std::is_floating_point<T>::value, "t must be floating point number");
    return ((t - T{1}) * (t - T{1}) * (t - T{1}) + T{1});
}

/// @brief Ease inout cubic
/// @sa https://easings.net/#easeInOutCubic
template<typename T> constexpr T inOutCubic(const T t)
{
    static_assert(std::is_floating_point<T>::value, "t must be floating point number");    
    return (t * T{2}) < T{1} ?
            T{0.5} * (t * T{2}) * (t * T{2} ) * (t * T{2}) :
            T{0.5} * ((t * T{2} - T{2}) * (t * T{2} - T{2}) * (t * T{2} - T{2}) + T{2});
}

/// @brief Ease in quartic
/// @sa https://easings.net/#easeInQuart
template<typename T> constexpr T inQuartic(const T t)
{
    static_assert(std::is_floating_point<T>::value, "t must be floating point number");
    return t * t * t * t;
}

/// @brief Ease out quartic
/// @sa https://easings.net/#easeOutQuart
template<typename T> constexpr T outQuartic(const T t)
{
    static_assert(std::is_floating_point<T>::value, "t must be floating point number");
    return -((t - T{1}) * (t - T{1}) * (t - T{1}) * (t - T{1}) - T{1});
}

/// @brief Ease inout quartic
/// @sa https://easings.net/#easeOutQuart
template<typename T> constexpr T inOutQuartic(const T t)
{
    static_assert(std::is_floating_point<T>::value, "t must be floating point number");
    return (t * T{2}) < T{1} ?
            T{0.5} * (t * T{2}) * (t * T{2}) * (t * T{2}) * (t * T{2}) :
            -T{0.5} * ((t * T{2} - T{2}) * (t * T{2} - T{2}) * (t * T{2} - T{2}) * (t * T{2} - T{2}) - T{2});
}

/// @brief Ease in quintic
/// @sa https://easings.net/#easeInQuint
template<typename T> constexpr T inQuintic(const T t)
{
    static_assert(std::is_floating_point<T>::value, "t must be floating point number");
    return t * t * t * t * t;
}

/// @brief Ease out quintic
/// @sa https://easings.net/#easeOutQuint
template<typename T> constexpr T outQuintic(const T t)
{
    static_assert(std::is_floating_point<T>::value, "t must be floating point number");
    return ((t - T{1}) * (t - T{1}) * (t - T{1}) * (t - T{1}) * (t - T{1}) + T{1});
}

/// @brief Ease inout quintic
/// @sa https://easings.net/#easeInOutQuint
template<typename T> constexpr T inOutQuintic(const T t)
{
    static_assert(std::is_floating_point<T>::value, "t must be floating point number");
    return (t * T{2}) < T{1} ?
            T{0.5} * (t * T{2}) * (t * T{2}) * (t * T{2}) * (t * T{2}) * (t * T{2}) :
            T{0.5} * ((t * T{2} - T{2}) * (t * T{2} - T{2}) * (t * T{2} - T{2}) * (t * T{2} - T{2}) * (t * T{2} - T{2}) + T{2});
}

/// @brief Ease in exponential
/// @sa https://easings.net/#easeInExpo
template<typename T> constexpr T inExponential(const T t)
{
    static_assert(std::is_floating_point<T>::value, "t must be floating point number");
    return math::equal_fp(t, T{0}) ? T{0} : math::pow(T{2}, T{10} * (t - T{1}));
}

/// @brief Ease out exponential
/// @sa https://easings.net/#easeOutExpo
template<typename T> constexpr T outExponential(const T t)
{
    static_assert(std::is_floating_point<T>::value, "t must be floating point number");
    return math::equal_fp(t, T{1}) ? T{1} : -math::pow(T{2}, -T{10} * t) + T{1};
}

/// @brief Ease inout exponential
/// @sa https://easings.net/#easeInOutExpo
template<typename T>constexpr T inOutExponential(const T t)
{
    static_assert(std::is_floating_point<T>::value, "t must be floating point number");
    return math::equal_fp(t, T{0}) ? T{0} :
            math::equal_fp(t, T{1}) ? T{1} :
            (t * T{2}) < T{1} ?
            T{0.5} * math::pow(T{2}, T{10} * (t * T{2} - T{1})) :
            T{0.5} * (-math::pow(T{2}, -T{10} * (t * T{2} - T{1})) + T{2});
}

/// @brief Ease in circular
/// @sa https://easings.net/#easeInCirc
template<typename T> constexpr T inCircular(const T t)
{
    static_assert(std::is_floating_point<T>::value, "t must be floating point number");
    return -(math::sqrt(T{1} - t * t) - T{1});
}

/// @brief Ease out circular
/// @sa https://easings.net/#easeOutCirc
template<typename T> constexpr T outCircular(const T t)
{
    return math::sqrt(T{1} - (t - T{1}) * (t - T{1}));
}

/// @brief Ease inout circular
/// @sa https://easings.net/#easeInOutCirc
template<typename T> constexpr T inOutCircular(const T t)
{
    static_assert(std::is_floating_point<T>::value, "t must be floating point number");
    return (t * T{2}) < T{1} ?
            -T{0.5} * (math::sqrt(T{1} - (t * T{2}) * (t * T{2})) - T{1}) :
             T{0.5} * (math::sqrt(T{1} - (t * T{2} - T{2}) * (t * T{2} - T{2})) + T{1});
}

/// @brief Ease in back
/// @sa https://easings.net/#easeInBack
template<typename T> constexpr T inBack(const T t)
{
    static_assert(std::is_floating_point<T>::value, "t must be floating point number");
    return t * t * ((constants::back_factor<T>() + T{1} ) * t - constants::back_factor<T>());
}

/// @brief Ease out back
/// @sa https://easings.net/#easeOutBack
template<typename T> constexpr T outBack(const T t)
{
    static_assert(std::is_floating_point<T>::value, "t must be floating point number");
    return ((t - T{1})* (t - T{1}) * ((constants::back_factor<T>() + T{1} ) * (t - T{1}) + constants::back_factor<T>()) + T{1});
}

/// @brief Ease inout back
/// @sa https://easings.net/#easeInOutBack
template<typename T> constexpr T inOutBack(const T t)
{
    static_assert(std::is_floating_point<T>::value, "t must be floating point number");
    return (t * T{2}) < T{1} ?
            T{0.5} * ((t * T{2}) * (t * T{2}) * ((constants::back_factor2<T>() + T{1}) * (t * T{2}) - constants::back_factor2<T>()) ) :
    T{0.5} * ((t * T{2} - T{2}) * (t * T{2} - T{2}) * ((constants::back_factor2<T>() + T{1}) * (t * T{2} - T{2}) + constants::back_factor2<T>()) + T{2});
}

/// @brief Ease in elastic
/// @sa https://easings.net/#easeInElastic
template<typename T> constexpr T inElastic(const T t)
{
    static_assert(std::is_floating_point<T>::value, "t must be floating point number");
    //INF,NaN occurs depending on the value of float in own sin, in that case switch to double.
#if defined(GOBLIB_EASING_USING_OWN_MATH)
    using sin_type = typename std::common_type< T, double>::type;
#else
    using sin_type = T;
#endif
    return (t <= T{0}) ? T{0} :
            (t >= T{1}) ? T{1} :
            -math::pow(T{2}, T{10} * t - T{10}) * math::sin((sin_type)(t * T{10} - T{10.75}) * constants::elastic_factor<T>());
}

/// @brief Ease out elastic
/// @sa https://easings.net/#easeOutElastic
template<typename T> constexpr T outElastic(const T t)
{
    static_assert(std::is_floating_point<T>::value, "t must be floating point number");
#if defined(GOBLIB_EASING_USING_OWN_MATH)
    using sin_type = typename std::common_type< T, double>::type;
#else
    using sin_type = T;
#endif
    return (t <= T{0}) ? T{0} :
            (t >= T{1}) ? T{1} :
            math::pow(T{2}, -T{10} * t) * math::sin((sin_type)(t * T{10} - T{0.75}) * constants::elastic_factor<T>()) + T{1};
}

/// @brief Ease inout elastic
/// @sa https://easings.net/#easeInOutElastic
template<typename T> constexpr T inOutElastic(const T t)
{
    static_assert(std::is_floating_point<T>::value, "t must be floating point number");
#if defined(GOBLIB_EASING_USING_OWN_MATH)
    using sin_type = typename std::common_type< T, double>::type;
#else
    using sin_type = T;
#endif
    return (t <= T{0}) ? T{0} :
            ((t >= T{1}) ? T{1} :
             (t < T{0.5} ?
            -T{0.5} * (math::pow(T{2}, T{20} * t - T{10}) * math::sin((sin_type)(T{20} * t - T{11.125}) * constants::elastic_factor2<T>())) :
             T{0.5} * (math::pow(T{2}, -T{20} * t + T{10}) * math::sin((sin_type)(T{20} * t - T{11.125}) * constants::elastic_factor2<T>())) + T{1}));
}

/// @brief Ease out bounce
/// @sa https://easings.net/#easeOutBounce
template<typename T> constexpr T outBounce(const T t)
{
    static_assert(std::is_floating_point<T>::value, "t must be floating point number");
    return t < (T{1} / constants::bounce_factor<T>()) ? constants::bounce_factor2<T>() * t * t :
            t < (T{2} / constants::bounce_factor<T>()) ? constants::bounce_factor2<T>() * (t - (T{1.5} / constants::bounce_factor<T>())) * (t - (T{1.5} / constants::bounce_factor<T>())) + T{0.75} :
            t < (T{2.5} / constants::bounce_factor<T>()) ? constants::bounce_factor2<T>() * (t - (T{2.25} / constants::bounce_factor<T>())) * (t- (T{2.25} / constants::bounce_factor<T>())) + T{0.9375} :
            constants::bounce_factor2<T>() * (t - (T{2.625} / constants::bounce_factor<T>())) * (t - (T{2.625} / constants::bounce_factor<T>())) + T{0.984375};
}

/// @brief Ease in bounce
/// @sa https://easings.net/#easeInBounce
template<typename T> constexpr T inBounce(const T t)
{
    static_assert(std::is_floating_point<T>::value, "t must be floating point number");
    return T{1} - outBounce<T>(T{1} - t);
}

/// @brief Ease inout bounce
/// @sa https://easings.net/#easeInOutBounce
template<typename T>constexpr T inOutBounce(const T t)
{
    static_assert(std::is_floating_point<T>::value, "t must be floating point number");
    return t < T{0.5} ? (T{1} - outBounce<T>(T{1} - T{2} * t)) * T{0.5}
            :  (T{1} + outBounce<T>(T{2} * t - T{1})) * T{0.5};
}
/// @}
}}
#endif

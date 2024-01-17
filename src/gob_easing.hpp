/*!
  @file gob_easing.hpp
  @brief Standard group of easing functions that allowed compile-time calculation with constexpr

  @mainpage gob_easing
  Standard group of easing functions that allowed compile-time calculation with constexpr

  @sa https://easings.net/
  
  @author GOB / GitHub:<a href="https://github.com/GOB52/">GOB52</a> / X(Twitter):<a href="https://twitter.com/gob_52_gob">@GOB_52_GOB</a>

  @copyright 2024 GOB
  @copyright Licensed under the MIT license. See LICENSE file in the project root for full license information.
 */
#ifndef GOB_EASING_HPP
#define GOB_EASING_HPP

#include <cstdint>
#include <cmath>

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
constexpr float pi = M_PI;
constexpr float half_pi = pi * 0.5f;
constexpr float pi2 = pi * 2.0f;

constexpr float e = 2.71828182845904523536f;

constexpr float back_factor = 1.70158f;
constexpr float back_factor2 = back_factor * 1.525f;

constexpr float elastic_factor = pi2 / 3.0f;
constexpr float elastic_factor2 = pi2 / 4.5f;

constexpr float bounce_factor =  2.75f;
constexpr float bounce_factor2 = 7.5625f;
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
template<typename T> constexpr T abs(const T x) noexcept
{
    return x >= T{0} ? x : -x;
}            

template<typename T> constexpr bool equal_fp(const T x, const T y) noexcept
{
    return abs(x - y) <= std::numeric_limits<T>::epsilon();
}

#if defined(GOBLIB_EASING_USING_OWN_MATH)
# pragma message "Using uniquely implemented arithmetic functions"

// sqrt
template<typename T> constexpr T sqrt_impl(const T x, const T curr, const T prev) noexcept
{
    return equal_fp(curr, prev) ? curr : sqrt_impl(x, T{0.5} * (curr + x / curr), curr);
}
template<typename T> constexpr T sqrt(const T x) noexcept
{
    return (x >= T{0} && x < std::numeric_limits<T>::infinity())
            ? sqrt_impl(x, x, T{0})
            : (x < 0) ? std::numeric_limits<T>::quiet_NaN() : std::numeric_limits<T>::infinity();
}

// factorial(unsigned int)
template<typename T> constexpr T factorial(T n) noexcept
{
    static_assert(std::is_unsigned<T>::value, "n must be unsigned arithmetic type");
    return (n == 0) ? 1 : n * factorial(n - 1);
}

// exp(fp)
template <typename T> constexpr T exp_impl(T x, T sum, T n, int i, T t) noexcept
{
    return equal_fp(sum, sum + t/n) ? sum : exp_impl(x, sum + t/n, n * i, i+1, t * x);
}
template <typename T> constexpr T exp(T x) noexcept
{
    static_assert(std::is_floating_point<T>::value, "x must be floating point number");
    return exp_impl(x, T{1}, T{1}, 2, x);
}

// log(fp,fp)
template <typename T>
constexpr T log_iter(T x, T y)
{
    return y + T{2} * (x - exp(y)) / (x + exp(y));
}

template <typename T>
constexpr T log(T x, T y)
{
    return equal_fp(y, log_iter(x, y)) ? y : log(x, log_iter(x, y));
}

// pow(fp, integer) pow(fp, fp)
template <typename T, typename U,
          typename std::enable_if< std::is_integral<U>::value, std::nullptr_t>::type = nullptr>
constexpr T pow(T x, U y) noexcept
{
    static_assert(std::is_arithmetic<T>::value, "x must be arithmetic type");
    return (y == 0) ? T{1} :
            y == 1 ? x :
            y > 1 ?
            ((y & 1) ? x * pow(x, y-1) : pow(x, y/2) * pow(x,y/2)) :
            T{1} / pow(x, -y);
}

template <typename T,
          typename std::enable_if< std::is_floating_point<T>::value, std::nullptr_t>::type = nullptr>
constexpr T pow(T x, T y) noexcept
{
    static_assert(std::is_arithmetic<T>::value, "x must be arithmetic type");
    return (y == std::numeric_limits<T>::infinity()) ? std::numeric_limits<T>::infinity() :
            (y == -std::numeric_limits<T>::infinity()) ? T{0} : exp(log(x, T{constants::e}) * y);
}

// sin(fp)

template <typename T> constexpr T trig_series(T x, T sum, T n, int i, int s, T t) noexcept
{
    return equal_fp(sum ,sum + t*s/n) ? sum :
            trig_series(x, sum + t*s/n, n*i*(i+1), i+2, -s, t*x*x);
}

template <typename T> constexpr T sin(const T x) noexcept
{
    return trig_series(x, x, T{6}, 4, -1, x*x*x);
}

template <typename T> constexpr T cos(const T x) noexcept
{
    return trig_series(x, T{1}, T{2}, 3, -1, x*x);
}


#else

# pragma message "Using GCC-extended arithmetic functions"
template<typename T> constexpr T sqrt(const T x) { return std::sqrt(x); }
template<typename T> constexpr T pow(T x, T y) { return std::pow(x, y); }
template<typename T> constexpr T pow(T x, int y) { return std::pow(x, y); }
template<typename T> constexpr T sin(const T x) { return std::sin(x); }
template<typename T> constexpr T cos(const T x) { return std::cos(x); }

#endif
///@endcond
}//

///@name Easing behavior
///@note Argument t [0.0 ~ 1.0]
///@warning No range check of values is performed, so check on the side to be passed on.
///@{

//! @brief Linear
constexpr float linear(const float t)
{
    return 1.0f * t;
}

//! @brief Quadratic in
constexpr float quadratic_in(const float t)
{
    return 1.0f * t * t;
}

//! @brief Quadratic out
constexpr float quadratic_out(const float t)
{
    return -1.0f * t * (t - 2.0f);
}

//! @brief Quadratic in-out
constexpr float quadratic_inout(const float t)
{
    return (t * 2.0f) < 1.0f ?
            0.5f * (t * 2.0f) * (t * 2.0f) :
            -0.5f * (((t * 2.0f) - 1.0f) * (((t * 2.0f) - 1.0f) - 2.0f) - 1.0f);
}

//! @brief Cubic in
constexpr float cubic_in(const float t)
{
    return 1.0f * t * t * t;
}

//! @brief Cubic out
constexpr float cubic_out(const float t)
{
    return 1.0f * ((t - 1.0f) * (t - 1.0f) * (t - 1.0f) + 1.0f);
}

//! @brief Cubic in-out
constexpr float cubic_inout(const float t)
{
    return (t * 2.0f) < 1.0f ?
            0.5f * (t * 2.0f) * (t * 2.0f ) * (t * 2.0f) :
            0.5f * ((t * 2.0f - 2.0f) * (t * 2.0f - 2.0f) * (t * 2.0f - 2.0f) + 2.0f);
}

//! @brief Quartic in
constexpr float quartic_in(const float t)
{
    return 1.0f * t * t * t * t;
}

//! @brief Quartic out
constexpr float quartic_out(const float t)
{
    return -1.0f * ((t - 1.0f) * (t - 1.0f) * (t - 1.0f) * (t - 1.0f) - 1.0f);
}

//! @brief Quartic in-out
constexpr float quartic_inout(const float t)
{
    return (t * 2.0f) < 1.0f ?
            0.5f * (t * 2.0f) * (t * 2.0f) * (t * 2.0f) * (t * 2.0f) :
            -0.5f * ((t * 2.0f - 2.0f) * (t * 2.0f - 2.0f) * (t * 2.0f - 2.0f) * (t * 2.0f - 2.0f) - 2.0f);
}

//! @brief Quintic in
constexpr float quintic_in(const float t)
{
    return 1.0f * t * t * t * t * t;
}

//! @brief Quintic out
constexpr float quintic_out(const float t)
{
    return 1.0f * ((t - 1.0f) * (t - 1.0f) * (t - 1.0f) * (t - 1.0f) * (t - 1.0f) + 1.0f);
}

//! @brief Quintic in-out
constexpr float quintic_inout(const float t)
{
    return (t * 2.0f) < 1.0f ?
            0.5f * (t * 2.0f) * (t * 2.0f) * (t * 2.0f) * (t * 2.0f) * (t * 2.0f) :
            0.5f * ((t * 2.0f - 2.0f) * (t * 2.0f - 2.0f) * (t * 2.0f - 2.0f) * (t * 2.0f - 2.0f) * (t * 2.0f - 2.0f) + 2.0f);
}

//! @brief Sinusoidal in
constexpr float sinusoidal_in(const float t)
{
    return -1.0f * math::cos(t * constants::half_pi) + 1.0f;
}

//! @brief Sinusoidal out
constexpr float sinusoidal_out(const float t)
{
    return 1.0f * math::sin(t * constants::half_pi);
}

//! @brief Sinusoidal in-out
constexpr float sinusoidal_inout(const float t)
{
    return -0.5f * (math::cos(t * constants::pi) - 1.0f);
}

//! @brief Exponential in
constexpr float exponential_in(const float t)
{
    return math::equal_fp(t, 0.0f) ? 0.0f : 1.0f * math::pow(2.0f, 10.0f * (t - 1.0f));
}

//! @brief Exponential out
constexpr float exponential_out(const float t)
{
    return math::equal_fp(t, 1.0f) ? 1.0f : 1.0f * (-math::pow(2.0f, -10.0f * t) + 1.0f);
}

//! @brief Exponential in-out
constexpr float exponential_inout(const float t)
{
    return math::equal_fp(t, 0.0f) ? 0.0f :
            math::equal_fp(t, 1.0f) ? 1.0f :
            (t * 2.0f) < 1.0f ?
            0.5f * math::pow(2.0f, 10.0f * (t * 2.0f - 1.0f)) :
            0.5f * (-math::pow(2.0f, -10.0f * (t * 2.0f - 1.0f)) + 2.0f);
}

//! @brief Circular in
constexpr float circular_in(const float t)
{
    return -1.0f * (math::sqrt(1.0f - t * t) - 1.0f);
}

//! @brief Circular out
constexpr float circular_out(const float t)
{
    return 1.0f * math::sqrt(1.0f - (t - 1.0f) * (t - 1.0f));
}

//! @brief Circular in-out
constexpr float circular_inout(const float t)
{
    return (t * 2.0f) < 1.0f ?
            -0.5f * (math::sqrt(1.0f - (t * 2.0f) * (t * 2.0f)) - 1.0f) :
            0.5f * (math::sqrt(1.0f - (t * 2.0f - 2.0f) * (t * 2.0f - 2.0f)) + 1.0f);
}

//! @brief Back in
constexpr float back_in(const float t)
{
    return 1.0f * t * t * ((constants::back_factor + 1.0f ) * t - constants::back_factor);
}

//! @brief Back out
constexpr float back_out(const float t)
{
    return 1.0f * ((t - 1.0f)* (t - 1.0f) * ((constants::back_factor + 1.0f ) * (t - 1.0f) + constants::back_factor) + 1.0f);
}

//! @brief Back in-out
constexpr float back_inout(const float t)
{
    return (t * 2.0f) < 1.0f ?
            0.5f * ((t * 2.0f) * (t * 2.0f) * ((constants::back_factor2 + 1.0f) * (t * 2.0f) - constants::back_factor2) ) :
            0.5f * ((t * 2.0f - 2.0f) * (t * 2.0f - 2.0f) * ((constants::back_factor2 + 1.0f) * (t * 2.0f - 2.0f) + constants::back_factor2) + 2.0f);
}

//! @brief Elastic in
constexpr float elastic_in(const float t)
{
    return (t <= 0.0f) ? 0.0f :
            (t >= 1.0f) ? 1.0f :
            -math::pow(2.0f, 10.0f * t - 10.0f) * math::sin((t * 10.0f - 10.75f) * constants::elastic_factor);
}

//! @brief Elastic out
constexpr float elastic_out(const float t)
{
    return (t <= 0.0f) ? 0.0f :
            (t >= 1.0f) ? 1.0f :
            math::pow(2.0f, -10.0f * t) * math::sin((t * 10.0f - 0.75f) * constants::elastic_factor) + 1.0f;
}

//! @brief Elastic in-out
constexpr float elastic_inout(const float t)
{
    return (t <= 0.0f) ? 0.0f :
            (t >= 1.0f) ? 1.0f :
            t < 0.5f ?
            -0.5f * (math::pow(2.0f, 20.0f * t - 10.0f) * math::sin((20.0f * t - 11.125f) * constants::elastic_factor2)) :
            0.5f * (math::pow(2.0f, -20.0f * t + 10.0f) * math::sin((20.0f * t - 11.125f) * constants::elastic_factor2)) + 1.0f;
}

//! @brief Bounce in
constexpr float bounce_out(const float t)
{
    return t < (1.0f / constants::bounce_factor) ? constants::bounce_factor2 * t * t :
            t < (2.0f / constants::bounce_factor) ? constants::bounce_factor2 * (t - (1.5f / constants::bounce_factor)) * (t - (1.5f / constants::bounce_factor)) + 0.75f :
            t < (2.5f / constants::bounce_factor) ? constants::bounce_factor2 * (t - (2.25f / constants::bounce_factor)) * (t- (2.25f / constants::bounce_factor)) + 0.9375f :
            constants::bounce_factor2 * (t - (2.625f / constants::bounce_factor)) * (t - (2.625f / constants::bounce_factor)) + 0.984375f;
}

//! @brief Bounce out
constexpr float bounce_in(const float t)
{
    return 1.0f - bounce_out(1.0f - t);
}

//! @brief Bounce in-out
constexpr float bounce_inout(const float t)
{
    return t < 0.5f ? (1.0f - bounce_out(1.0f - 2.0f * t)) * 0.5f
            :  (1.0f + bounce_out(2.0f * t - 1.0f)) * 0.5f;
}
/// @}
}}
#endif


#include <gtest/gtest.h>
#include <gob_easing.hpp>
#include <cmath>
#include <limits>

using namespace goblib;

#if defined(GOBLIB_EASING_USING_OWN_MATH)
namespace
{

template<typename T> constexpr T deg2rad(T deg)
{
    static_assert(std::is_floating_point<T>::value, "T must be floating point");
    return deg * (M_PI / T{180});
}

// Approximately equal
template<typename T> bool fp_eq(const T a, const T b)
{
    static_assert(std::is_floating_point<T>::value, "T must be floating point");
    return (fabs(a) != std::numeric_limits<T>::infinity() && fabs(b) != std::numeric_limits<T>::infinity()) ?
            (fabs(a - b) <= T{0.00001} ? true :
             fabs(a - b) <= ( (fabs(a) < fabs(b) ? fabs(b) : fabs(a)) * std::numeric_limits<T>::epsilon()))
            : true;
}
//
}

// -------------------------------------
// sqrt
namespace
{
template<typename T> void test_sqrt()
{
    constexpr T table[] =  { 0.0, 0.5,      1.0, 2.0,      4.0f, std::numeric_limits<T>::infinity(), -0.0f };
    constexpr T answer[] = { 0.0, 0.707107, 1.0, 1.414214, 2.0f, std::numeric_limits<T>::infinity(), -0.0f };
    static_assert(sizeof(table)/sizeof(table[0]) == sizeof(answer)/sizeof(answer[0]), "Oops");

    for(size_t i=0;i<sizeof(table)/sizeof(table[0]);++i)
    {
        auto s = std::sqrt(table[i]);
        auto e = easing::math::sqrt(table[i]);
        EXPECT_TRUE(fp_eq(s, e)) << table[i] << answer[i];
    }
    // for negative (Notice: NAN == NAN make false)
    {
        auto s = std::sqrt(-1.0f);
        auto e = easing::math::sqrt(-1.0f);
        EXPECT_FALSE(s == s);
        EXPECT_FALSE(e == e);
        EXPECT_FALSE(s == e);
    }
}
//
}

TEST(own_math, sqrt)
{
    // constexpr
    constexpr float cf = easing::math::sqrt(2.0f);
    constexpr double cd = easing::math::sqrt(2.0);
    constexpr long double cld = easing::math::sqrt(2.0L);
    EXPECT_TRUE(fp_eq(std::sqrt(2.0f), cf));
    EXPECT_TRUE(fp_eq(std::sqrt(2.0), cd));
    EXPECT_TRUE(fp_eq(std::sqrt(2.0L), cld));

    test_sqrt<float>();
    test_sqrt<double>();
    test_sqrt<long double>();
}    

// -------------------------------------
// pow
namespace
{
template<typename T> void test_pow()
{
    {
        auto s = std::pow(T{2.0}, std::numeric_limits<T>::infinity());
        constexpr auto e = easing::math::pow(T{2.0}, std::numeric_limits<T>::infinity());
        EXPECT_TRUE(fp_eq(s,e));
    }
    {
        auto s = std::pow(T{2.0}, T{2.0});
        constexpr auto e = easing::math::pow(T{2.0}, T{2.0});
        EXPECT_TRUE(fp_eq(s,e));
    }
    {
        auto s = std::pow(T{2.0}, T{1.0});
        constexpr auto e = easing::math::pow(T{2.0}, T{1.0});
        EXPECT_TRUE(fp_eq(s,e));
    }
    {
        auto s = std::pow(T{2.0}, T{0.5});
        constexpr auto e = easing::math::pow(T{2.0}, T{0.5});
        EXPECT_TRUE(fp_eq(s,e));
    }
    {
        auto s = std::pow(T{2.0}, T{0.0});
        constexpr auto e = easing::math::pow(T{2.0}, T{0.0});
        EXPECT_TRUE(fp_eq(s,e));
    }
    {
        auto s = std::pow(T{2.0}, T{-0.5});
        constexpr auto e = easing::math::pow(T{2.0}, T{-0.5});
        EXPECT_TRUE(fp_eq(s,e));
    }
    {
        auto s = std::pow(T{2.0}, T{-1.0f});
        constexpr auto e = easing::math::pow(T{2.0}, T{-1.0f});
        EXPECT_TRUE(fp_eq(s,e));
    }
    {
        auto s = std::pow(T{2.0}, T{-2.0});
        constexpr auto e = easing::math::pow(T{2.0}, T{-2.0});
        EXPECT_TRUE(fp_eq(s,e));
    }
    {
        auto s = std::pow(T{2.0}, -std::numeric_limits<T>::infinity());
        constexpr auto e = easing::math::pow(T{2.0}, -std::numeric_limits<T>::infinity());
        EXPECT_TRUE(fp_eq(s,e));
    }
}
//
}

TEST(own_math, power)
{
    // contexpr
    {
        constexpr float  cf = easing::math::pow(2.0f, 0.5f);
        constexpr double cd = easing::math::pow(2.0, 0.5);
        constexpr long double cld = easing::math::pow(2.0L, 0.5L);
        EXPECT_TRUE(fp_eq(std::pow(2.0f, 0.5f), cf));
        EXPECT_TRUE(fp_eq(std::pow(2.0,  0.5),  cd));
        EXPECT_TRUE(fp_eq(std::pow(2.0L, 0.5L), cld));
    }

    // exp is integer
    {
        for(int i = 0;i < 32;++i)
        {
            float f = (float)i;
            double d = (double)i;
            long double ld = (long double)i;
            EXPECT_TRUE(fp_eq((float)std::pow( f, i), easing::math::pow( f, i))) << f << " | " << i;
            EXPECT_TRUE(fp_eq(std::pow( d, i), easing::math::pow( d, i))) << d << " | " << i;
            EXPECT_TRUE(fp_eq(std::pow(ld, i), easing::math::pow(ld, i))) << ld << " | " << i;
        }
    }
    // floating point number
    test_pow<float>();
    test_pow<double>();
    test_pow<long double>();
}

// -------------------------------------
// sin/cos
TEST(own_math, sincos)
{
    for(int i = 0;i < 360; ++i)
    {
        {
            float f = deg2rad((float)i);
            EXPECT_TRUE(fp_eq(std::sin(f), easing::math::sin(f)))
                    << i << " | " << f << " : " << std::sin(f) << " / " << easing::math::sin(f);
            EXPECT_TRUE(fp_eq(std::cos(f), easing::math::cos(f)))
                    << i << " | " << f << " : " << std::cos(f) << " / " << easing::math::cos(f);
        }
        {
            double d = deg2rad((double)i);
            EXPECT_TRUE(fp_eq(std::sin(d), easing::math::sin(d)))
                    << i << " | " << d << " : " << std::sin(d) << " / " << easing::math::sin(d);
            EXPECT_TRUE(fp_eq(std::cos(d), easing::math::cos(d)))
                    << i << " | " << d << " : " << std::cos(d) << " / " << easing::math::cos(d);
        }
        {
            long double ld = deg2rad((long double)i);
            EXPECT_TRUE(fp_eq(std::sin(ld), easing::math::sin(ld)))
                    << i << " | " << ld << " : " << std::sin(ld) << " / " << easing::math::sin(ld);
            EXPECT_TRUE(fp_eq(std::cos(ld), easing::math::cos(ld)))
                    << i << " | " << ld << " : " << std::cos(ld) << " / " << easing::math::cos(ld);
        }
    }
}
#endif

// -------------------------------------
// easing
namespace
{
using ease_function_float = float(*)(const float);
ease_function_float table_float[] =
{
    goblib::easing::linear<float>,
    goblib::easing::inSinusoidal<float>,
    goblib::easing::outSinusoidal<float>,
    goblib::easing::inOutSinusoidal<float>,
    goblib::easing::inQuadratic<float>,
    goblib::easing::outQuadratic<float>,
    goblib::easing::inOutQuadratic<float>,
    goblib::easing::inCubic<float>,
    goblib::easing::outCubic<float>,
    goblib::easing::inOutCubic<float>,
    goblib::easing::inQuartic<float>,
    goblib::easing::outQuartic<float>,
    goblib::easing::inOutQuartic<float>,
    goblib::easing::inQuintic<float>,
    goblib::easing::outQuintic<float>,
    goblib::easing::inOutQuintic<float>,
    goblib::easing::inExponential<float>,
    goblib::easing::outExponential<float>,
    goblib::easing::inOutExponential<float>,
    goblib::easing::inCircular<float>,
    goblib::easing::outCircular<float>,
    goblib::easing::inOutCircular<float>,
    goblib::easing::inBack<float>,
    goblib::easing::outBack<float>,
    goblib::easing::inOutBack<float>,
    goblib::easing::inElastic<float>,
    goblib::easing::outElastic<float>,
    goblib::easing::inOutElastic<float>,
    goblib::easing::inBounce<float>,
    goblib::easing::outBounce<float>,
    goblib::easing::inOutBounce<float>,
};

using ease_function_double = double(*)(const double);
ease_function_double table_double[] =
{
    goblib::easing::linear<double>,
    goblib::easing::inSinusoidal<double>,
    goblib::easing::outSinusoidal<double>,
    goblib::easing::inOutSinusoidal<double>,
    goblib::easing::inQuadratic<double>,
    goblib::easing::outQuadratic<double>,
    goblib::easing::inOutQuadratic<double>,
    goblib::easing::inCubic<double>,
    goblib::easing::outCubic<double>,
    goblib::easing::inOutCubic<double>,
    goblib::easing::inQuartic<double>,
    goblib::easing::outQuartic<double>,
    goblib::easing::inOutQuartic<double>,
    goblib::easing::inQuintic<double>,
    goblib::easing::outQuintic<double>,
    goblib::easing::inOutQuintic<double>,
    goblib::easing::inExponential<double>,
    goblib::easing::outExponential<double>,
    goblib::easing::inOutExponential<double>,
    goblib::easing::inCircular<double>,
    goblib::easing::outCircular<double>,
    goblib::easing::inOutCircular<double>,
    goblib::easing::inBack<double>,
    goblib::easing::outBack<double>,
    goblib::easing::inOutBack<double>,
    goblib::easing::inElastic<double>,
    goblib::easing::outElastic<double>,
    goblib::easing::inOutElastic<double>,
    goblib::easing::inBounce<double>,
    goblib::easing::outBounce<double>,
    goblib::easing::inOutBounce<double>,
};

using ease_function_long_double = long double(*)(const long double);
ease_function_long_double table_long_double[] =
{
    goblib::easing::linear<long double>,
    goblib::easing::inSinusoidal<long double>,
    goblib::easing::outSinusoidal<long double>,
    goblib::easing::inOutSinusoidal<long double>,
    goblib::easing::inQuadratic<long double>,
    goblib::easing::outQuadratic<long double>,
    goblib::easing::inOutQuadratic<long double>,
    goblib::easing::inCubic<long double>,
    goblib::easing::outCubic<long double>,
    goblib::easing::inOutCubic<long double>,
    goblib::easing::inQuartic<long double>,
    goblib::easing::outQuartic<long double>,
    goblib::easing::inOutQuartic<long double>,
    goblib::easing::inQuintic<long double>,
    goblib::easing::outQuintic<long double>,
    goblib::easing::inOutQuintic<long double>,
    goblib::easing::inExponential<long double>,
    goblib::easing::outExponential<long double>,
    goblib::easing::inOutExponential<long double>,
    goblib::easing::inCircular<long double>,
    goblib::easing::outCircular<long double>,
    goblib::easing::inOutCircular<long double>,
    goblib::easing::inBack<long double>,
    goblib::easing::outBack<long double>,
    goblib::easing::inOutBack<long double>,
    goblib::easing::inElastic<long double>,
    goblib::easing::outElastic<long double>,
    goblib::easing::inOutElastic<long double>,
    goblib::easing::inBounce<long double>,
    goblib::easing::outBounce<long double>,
    goblib::easing::inOutBounce<long double>,
};

const char* name[] =
{
    "linear",
    "inSinusoidal",
    "outSinusoidal",
    "inOutSinusoidal",
    "inQuadratic",
    "outQuadratic",
    "inOutQuadratic",
    "inCubic",
    "outCubic",
    "inOutCubic",
    "inQuartic",
    "outQuartic",
    "inOutQuartic",
    "inQuintic",
    "outQuintic",
    "inOutQuintic",
    "inExponential",
    "outExponential",
    "inOutExponential",
    "inCircular",
    "outCircular",
    "inOutCircular",
    "inBack",
    "outBack",
    "inOutBack",
    "inElastic",
    "outElastic",
    "inOutElastic",
    "inBounce",
    "outBounce",
    "inOutBounce",
};
static_assert(sizeof(name)/sizeof(name[0]) == sizeof(table_float)/sizeof(table_float[0]), "oops!");
static_assert(sizeof(name)/sizeof(name[0]) == sizeof(table_double)/sizeof(table_double[0]), "oops!");
static_assert(sizeof(name)/sizeof(name[0]) == sizeof(table_long_double)/sizeof(table_long_double[0]), "oops!");
//
}

TEST(easing, basic)
{
    constexpr int32_t iteration_max = 12345;

    // float
    {
        int idx = 0;
        for(auto& func : table_float)
        {
            auto v = func(float{0});
            bool b = std::is_same<float, decltype(v) >::value;

            EXPECT_TRUE(b) << name[idx];
            EXPECT_NEAR(0.0f, func(0.0f), FLT_EPSILON) << name[idx];
            EXPECT_NEAR(1.0f, func(1.0f), FLT_EPSILON) << name[idx];
            //EXPECT_FLOAT_EQ(float{0}, func(float{0})) << name[idx];
            //EXPECT_FLOAT_EQ(float{1}, func(float{1})) << name[idx];

            for(int32_t i = 0; i < iteration_max; ++i)
            {
                float t = (float)i / iteration_max;
                EXPECT_TRUE(std::isfinite(func(t))) << name[idx] << " | " << i;
            }
            ++idx;
        }
    }
    // double
    {
        int idx = 0;
        for(auto& func : table_double)
        {
            auto v = func(double{0});
            bool b = std::is_same<double, decltype(v) >::value;

            EXPECT_TRUE(b) << name[idx];
            EXPECT_NEAR(0.0, func(0.0), DBL_EPSILON) << name[idx];
            EXPECT_NEAR(1.0, func(1.0), DBL_EPSILON) << name[idx];
            //EXPECT_DOUBLE_EQ(double{0}, func(double{0})) << name[idx];
            //EXPECT_DOUBLE_EQ(double{1}, func(double{1})) << name[idx];
            for(int32_t i = 0; i < iteration_max; ++i)
            {
                double t = (double)i / iteration_max;
                EXPECT_TRUE(std::isfinite(func(t))) << name[idx] << " | " << i;
            }
            ++idx;
        }
    }
    // long double
    {
        int idx = 0;
        for(auto& func : table_long_double)
        {
            auto v = func(0.0L);
            bool b = std::is_same<long double, decltype(v) >::value;
            EXPECT_TRUE(b) << name[idx];
#if defined(GOBLIB_EASING_USING_OWN_MATH)
            EXPECT_DOUBLE_EQ(0.0L, func(0.0L)) << name[idx];
            EXPECT_DOUBLE_EQ(1.0L, func(1.0L)) << name[idx];
#else
            EXPECT_NEAR(0.0L, func(0.0L), LDBL_EPSILON) << name[idx];
            EXPECT_NEAR(1.0L, func(1.0L), LDBL_EPSILON) << name[idx];
#endif
            
            for(int32_t i = 0; i < iteration_max; ++i)
            {
                long double t = (long double)i / iteration_max;
                EXPECT_TRUE(std::isfinite(func(t))) << name[idx] << " | " << i;
            }
            ++idx;
        }
    }
}

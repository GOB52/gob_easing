
#include <gtest/gtest.h>
#include <gob_easing.hpp>
#include <cmath>
#include <limits>
#include <random>

auto rng = std::default_random_engine {};
using namespace goblib;

namespace
{
std::uniform_real_distribution<> dist(0.0f, 10000.0f);

template<typename T> constexpr T deg2rad(T deg)
{
    static_assert(std::is_floating_point<T>::value, "T must be floating point");
    return deg * (M_PI / T{180});
}

// Match to n decimal places
template<unsigned int DP = 4> bool match_fp(const float x, const float y)
{
    long int mp = std::pow(10L, DP);
    return static_cast<long int>(std::round(x * mp)) == static_cast<long int>(std::round(y * mp));
}


}//

#if defined(GOBLIB_EASING_USING_OWN_MATH)
// sqrt
TEST(own_math, sqrt)
{
    // constexpr
    constexpr double cd = easing::math::sqrt(2.0);
    constexpr float cf = easing::math::sqrt(2.0f);
    EXPECT_FLOAT_EQ(cd, 1.414214);
    EXPECT_FLOAT_EQ(cf, 1.414214f);

    // double
    {
        constexpr double table[] =  { 0.0, 0.5,      1.0, 2.0,      4.0, std::numeric_limits<double>::infinity(), -0.0 };
        constexpr double answer[] = { 0.0, 0.707107, 1.0, 1.414214, 2.0, std::numeric_limits<double>::infinity(), -0.0 };
        static_assert(sizeof(table)/sizeof(table[0]) == sizeof(answer)/sizeof(answer[0]), "Oops");

        for(size_t i=0;i<sizeof(table)/sizeof(table[0]);++i)
        {
            auto s = std::sqrt(table[i]);
            auto e = easing::math::sqrt(table[i]);
            EXPECT_FLOAT_EQ(s, e) << table[i] << answer[i];
        }

        // for negative (Notice: NAN == NAN make false)
        {
            auto s = std::sqrt(-1.0);
            auto e = easing::math::sqrt(-1.0);
            EXPECT_FALSE(s == s);
            EXPECT_FALSE(e == e);
            EXPECT_FALSE(s == e);
        }
    }

    // float
    {
        constexpr float table[] =  { 0.0f, 0.5f,      1.0f, 2.0f,      4.0f, std::numeric_limits<float>::infinity(), -0.0f };
        constexpr float answer[] = { 0.0f, 0.707107f, 1.0f, 1.414214f, 2.0f, std::numeric_limits<float>::infinity(), -0.0f };
        static_assert(sizeof(table)/sizeof(table[0]) == sizeof(answer)/sizeof(answer[0]), "Oops");

        for(size_t i=0;i<sizeof(table)/sizeof(table[0]);++i)
        {
            auto s = std::sqrt(table[i]);
            auto e = easing::math::sqrt(table[i]);
            EXPECT_FLOAT_EQ(s, e) << table[i] << answer[i];
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
}    

// pow
TEST(own_math, power)
{
    //
    {
        constexpr double ce = easing::math::pow(2.0, 0.5);
        constexpr float  cf = easing::math::pow(2.0f, 0.5f);
        EXPECT_FLOAT_EQ(ce, 1.414214);
        EXPECT_FLOAT_EQ(cf, 1.414214f);
    }

    // exp is integer
    {
        for(int i = 0;i < 32;++i)
        {
            auto v = dist(rng);
            EXPECT_FLOAT_EQ(std::pow(v, i), easing::math::pow(v, i)) << v << " | " << i;
        }
    }

    // exp is floating point number
    {
        auto s = std::pow(2.0, std::numeric_limits<double>::infinity());
        constexpr auto e = easing::math::pow(2.0, std::numeric_limits<double>::infinity());
        EXPECT_FLOAT_EQ(s,e);
    }
    {
        auto s = std::pow(2.0,2.0);
        constexpr auto e = easing::math::pow(2.0, 2.0);
        EXPECT_FLOAT_EQ(s,e);
    }
    {
        auto s = std::pow(2.0,1.0);
        constexpr auto e = easing::math::pow(2.0, 1.0);
        EXPECT_FLOAT_EQ(s,e);
    }
    {
        auto s = std::pow(2.0,0.5);
        constexpr auto e = easing::math::pow(2.0, 0.5);
        EXPECT_FLOAT_EQ(s,e);
    }
    {
        auto s = std::pow(2.0,0.0);
        constexpr auto e = easing::math::pow(2.0, 0.0);
        EXPECT_FLOAT_EQ(s,e);
    }
    {
        auto s = std::pow(2.0,-0.5);
        constexpr auto e = easing::math::pow(2.0, -0.5);
        EXPECT_FLOAT_EQ(s,e);
    }
    {
        auto s = std::pow(2.0,-1.0);
        constexpr auto e = easing::math::pow(2.0, -1.0);
        EXPECT_FLOAT_EQ(s,e);
    }
    {
        auto s = std::pow(2.0,-2.0);
        constexpr auto e = easing::math::pow(2.0, -2.0);
        EXPECT_FLOAT_EQ(s,e);
    }
    {
        auto s = std::pow(2.0, -std::numeric_limits<double>::infinity());
        constexpr auto e = easing::math::pow(2.0, -std::numeric_limits<double>::infinity());
        EXPECT_FLOAT_EQ(s,e);
    }
}

// sin/cos
TEST(own_math, sincos)
{
    for(int i = 0;i < 360; i += 5)
    {
        auto rad = deg2rad((float)i);
        //EXPECT_FLOAT_EQ(std::sin(rad), goblib::easing::math::sin(rad)) << i << " | " << rad;
        EXPECT_TRUE(match_fp(std::sin(rad), easing::math::sin(rad)))
                << i << " | " << rad << " : " << std::sin(rad) << " / " << easing::math::sin(rad);
        EXPECT_TRUE(match_fp(std::cos(rad), easing::math::cos(rad)))
                << i << " | " << rad << " : " << std::cos(rad) << " / " << easing::math::cos(rad);
    }
}
#endif

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
            EXPECT_NEAR(float{0}, func(float{0}), FLT_EPSILON) << name[idx];
            EXPECT_NEAR(float{1}, func(float{1}), FLT_EPSILON) << name[idx];

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
            EXPECT_NEAR(double{0}, func(double{0}), DBL_EPSILON) << name[idx];
            EXPECT_NEAR(double{1}, func(double{1}), DBL_EPSILON) << name[idx];

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
            EXPECT_NEAR(0.0L, func(0.0L), LDBL_EPSILON) << name[idx];
            EXPECT_NEAR(1.0L, func(1.0L), LDBL_EPSILON) << name[idx];

            for(int32_t i = 0; i < iteration_max; ++i)
            {
                long double t = (long double)i / iteration_max;
                EXPECT_TRUE(std::isfinite(func(t))) << name[idx] << " | " << i;
            }
            ++idx;
        }
    }
}

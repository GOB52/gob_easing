
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

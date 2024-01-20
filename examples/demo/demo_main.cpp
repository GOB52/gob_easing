/*
  gob_easing demo
 */
#include <M5Unified.h>
#include <gob_easing.hpp>

// Choose floating point number type
using fp_type = float;
//using fp_type = double;
//using fp_type = long double;

using sample_function = void(*)(size_t);
using ease_function = fp_type(*)(const fp_type);

namespace
{
auto& lcd = M5.Display;

ease_function table[] =
{
    goblib::easing::linear<fp_type>,
    goblib::easing::inSinusoidal<fp_type>,
    goblib::easing::outSinusoidal<fp_type>,
    goblib::easing::inOutSinusoidal<fp_type>,
    goblib::easing::inQuadratic<fp_type>,
    goblib::easing::outQuadratic<fp_type>,
    goblib::easing::inOutQuadratic<fp_type>,
    goblib::easing::inCubic<fp_type>,
    goblib::easing::outCubic<fp_type>,
    goblib::easing::inOutCubic<fp_type>,
    goblib::easing::inQuartic<fp_type>,
    goblib::easing::outQuartic<fp_type>,
    goblib::easing::inOutQuartic<fp_type>,
    goblib::easing::inQuintic<fp_type>,
    goblib::easing::outQuintic<fp_type>,
    goblib::easing::inOutQuintic<fp_type>,
    goblib::easing::inExponential<fp_type>,
    goblib::easing::outExponential<fp_type>,
    goblib::easing::inOutExponential<fp_type>,
    goblib::easing::inCircular<fp_type>,
    goblib::easing::outCircular<fp_type>,
    goblib::easing::inOutCircular<fp_type>,
    goblib::easing::inBack<fp_type>,
    goblib::easing::outBack<fp_type>,
    goblib::easing::inOutBack<fp_type>,
    goblib::easing::inElastic<fp_type>,
    goblib::easing::outElastic<fp_type>,
    goblib::easing::inOutElastic<fp_type>,
    goblib::easing::inBounce<fp_type>,
    goblib::easing::outBounce<fp_type>,
    goblib::easing::inOutBounce<fp_type>,
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
constexpr size_t table_size = sizeof(table)/sizeof(table[0]);
size_t current_ease = 0;

void drawEaseGraph(const size_t cur)
{
    int16_t left = 16;
    int16_t top = lcd.height()/4;
    int16_t right = lcd.width() - 16;
    int16_t bottom = lcd.height() - lcd.height()/4;
    int16_t wid = right - left + 1;
    int16_t hgt = bottom - top + 1;

    using point_t = std::pair<int16_t,int16_t>;  // point (x,y)
    std::vector<point_t> points;
    using pp_t = std::pair<point_t, point_t>; // 2 points for line (x1, y1), (x2, y2)
    std::vector<pp_t> pairs;
    
    lcd.drawLine(left, top, left, bottom, TFT_WHITE);
    lcd.drawLine(left, bottom, right, bottom, TFT_WHITE);
    
    for(int16_t x = 0; x <= wid; ++x)
    {
        fp_type t = (fp_type)x / wid; // Clamp between 0.0 and 1.0
        auto e = table[cur](t);   // Call ease function
        auto y = bottom - hgt * e;
        points.push_back(std::make_pair(left + x, y));
    }
    assert(!points.empty() && "points empty");
    // transform to 2 points vector from single point vector.
    // { e0, e1, e2, .... en } => { {e0, e1}, {e1, e2}, ... }
    std::transform(points.begin(), std::prev(points.end()),
                   std::next(points.begin()),
                   std::back_inserter(pairs),
                   std::make_pair<const point_t&, const point_t&>);
    for(auto& p : pairs)
    {
        // draw line from p.first to p.second
        lcd.drawLine(p.first.first, p.first.second, p.second.first, p.second.second, TFT_WHITE);
    }
    
    lcd.drawString(name[cur], 0,0);
}

void drawEaseGradient(const size_t cur)
{
    int16_t left = 16;
    int16_t top = 16;
    int16_t right = lcd.width() - 16;
    int16_t bottom = lcd.height() - 16;
    int16_t hgt = bottom - top + 1;

    constexpr m5gfx::rgb888_t fclr(0x14, 0x73, 0xe6);
    constexpr m5gfx::rgb888_t tclr(0x24, 0x7b, 0x5e);
    
    for(int16_t y=0;y <= hgt; ++y)
    {
        fp_type t = (fp_type)y / hgt; // Clamp between 0.0 and 1.0
        auto e = table[cur](t);   // Call ease function

        auto r = fclr.r + (tclr.r - fclr.r) * e;
        auto g = fclr.g + (tclr.g - fclr.g) * e;
        auto b = fclr.b + (tclr.b - fclr.b) * e;
        
        lcd.drawLine(left, top + y, right, top + y, m5gfx::rgb888_t(r,g,b));
    }
    lcd.drawString(name[cur], 0,0);
}

int32_t counter{};
constexpr int16_t radius = 16;
constexpr int32_t complete_frames = 90;

void drawEaseAction(const size_t cur)
{
    constexpr int16_t fx = radius;
    int16_t tx = lcd.width() - radius;

    if(counter > complete_frames) { counter = 0; lcd.clear(TFT_BLACK); }
    fp_type t = (fp_type)counter / (complete_frames - 30);
    t = std::fmin(std::fmax(t, fp_type{0}), fp_type{1}); // Clamp between 0.0 and 1.0
    auto e = table[cur](t);   // Call ease function

    int16_t ox = fx + (tx - fx) * e;

    lcd.drawFastVLine(fx,                   32, lcd.height()/2 - radius - 32, TFT_GREEN);
    lcd.drawFastVLine(lcd.width() - radius, 32, lcd.height()/2 - radius - 32, TFT_GREEN);
    lcd.fillRect(0, lcd.height()/2 - radius, lcd.width(), radius * 2 + 1, TFT_BLACK);

    lcd.fillCircle(ox, lcd.height()/2, radius, TFT_ORANGE);
    lcd.drawPixel(ox, lcd.height()/2 + lcd.height()/4, TFT_WHITE);

    lcd.drawString(name[cur], 0,0);

    lgfx::delay(1000/60);
    ++counter;
}

sample_function sf_table[] = { drawEaseGraph, drawEaseGradient, drawEaseAction };
constexpr size_t sf_table_size = sizeof(sf_table)/sizeof(sf_table[0]);
size_t current_sample = 0;
//
}

void setup()
{
    M5.begin();

    lcd.startWrite();
    lcd.setFont(&fonts::Font2);
    lcd.clear(TFT_BLACK);
    
    sf_table[current_sample](current_ease);
}

void loop()
{
    M5.update();
    if(M5.BtnA.wasClicked())
    {
        current_ease = (current_ease + table_size - 1) % table_size;
        counter = 0;
        lcd.clear(TFT_BLACK);
    }
    else if(M5.BtnC.wasClicked())
    {
        current_ease = (current_ease + 1) % table_size;
        counter = 0;
        lcd.clear(TFT_BLACK);
    }
    if(M5.BtnB.wasClicked())
    {
        current_sample = (current_sample + 1) % sf_table_size;
        counter = 0;
        lcd.clear(TFT_BLACK);
    }
    sf_table[current_sample](current_ease);
}

#if defined ( SDL_h_ )
__attribute__((weak)) int user_func(bool* running)
{
    setup();
    do
    {
        loop();
    } while (*running);
    return 0;
}

int main(int, char**)
{
    // The second argument is effective for step execution with breakpoints.
    // You can specify the time in milliseconds to perform slow execution that ensures screen update
    return lgfx::Panel_sdl::main(user_func, 128);
}
#endif

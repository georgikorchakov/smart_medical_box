#ifndef WS2812B_H
#define WS2812B_H

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <signal.h>
#include <stdarg.h>
#include <getopt.h>


// #include <ws2811/clk.h>
// #include <ws2811/gpio.h>
// #include <ws2811/dma.h>
// #include <ws2811/pwm.h>
#include <ws2811/ws2811.h>

#define TARGET_FREQ             WS2811_TARGET_FREQ
#define GPIO_PIN                12
#define DMA                     10
//#define STRIP_TYPE            WS2811_STRIP_RGB		// WS2812/SK6812RGB integrated chip+leds
#define STRIP_TYPE              WS2811_STRIP_GBR		// WS2812/SK6812RGB integrated chip+leds
//#define STRIP_TYPE            SK6812_STRIP_RGBW		// SK6812RGBW (NOT SK6812RGB)

#define LED_COUNT               45

class WS2812B
{
private:
    ws2811_t __ledstring = 
    {
        .freq = TARGET_FREQ,
        .dmanum = DMA,
        .channel =
        {
            [0] =
            {
                .gpionum = GPIO_PIN,
                .invert = 0,
                .count = LED_COUNT,
                .strip_type = STRIP_TYPE,
                .brightness = 255,
            },
            [1] =
            {
                .gpionum = 0,
                .invert = 0,
                .count = 0,
                .brightness = 0,
            },
        },
    };

    ws2811_led_t *__matrix;

public:
    WS2812B(int led_count=45);
    ~WS2812B();
    int x_y_to_led_number(int x, int y);
    void set_led_number(int led_number, uint32_t color);
    void set_box_led(int x, int y, uint32_t color);
    void set_box_whole_line(int y, uint32_t color);
    void set_box_whole_column(int x, uint32_t color);
    void draw_line(int x_from, int x_to, int y, uint32_t color);
    void bulgarian_flag_test();
    void open_frame_animation(int x, int y, uint32_t color_box, uint32_t color_light);
    void close_frame_animation(uint32_t color_box);
    void snake_animation_top_to_bottom(int x, int y, uint32_t color_snake, uint32_t color_light);
    void snake_animation_bottom_to_top(uint32_t color_snake, uint32_t color_light);
    void clear();
    int render();
};


#endif // WS2812B_H
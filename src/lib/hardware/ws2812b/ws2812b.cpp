#include "./ws2812b.h"

WS2812B::WS2812B(int led_count)
{
    __ledstring.channel[0].count = led_count;

    // __matrix = (ws2811_led_t*)malloc(sizeof(ws2811_led_t) * led_count);
    __matrix = new ws2811_led_t[led_count];

    for (int i = 0; i < led_count; ++i)
    {
        __matrix[i] = 0;
    }

    ws2811_return_t ret;
    
    if ((ret = ws2811_init(&__ledstring)) != WS2811_SUCCESS)
    {
        fprintf(stderr, "ws2811_init failed: %s\n", ws2811_get_return_t_str(ret));
    }
}

WS2812B::~WS2812B()
{
    clear();
    render();
    ws2811_fini(&__ledstring);
    delete[] __matrix;
}

int WS2812B::x_y_to_led_number(int x, int y)
{
    int led_number = 0;

    if (y == 0)
    {
        led_number = 44 - x;
    }
    else if (y == 1)
    {
        led_number = 29 + x;
    }
    else if (y == 2)
    {
        led_number = 28 - x;
    }
    else if (y == 3)
    {
        led_number = 13 + x;
    }
    else if (y == 4)
    {
        led_number = 12 - x; 
    }
    else if (y == 4)
    {
        led_number = 12 - x; 
    }
    else if (y == 5)
    {
        led_number = x - 3; 
    }

    return led_number;
}

void WS2812B::set_box_led(int x, int y, uint32_t color)
{
    ////////////////////////////////////////
    // 0 1 2 x
    // 1
    // y

    int led_number = x_y_to_led_number(x, y);

    if (led_number >= 0)
    { 
        set_led_number(led_number, color);
    }
}

void WS2812B::set_box_whole_line(int y, uint32_t color)
{
    for (int x = 0; x < 8; ++x)
    {
        set_box_led(x, y, color);
    }
}

void WS2812B::set_box_whole_column(int x, uint32_t color)
{
    for (int y = 0; y < 6; ++y)
    {
        set_box_led(x, y, color);
    }
}

void WS2812B::draw_line(int x_from, int x_to, int y, uint32_t color)
{
    for (int x = x_from; x <= x_to; ++x)
    {
        set_box_led(x, y, color);
    }
}

void WS2812B::open_frame_animation(int x, int y, uint32_t color_box, uint32_t color_light)
{
    for (int i = 0; i < 4; ++i)
    {
        if (i != 0)
        {
            set_box_led(4 - i, 0, color_box);
            set_box_led(4 - i, 1, 0);
            set_box_led(4 - i, 2, 0);
            set_box_led(4 - i, 3, 0);
            set_box_led(4 - i, 4, color_box);

            set_box_led(3 + i, 0, color_box);
            set_box_led(3 + i, 1, 0);
            set_box_led(3 + i, 2, 0);
            set_box_led(3 + i, 3, 0);
            set_box_led(3 + i, 4, color_box);
        }

        // Left side animation (full line)
        set_box_led(3 - i, 0, color_box);
        set_box_led(3 - i, 1, color_box);
        set_box_led(3 - i, 2, color_box);
        set_box_led(3 - i, 3, color_box);
        set_box_led(3 - i, 4, color_box);

        // Right side animation (full line)
        set_box_led(4 + i, 0, color_box);
        set_box_led(4 + i, 1, color_box);
        set_box_led(4 + i, 2, color_box);
        set_box_led(4 + i, 3, color_box);
        set_box_led(4 + i, 4, color_box);

        render();
        usleep(1000000);
    }

    set_box_led(x, y, color_light);
    render();
}

void WS2812B::close_frame_animation(uint32_t color_box)
{
    for (int i = 0; i < 4; ++i)
    {
        if (i != 3)
        {
            set_box_led(1 + i, 0, color_box);
            set_box_led(1 + i, 1, color_box);
            set_box_led(1 + i, 2, color_box);
            set_box_led(1 + i, 3, color_box);
            set_box_led(1 + i, 4, color_box);

            set_box_led(6 - i, 0, color_box);
            set_box_led(6 - i, 1, color_box);
            set_box_led(6 - i, 2, color_box);
            set_box_led(6 - i, 3, color_box);
            set_box_led(6 - i, 4, color_box);
        }

        // Left side animation (full line)
        set_box_led(i, 0, 0);
        set_box_led(i, 1, 0);
        set_box_led(i, 2, 0);
        set_box_led(i, 3, 0);
        set_box_led(i, 4, 0);

        // Right side animation (full line)
        set_box_led(7 - i, 0, 0);
        set_box_led(7 - i, 1, 0);
        set_box_led(7 - i, 2, 0);
        set_box_led(7 - i, 3, 0);
        set_box_led(7 - i, 4, 0);

        render();
        usleep(1000000 / 2);
    }
}

void WS2812B::snake_animation_top_to_bottom(int x, int y, uint32_t color_snake, uint32_t color_light)
{
    clear();

    uint32_t color_snake_head = color_light;

    for (int i = 44; i >= 0; --i)
    {
        if ((i != 44) && (i+1 != x_y_to_led_number(x, y)))
        {
            set_led_number(i + 1, color_snake);
        }

        if (x_y_to_led_number(x, y) == i)
        {
            set_led_number(i, color_light);
            color_snake_head = color_snake;
        }
        else
        {
            set_led_number(i, color_snake_head);
        }

        render();
        usleep(100000);
    }

    for (int i = 44; i >= 0; --i)
    {
        if ((i != x_y_to_led_number(x, y)) 
            && (i != 10)
            && (i != 11)
            && (i != 12))
        {
            set_led_number(i, 0);
        }

        render();
        usleep(100000);
    }

}

void WS2812B::snake_animation_bottom_to_top(uint32_t color_snake, uint32_t color_light)
{
    uint32_t color_snake_head = color_snake;

    for (int i = 0; i < 45; ++i)
    {
        if (__matrix[i] == color_light)
        {
            color_snake_head = color_light;
        }

        set_led_number(i, color_snake_head);

        if (i != 0)
        {
            set_led_number(i - 1, color_snake);
        }

        render();
        usleep(100000);
    }

    set_led_number(44, color_snake);
    render();
    usleep(100000);

    for (int i = 0; i < 45; ++i)
    {
        set_led_number(i, 0);
        render();
        usleep(100000);
    }
}

void WS2812B::bulgarian_flag_test()
{
    for (int i = 6; i >= 0; --i)
    {
        for (int y = 0; y < i; y++)
        {

            if (y != 0)
            {
                set_box_whole_line(y - 1, 0);
            }
            render();
            usleep(100000);

            if ((i == 5) || (i == 6))
            {
                set_box_whole_line(y, 0x0000FF);
            }
            else if ((i == 3) || (i == 4))
            {
                set_box_whole_line(y, 0x00FF00);
            }
            else if ((i == 1) || (i == 2))
            {
                set_box_whole_line(y, 0xFFFFFF);
            }

            render();
            usleep(1000000 / 4);
        }
    }
    usleep(1000000);
}

void WS2812B::set_led_number(int led_number, uint32_t color)
{
    __matrix[led_number] = color;
}

void WS2812B::clear()
{
    for (int i = 0; i < __ledstring.channel[0].count; ++i)
    {
        __matrix[i] = 0;
    }
}

int WS2812B::render()
{
    for (int i = 0; i <  __ledstring.channel[0].count; ++i)
    {
        __ledstring.channel[0].leds[i] = __matrix[i];
    }

    ws2811_return_t ret;
    if ((ret = ws2811_render(&__ledstring)) != WS2811_SUCCESS)
    {
        fprintf(stderr, "ws2811_render failed: %s\n", ws2811_get_return_t_str(ret));
        return 1;
    }

    return 0;
}

// g++ example.cpp ../ws2812b.cpp -lws2811 -o example

#include "../ws2812b.h"

int main()
{
    WS2812B led;

    // led.set_box_whole_line(0, 0xFFFFFF);
    // led.set_box_whole_line(1, 0xFFFFFF);

    // led.set_box_whole_line(2, 0x00FF00);
    // led.set_box_whole_line(3, 0x00FF00);

    // led.set_box_whole_line(0, 0xFFFFFF);
    // led.set_box_whole_line(4, 0xFFFFFF);
    // led.set_box_whole_column(0, 0xFFFFFF);
    // led.set_box_whole_column(7, 0xFFFFFF);
    

    // led.bulgarian_flag_test();
    // usleep(2000000);

    // led.clear();
    // led.render();

    // led.open_box_animation(0, 0, 0xffc738, 0xffffff); //b g r
    // usleep(2000000);

    led.snake_animation_top_to_bottom(3, 3, 0xff0000, 0x0000ff);
    
    return 0;
}
#ifndef SCREEN_H
#define SCREEN_H

#include<SDL2/SDL.h>
#include<mutex>

class Screen
{
private:
    SDL_Window* window = nullptr;
    SDL_Renderer* renderer = nullptr;
    SDL_Texture* texture = nullptr;
    Uint32* buffer = nullptr;
    Uint32* buffer1 = nullptr;
    bool ready = false;
    int SCREEN_WIDTH;
    int SCREEN_HEIGHT;

    bool init_screen();
    void clean_resources();
    void horizontal_conv(std::mutex& t_lock);
    void vertical_conv(std::mutex& t_lock);

public:
    Screen();
    Screen(int width,int height);
    ~Screen();
    void update_screen();
    void clear_screen();
    void box_blur();
    bool update_pixel(int x,int y,Uint8 red,Uint8 green,Uint8 blue);
    bool is_ready();
    bool close();
    int get_width();
    int get_height();
};

#endif // SCREEN_H

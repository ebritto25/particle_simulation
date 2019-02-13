#include "screen.h"
#include<iostream>
#include<thread>
#include<mutex>

Screen::Screen():SCREEN_WIDTH(800),
                 SCREEN_HEIGHT(600)
{
    this->ready = init_screen();
}

Screen::Screen(int width,int height):SCREEN_WIDTH(width),
                                     SCREEN_HEIGHT(height)
{
    this->ready = init_screen();
}

Screen::~Screen()
{
    clean_resources();
}

bool Screen::init_screen()
{
    if(SDL_Init(SDL_INIT_VIDEO) < 0) {return false;}

    this->window = SDL_CreateWindow("Particle Simulation",
                                    SDL_WINDOWPOS_UNDEFINED,
                                    SDL_WINDOWPOS_UNDEFINED,
                                    this->SCREEN_WIDTH,
                                    this->SCREEN_HEIGHT,
                                    SDL_WINDOW_SHOWN);

    if(!this->window) { clean_resources(); return false; }

    this->renderer = SDL_CreateRenderer(this->window,
                                        -1,
                                        SDL_RENDERER_PRESENTVSYNC);

    if(!this->renderer) { clean_resources(); return false; }

    this->texture = SDL_CreateTexture(this->renderer,
                                      SDL_PIXELFORMAT_RGBA8888,
                                      SDL_TEXTUREACCESS_STATIC,
                                      this->SCREEN_WIDTH,
                                      this->SCREEN_HEIGHT);

    if(!this->texture) { clean_resources(); return false; }

    this->buffer = new Uint32[this->SCREEN_WIDTH*this->SCREEN_HEIGHT];
    this->buffer1 = new Uint32[this->SCREEN_WIDTH*this->SCREEN_HEIGHT];

    if(!this->buffer) { clean_resources(); return false; }

    memset(this->buffer,0,
           this->SCREEN_WIDTH*this->SCREEN_HEIGHT*sizeof(Uint32));

    memset(this->buffer1,0,
           this->SCREEN_WIDTH*this->SCREEN_HEIGHT*sizeof(Uint32));

    update_screen();

    return true;
}

void Screen::clean_resources()
{
    if(this->window)
        SDL_DestroyWindow(this->window);
    if(this->renderer)
        SDL_DestroyRenderer(this->renderer);
    if(this->texture)
        SDL_DestroyTexture(this->texture);
    if(this->buffer)
        delete [] buffer;
    SDL_Quit();
}

void Screen::update_screen()
{
    SDL_UpdateTexture(this->texture,nullptr,this->buffer,
                      this->SCREEN_WIDTH*sizeof(Uint32));
    SDL_RenderClear(this->renderer);
    SDL_RenderCopy(this->renderer,this->texture,nullptr,nullptr);
    SDL_RenderPresent(this->renderer);
}

void Screen::clear_screen()
{
    memset(this->buffer,0,
          this->SCREEN_WIDTH*this->SCREEN_HEIGHT*sizeof(Uint32));
}

void Screen::horizontal_conv(std::mutex& t_lock)
{
    int end = this->SCREEN_HEIGHT*this->SCREEN_WIDTH;
    Uint32* temp = new Uint32[end];
    for(int pos = 0;pos < end;pos++)
    {
        int red = 0, green = 0, blue = 0;
        for(int i = -1,k_pos = 0;i <= 1;i++,k_pos++)
        {
            int b_pos = pos + i;
            if(b_pos >= 0 && b_pos < end)
            {
                Uint32 value = this->buffer[b_pos];
                red += ((value & 0xFF000000) >> 24);
                green += ((value & 0x00FF0000) >> 16);
                blue += ((value & 0x0000FF00) >> 8);
            }
        }

        red /= 3;
        green /= 3;
        blue /= 3;

        temp[pos] = (red<<24) + (green<<16) + (blue<<8) + 0xFF;
    }

	std::lock_guard<std::mutex> guard(t_lock);
    delete [] this->buffer;
    this->buffer = temp;
}

void Screen::vertical_conv(std::mutex& t_lock)
{
    int end = this->SCREEN_HEIGHT*this->SCREEN_WIDTH;
    Uint32* temp = new Uint32[end];
    for(int pos = 0;pos < end;pos++)
    {
        int red = 0, green = 0, blue = 0;
        for(int i = -1,k_pos = 0;i <= 1;i++,k_pos++)
        {
            int b_pos = pos + (i*this->SCREEN_WIDTH);
            if(b_pos >= 0 && b_pos < end)
            {
                Uint32 value = this->buffer[b_pos];
                red += ((value & 0xFF000000) >> 24);
                green += ((value & 0x00FF0000) >> 16);
                blue += ((value & 0x0000FF00) >> 8);
            }
        }

        red /= 3;
        green /= 3;
        blue /= 3;

        temp[pos] = (red<<24) + (green<<16) + (blue<<8) + 0xFF;
    }

	std::lock_guard<std::mutex> guard(t_lock);
    delete [] this->buffer;
    this->buffer = temp;
}

/*
this function is too slow, i left it here just to show a diferent
way of doing a box blur

void Screen::box_blur()
{
    Uint32* temp = this->buffer;
    this->buffer = this->buffer1;
    this->buffer1 = temp;

    for(int y = 0;y < this->SCREEN_HEIGHT;y++)
    {
        for(int x = 0;x < this->SCREEN_WIDTH;x++)
        {
            int r_total = 0;
            int g_total = 0;
            int b_total = 0;

            for(int row = -1;row <= 1;row++)
            {
                for(int col = -1;col <= 1;col++)
                {
                    int currentX = col + x;
                    int currentY = row + y;

                    if(currentX > 0 && currentX < this->SCREEN_WIDTH &&
                       currentY > 0 && currentY < this->SCREEN_HEIGHT)
                    {
                        Uint32 color = this->buffer1[currentY * this->SCREEN_WIDTH + currentX];

                        Uint32 red =   (color & 0xFF000000) >> 24;
                        Uint32 green = (color & 0x00FF0000) >> 16;
                        Uint32 blue =  (color & 0x0000FF00) >> 8;

                        r_total += red;
                        g_total += green;
                        b_total += blue;
                    }
                }
            }
            update_pixel(x,y,r_total/9,g_total/9,b_total/9);

        }

    }
}
*/

void Screen::box_blur()
{
    /* this new function uses separable filters and it's faster
     * than the old one
     */
	std::mutex t_lock;
	std::thread horizontal([this,&t_lock]() {horizontal_conv(t_lock);});
    std::thread vertical([this,&t_lock]() {vertical_conv(t_lock);});

	horizontal.join();
	vertical.join();
}


bool Screen::update_pixel(int x,int y,Uint8 red,Uint8 green,Uint8 blue)
{
    if((x < 0)||(x >= this->SCREEN_WIDTH) ||
       (y < 0)||(y >= this->SCREEN_HEIGHT))
    {
        return false;
    }

    Uint32 value = (red<<24) + (green<<16) + (blue<<8) + (255);
    this->buffer[y * this->SCREEN_WIDTH + x] = value;

    return true;
}

bool Screen::is_ready()
{
    return this->ready;
}

bool Screen::close()
{
    SDL_Event event;
    SDL_PollEvent(&event);

    if(event.type == SDL_QUIT)
        return true;
    else
        return false;
}

int Screen::get_width()
{
    return this->SCREEN_WIDTH;
}

int Screen::get_height()
{
    return this->SCREEN_HEIGHT;
}

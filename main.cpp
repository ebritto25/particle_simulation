#include<iostream>
#include<math.h>
#include"screen.h"
#include"swarm.h"

#define DEBUG_ENV 0

#if DEBUG_ENV == 1
    #define DEBUG(X) std::cout << X << "\n";
#else
    #define DEBUG(X)
#endif

int main()
{
    std::cout << "Particle simulation\n";
    Screen screen;
    Swarm swarm(5000);
    Particle* particles = swarm.get_particles();

    if(!screen.is_ready()) {
        std::cout << "Screen Initialization Failed\n";
        return -1;
    }

    const int screen_x_center = screen.get_width()/2;
    const int screen_y_center = screen.get_height()/2;

    int delta = 0;
    while(true)
    {
        Uint32 time = SDL_GetTicks();
        Uint8 red = (Uint8)((1 + std::sin((time * 0.0001)))*128);
        Uint8 green = (Uint8)((1 + std::sin((time * 0.0005)))*128);
        Uint8 blue = (Uint8)((1 + std::sin((time * 0.0007)))*128);

        long particle_number = swarm.get_particle_number();

        for(int i = 0, x = 0, y = 0;i < particle_number;i++)
        {
            x = (particles[i].x + 1) * screen_x_center;
            y = particles[i].y * screen_x_center + screen_y_center;

            screen.update_pixel(x,y,red,green,blue);
        }

        screen.box_blur();
        screen.update_screen();

        delta = SDL_GetTicks() - time;
        swarm.update_particles_position(delta);

        if(screen.close()) { break; }

        DEBUG("Looptime(milliseconds) ~= " << delta)
        DEBUG("Frames ~= " << 1000/delta)
    }


    return 0;
}

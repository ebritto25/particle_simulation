#ifndef SWARM_H
#define SWARM_H

#include<random>
#include<math.h>

struct Particle
{
    double x;
    double y;
    double direction;
    double speed;

    Particle():x(0),y(0)
    {
        std::random_device dev;
        std::mt19937 gen(dev());
        std::uniform_real_distribution<> dir_dist(0.0,2.0*M_PI);
        std::uniform_real_distribution<> speed_dist(0.0,1.0);

        this->direction = dir_dist(gen);
        this->speed = speed_dist(gen);

    }
};

class Swarm
{
    Particle* particles;
    long n_particles;
public:
    Swarm(long particle_number);
    ~Swarm();
    Particle* get_particles();
    long get_particle_number();
    void update_particles_position(int delta);
};
#endif // SWARM_H

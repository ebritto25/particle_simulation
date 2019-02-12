#include "swarm.h"
#include<iostream>

Swarm::Swarm(long particle_number)
{
    n_particles = particle_number;
    particles = new Particle[(unsigned long)particle_number];
}
Swarm::~Swarm()
{
    delete [] particles;
}
long Swarm::get_particle_number()
{
    return this->n_particles;
}

Particle* Swarm::get_particles()
{
    return this->particles;
}

void Swarm::update_particles_position(int delta)
{
    double velocity = 0.0001;
    double direction_inc = 0.0003;
    Particle* start = this->particles;
    Particle* end = this->particles + this->n_particles;
    for(Particle* particle = start;particle < end;particle++)
    {
        double x_inc = particle->speed*cos(particle->direction);
        double y_inc = particle->speed*sin(particle->direction);

        particle->direction += delta*direction_inc;
        particle->x += velocity * x_inc * delta;
        particle->y += velocity * y_inc * delta;

        if(particle->x < -1.0 || particle->x >= 1.0) {
            particle->speed = -particle->speed;
        }
        if(particle->y < -1.0 || particle->y >= 1.0) {
            particle->speed = -particle->speed;
        }
    }
}

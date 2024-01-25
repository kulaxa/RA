#include <glm/vec3.hpp>
#include <vector>
#include "particle.h"

namespace ParticleSystem{

    class ParticleSystem {
    public:
        ParticleSystem(glm::vec3 center, float particleSize, float particleLife, float particleSpeed, float gravity);
        glm::vec3 center;
        float particleSize;
        float particleLife;
        float particleSpeed;
        float gravity;
        glm::vec3 startColor;
        glm::vec3 endColor;
        std::vector<Particle::Particle> particles;  // Collection to store multiple particles



        void create_particles(int i);
        Particle::Particle generate_particle(Particle::Particle particle);
        Particle::Particle generate_particle();
        void update_particles(float delta_time);
        void recreateDeadParticles();
    };

}
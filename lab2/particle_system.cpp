#include <glm/vec3.hpp>
#include "particle_system.h"
#include "utils.h"
#include <glm/glm.hpp>
#include <iostream>

namespace ParticleSystem {


    void ParticleSystem::create_particles(int i) {
        for (int j = 0; j < i; j++) {

            particles.push_back(generate_particle());
        }
    }

    void ParticleSystem::recreateDeadParticles() {
        for (auto& particle : particles) {
            if (particle.life > 0.999f) {
                particle = generate_particle(particle);
            }
        }
    }

    void ParticleSystem::update_particles(float delta_time) {
        for (auto& particle : particles) {
            // apply gravity
            particle.velocity.y -= 0.1f * gravity * delta_time;
            particle.position += particle.velocity * delta_time;
            particle.life += delta_time / particleLife;
        }

        }

    Particle::Particle ParticleSystem::generate_particle() {
        auto velocity = particleSpeed* glm::normalize(Utils::randomVec3(-1, 1));
        float life = Utils::randomVec3(0, 1).x;
        float zoom = Utils::randomVec3(particleSize * 0.5f, particleSize * 1.5f).x;
        return Particle::Particle({center,velocity, zoom, life, center, startColor, endColor});
    }

    ParticleSystem::ParticleSystem(glm::vec3 center, float particleSize, float particleLife, float particleSpeed, float gravity) {
        this->center = center;
        this->particleSize = particleSize;
        this->particleLife = particleLife;
        this->particleSpeed = particleSpeed;
        this->gravity = gravity;
    }

    Particle::Particle ParticleSystem::generate_particle(Particle::Particle particle) {
        particle.velocity = particleSpeed* glm::normalize(Utils::randomVec3(-1, 1));
        particle.life = Utils::randomVec3(0, 1).x;
        particle.position = particle.startingPosition;
        particle.startColor = startColor;
        particle.endColor = endColor;
        return particle;
    }


}
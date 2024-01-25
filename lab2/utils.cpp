#include "utils.h"

#include <random>
#include <glm/glm.hpp>

namespace  Utils {

    glm::vec3 randomVec3(float min, float max){
        std::random_device rd;
        std::mt19937 gen(rd());
        std::uniform_real_distribution<float> dis(min, max);
        return glm::vec3(dis(gen), dis(gen), dis(gen));
    }
}
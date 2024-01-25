namespace Particle {
    class Particle {
    public:
        glm::vec3 position;
        glm::vec3 velocity;
        float zoom;
        float life; // 0 means just created, 1 means dead
        glm::vec3 startingPosition;
        glm::vec3 startColor;
        glm::vec3 endColor;
    };

}
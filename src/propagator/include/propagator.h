#ifndef PROPAGATOR_PROPAGATOR_H
#define PROPAGATOR_PROPAGATOR_H

#include <glm/glm.hpp>
#include <vector>
#include "helix.h"

class Propagator {
public:
    float maxR;
    float maxZ;
    float charge;
    std::vector<glm::vec3> points;
    glm::vec3 startVertex;
    glm::vec3 startMomentum;
    float maxOrbs;
    int nMax;

    Helix h;

    Propagator(const glm::vec3& point, const glm::vec3& mom, float charg, int nMax = 4096);

    bool IsOutsideBounds(const glm::vec3& point, float maxRsqr, float mZ);

    void Init();

    void MakeTrack(bool recurse);

    void GoToBounds(glm::vec3& p);

    void Update(const glm::vec3& v, const glm::vec3& p, bool full_update, bool enforce_max_step);

    void Step(const glm::vec4 &vv, const glm::vec3 &p, glm::vec4 &vOut, glm::vec3 &pOut);
    void LoopToBounds(glm::vec3& p);
    glm::vec3 Field(const glm::vec3& p);

    template<glm::length_t L, typename T, glm::qualifier Q>
    T Perp2(const glm::vec<L, T, Q> &vec) const { return vec.x*vec.x + vec.y*vec.y; }

    template<glm::length_t L, typename T, glm::qualifier Q>
    T Perp(const glm::vec<L, T, Q> &vec) const { return glm::sqrt(Perp2(vec)); }

    template<glm::length_t L, typename T, glm::qualifier Q>
    T R(const glm::vec<L, T, Q> &vec) const { return Perp(vec); }
};

#endif //PROPAGATOR_PROPAGATOR_H

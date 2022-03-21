#include "propagator.h"

#include <glm/gtc/constants.hpp>
#include <cstdio>

Propagator::Propagator(const glm::vec3& point, const glm::vec3& mom, float charg, int nMax): maxR(490), maxZ(450), nMax(nMax), maxOrbs(0.5) {
    charge = charg;
    startVertex = point;
    startMomentum = mom;
}

void Propagator::Init() {
    points.push_back(startVertex);
    h.fPhi = 0;
    h.fCharge = charge;
}

bool Propagator::IsOutsideBounds(const glm::vec3& point, float maxRsqr, float mZ) {
    return glm::abs(point.z) > mZ || (point.x*point.x + point.y*point.y) > maxRsqr;
}

void Propagator::MakeTrack(bool recurse) {
    const auto maxRsq = maxR * maxR;

    if (!IsOutsideBounds(startVertex, maxRsq, maxZ)) {
        Init();
        GoToBounds(startMomentum);
    }
}

void Propagator::GoToBounds(glm::vec3& p) {
    Update(startVertex, p, true, false);

    LoopToBounds(p);
}

glm::vec3 Propagator::Field(const glm::vec3& p) {
    return glm::vec3(0, 0, 0.5);
}

void Propagator::Update(const glm::vec3& v, const glm::vec3& p, bool full_update, bool enforce_max_step) {
    h.UpdateHelix(p, Field(v), full_update, enforce_max_step);
}

void Propagator::Step(const glm::vec4 &v, const glm::vec3 &p, glm::vec4 &vOut, glm::vec3 &pOut) {
    h.Step(v, p, vOut, pOut);
}

void Propagator::LoopToBounds(glm::vec3& p) {
    const auto maxRsq = maxR * maxR;

    glm::vec4 currV(startVertex, 0);
    glm::vec4 forwV(startVertex, 0);
    glm::vec3 forwP(p);

    auto np = points.size();
    auto maxPhi = maxOrbs * 2 * glm::pi<float>();

    while (h.fPhi < maxPhi && np < nMax)
    {
        Step(currV, p, forwV, forwP);

        // cross R
        if (Perp2(forwV) > maxRsq)
        {
            float t = (maxR - R(currV)) / (R(forwV) - R(currV));
            if (t < 0 || t > 1)
            {
                printf("HelixToBounds: In MaxR crossing expected t>=0 && t<=1: t=%f, r1=%f, r2=%f, MaxR=%f.",
                       t, R(currV), R(forwV), maxR);
                return;
            }
            glm::vec3 d(forwV);
            d -= glm::vec3(currV);
            d *= t;
            d += glm::vec3(currV);
            points.push_back(d);
            return;
        }

        // cross Z
        else if (glm::abs(forwV.z) > maxZ)
        {
            float t = (maxZ - glm::abs(currV.z)) / glm::abs((forwV.z - currV.z));
            if (t < 0 || t > 1)
            {
                printf("HelixToBounds: In MaxZ crossing expected t>=0 && t<=1: t=%f, z1=%f, z2=%f, MaxZ=%f.",
                       t, currV.z, forwV.z, maxZ);
                return;
            }
            glm::vec3 d(forwV -currV);
            d *= t;
            d += glm::vec3(currV);
            points.push_back(d);
            return;
        }

        currV = forwV;
        p     = forwP;
        Update(currV, p, false, false);

        points.emplace_back(currV);
        ++np;
    }
}

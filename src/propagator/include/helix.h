#ifndef PROPAGATOR_HELIX_H
#define PROPAGATOR_HELIX_H

#include <glm/glm.hpp>

class Helix {
public:
    int   fCharge;   // Charge of tracked particle.
    float fMaxAng;   // Maximum step angle.
    float fMaxStep;  // Maximum allowed step size.
    float fDelta;    // Maximum error in the middle of the step.

    float fPhi;      // Accumulated angle to check fMaxOrbs by propagator.
    bool   fValid;    // Corner case pT~0 or B~0, possible in variable mag field.

    // ----------------------------------------------------------------

    // helix parameters
    float fLam;         // Momentum ratio pT/pZ.
    float fR;           // Helix radius in cm.
    float fPhiStep;     // Caluclated from fMinAng and fDelta.
    float fSin, fCos;   // Current sin/cos(phistep).

    // Runge-Kutta parameters
    float fRKStep;      // Step for Runge-Kutta.

    // cached
    glm::vec3 fB;        // Current magnetic field, cached.
    glm::vec3 fE1, fE2, fE3; // Base vectors: E1 -> B dir, E2->pT dir, E3 = E1xE2.
    glm::vec3 fPt, fPl;  // Transverse and longitudinal momentum.
    float fPtMag;       // Magnitude of pT.
    float fPlMag;       // Momentum parallel to mag field.
    float fLStep;       // Transverse step arc-length in cm.

    // ----------------------------------------------------------------
public:
    Helix();

    void UpdateCommon(const glm::vec3 & p, const glm::vec3& b);
    void UpdateHelix (const glm::vec3 & p, const glm::vec3& b, bool full_update, bool enforce_max_step);

    void Step(const glm::vec4& v, const glm::vec3& p, glm::vec4& vOut, glm::vec3& pOut);
};

#endif //PROPAGATOR_HELIX_H

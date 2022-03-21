#include "helix.h"

namespace
{
    const float fgkB2C    = 0.299792458e-2;
    const float kPtMinSqr = 1e-20;
    const float kAMin     = 1e-10;
}

Helix::Helix() :
        fCharge(0),
        fMaxAng(45.0f), fMaxStep(20.f), fDelta(0.1f),
        fPhi(0.0f), fValid(false),
        fLam(-1), fR(-1), fPhiStep(-1), fSin(-1), fCos(-1),
        fRKStep(20.0f),
        fPtMag(-1), fPlMag(-1), fLStep(-1)
{
}

////////////////////////////////////////////////////////////////////////////////
/// Common update code for helix and RK propagation.

void Helix::UpdateCommon(const glm::vec3& p, const glm::vec3& b)
{
    fB = b;

    // base vectors
    fE1 = glm::normalize(b);
    fPlMag = glm::dot(p, fE1);
    fPl    = fE1*fPlMag;
    fPt    = p - fPl;
    fPtMag = glm::length(fPt);
    fE2    = glm::normalize(fPt);
}

////////////////////////////////////////////////////////////////////////////////
/// Update helix parameters.

void Helix::UpdateHelix(const glm::vec3& p, const glm::vec3& b,
                        bool full_update, bool enforce_max_step) {
    UpdateCommon(p, b);

    // helix parameters
    fE3 = glm::cross(fE1, fE2);

    if (fCharge < 0) {
        fE3 = -fE3;
    }

    if (full_update) {
        float a = fgkB2C * glm::length(b) * glm::abs(fCharge);
        if (a > kAMin && fPtMag*fPtMag > kPtMinSqr) {
            fValid = true;

            fR   = glm::abs(fPtMag / a);
            fLam = fPlMag / fPtMag;

            // get phi step, compare fMaxAng with fDelta
            fPhiStep = glm::radians(fMaxAng);
            if (fR > fDelta) {
                float ang  = 2.0f * glm::acos(1.0f - fDelta/fR);
                if (ang < fPhiStep) {
                    fPhiStep = ang;
                }
            }

            // check max step size
            float curr_step = fR * fPhiStep * glm::sqrt(1.0f + fLam*fLam);
            if (curr_step > fMaxStep || enforce_max_step) {
                fPhiStep *= fMaxStep / curr_step;
            }

            fLStep = fR * fPhiStep * fLam;
            fSin   = glm::sin(fPhiStep);
            fCos   = glm::cos(fPhiStep);
        } else {
            fValid = false;
        }
    }
}

////////////////////////////////////////////////////////////////////////////////
/// Step helix for given momentum p from vertex v.

void Helix::Step(const glm::vec4& v, const glm::vec3& p,
                 glm::vec4& vOut, glm::vec3& pOut) {
    vOut = v;

    if (fValid) {
        glm::vec3 d = fE2*(fR*fSin) + fE3*(fR*(1-fCos)) + fE1*fLStep;
        vOut    += glm::vec4(d, 0);
        vOut.w += glm::abs(fLStep);

        pOut = fPl + fE2*(fPtMag*fCos) + fE3*(fPtMag*fSin);

        fPhi += fPhiStep;
    } else {
        // case: pT < kPtMinSqr or B < kBMin
        // might happen if field directon changes pT ~ 0 or B becomes zero
        vOut    += glm::vec4(p * (fMaxStep / glm::length(p)), 0);
        vOut.w += fMaxStep;
        pOut  = p;
    }
}

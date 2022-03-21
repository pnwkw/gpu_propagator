#include "propagator_config.glsl"

bool valid = false;
float PhiStep = -1, LStep = -1, Sin = -1, Cos = -1, Phi = 0, PlMag, PtMag;
float R = -1, Lam = -1;
vec3 Pl, Pt;
vec3 E1, E2, E3;

bool IsOutsideBounds(const vec3 point, const float maxRsqr, const float mZ) {
    return abs(point.z) > mZ || dot(point.xy, point.xy) > maxRsqr;
}

void UpdateCommon(const vec3 p, const vec3 b) {
    E1 = normalize(b);
    PlMag = dot(p, E1);
    Pl = E1 * PlMag;
    Pt = p - Pl;
    PtMag = length(Pt);
    E2 = normalize(Pt);
}

void Update(const vec3 p, const vec3 b, const float charge, const bool full_update, const bool enforce_max_step) {
    UpdateCommon(p, b);

    E3 = cross(E1, E2);

    if (charge < 0) {
        E3 = -E3;
    }

    if (full_update) {
        float a = fgkB2C * length(b) * abs(charge);
        if (a > kAMin && PtMag*PtMag > kPtMinSqr) {
            valid = true;

            R   = abs(PtMag / a);
            Lam = PlMag / PtMag;

            // get phi step, compare MaxAng with Delta
            PhiStep = radians(MaxAng);
            if (R > Delta) {
                float ang  = 2.0f * acos(1.0f - Delta/R);
                if (ang < PhiStep) {
                    PhiStep = ang;
                }
            }

            // check max step size
            float curr_step = R * PhiStep * sqrt(1.0f + Lam*Lam);
            if (curr_step > MaxStep || enforce_max_step) {
                PhiStep *= MaxStep / curr_step;
            }

            LStep = R * PhiStep * Lam;
            Sin   = sin(PhiStep);
            Cos   = cos(PhiStep);
        } else {
            valid = false;
        }
    }
}

void StepHelix(const vec4 v, const vec3 p, out vec4 vOut, out vec3 pOut) {
    if (valid) {
        vec3 d = E2 * (R * Sin) + E3 * (R * (1 - Cos)) + E1 * LStep;

        vOut = v + vec4(d, abs(LStep));
        pOut = Pl + E2 * (PtMag * Cos) + E3 * (PtMag * Sin);
        Phi += PhiStep;
    } else {
        // case: pT < kPtMinSqr or B < kBMin
        // might happen if field directon changes pT ~ 0 or B becomes zero
        vOut = v + vec4(p * (MaxStep / length(p)), MaxStep);
        pOut  = p;
    }
}

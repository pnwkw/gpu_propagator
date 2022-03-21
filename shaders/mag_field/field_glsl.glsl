#include "mag_cheb.glsl"

const float SOL_MIN_Z = -550.f;
const float SOL_MAX_Z = 850.f;
const float DIP_MIN_Z = -1760.f;
const float DIP_MAX_Z = -532.46997f;

const float MIN_Z = DIP_MIN_Z;
const float MAX_Z = SOL_MAX_Z;

vec3 CyltoCartCylB(vec3 rphiz, vec3 brphiz) {
    float btr = length(brphiz.xy);
    float psiPLUSphi = atan(brphiz.y, brphiz.x) + rphiz.y;

    return vec3(btr*cos(psiPLUSphi), btr*sin(psiPLUSphi), brphiz.z);
}

vec3 MachineField(vec3 pos) {
    return vec3(0);
}

vec3 SolDipField(vec3 pos) {
    if(pos.z>SOL_MIN_Z) {
        vec3 rphiz = CarttoCyl(pos);
        int segID = findSolSegment(rphiz);
        if(segID >=0 && IsInsideSol(segID, rphiz)) {
            vec3 brphiz = EvalSol(segID, rphiz);
            return CyltoCartCylB(rphiz, brphiz);
        }
    }

    int segID = findDipSegment(pos);
    if(segID >= 0 && IsInsideDip(segID, pos)) {
        return EvalDip(segID, pos);
    }

    return vec3(0);
}

vec3 Field(vec3 pos) {
    if(pos.z > MIN_Z && pos.z < MAX_Z) {
        return SolDipField(pos);
    }
    return MachineField(pos);
}

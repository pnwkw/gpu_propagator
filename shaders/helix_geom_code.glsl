#ifdef BENCHMARK
layout(location = 0) out vec3 outVert;
#endif

int number_points = 0;

void LoopToBounds(const vec3 vtx, const vec3 mmtm, const float charge) {
    const float maxRsq = maxR * maxR;
    const float maxPhi = maxOrbs * 2 * PI;

    vec4 currV = vec4(vtx, 0);
    vec3 currP = mmtm;
    vec4 forwV = vec4(vtx, 0);
    vec3 forwP = vec3(currP);

    while (Phi < maxPhi && number_points < NMAX) {
        StepHelix(currV, currP, forwV, forwP);

        const float RforwVsq = dot(forwV.xy, forwV.xy);

        // cross R
        if (RforwVsq > maxRsq) {
            const float RcurrV = length(currV.xy);
            const float RforwV = sqrt(RforwVsq);

            float t = (maxR - RcurrV) / (RforwV - RcurrV);
            if (t < 0 || t > 1) {
                break;
            }
            vec3 d = mix(currV.xyz, forwV.xyz, t);

#ifdef BENCHMARK
            outVert = d;
#else
            gl_Position = state.MVP * vec4(d, 1);
#endif

            EmitVertex();
#ifdef ADD_PADDING
            break;
#else
            return;
#endif
        }

        // cross Z
        else if (abs(forwV.z) > maxZ) {
            float t = (maxZ - abs(currV.z)) / abs((forwV.z - currV.z));
            if (t < 0 || t > 1) {
#ifdef ADD_PADDING
                break;
#else
                return;
#endif
            }
            vec3 d = mix(currV.xyz, forwV.xyz, t);

#ifdef BENCHMARK
            outVert = d;
#else
            gl_Position = state.MVP * vec4(d, 1);
#endif

            EmitVertex();
#ifdef ADD_PADDING
            break;
#else
            return;
#endif
        }

        currV = forwV;
        currP = forwP;
        Update(currP, Field(currV.xyz) * SCALE, charge, false, false);

#ifdef BENCHMARK
        outVert = currV.xyz;
#else
        gl_Position = state.MVP * vec4(currV.xyz, 1);
#endif

        EmitVertex();
        ++number_points;
    }

#ifdef ADD_PADDING
    for (int i = number_points; i < NMAX; i++) {
#ifdef BENCHMARK
        outVert = vec3(0,0,0);
#else
        gl_Position = state.MVP * vec4(vec3(0,0,0), 1);
#endif
        EmitVertex();
    }
#endif
}

void main(void) {
    const float maxRsq = maxR * maxR;

    const vec3 iVertex = gl_in[0].gl_Position.xyz;
    const vec3 iMomentum = inMomentum[0].xyz;
    const float iCharge = inCharge[0];

    if (!IsOutsideBounds(iVertex, maxRsq, maxZ)) {

#ifdef BENCHMARK
        outVert = iVertex.xyz;
#else
        gl_Position = state.MVP * vec4(iVertex.xyz, 1);
#endif

        EmitVertex();

        Update(iMomentum, Field(iVertex) * SCALE, iCharge, true, false);
        LoopToBounds(iVertex, iMomentum, iCharge);
    }

    EndPrimitive();
}

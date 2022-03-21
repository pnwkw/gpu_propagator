#version 460 core

layout(location = 0) in vec4 inPos;
layout(location = 1) in vec4 inMomentum;
layout(location = 2) in float inCharge;

layout(location = 0) out vec4 outMomentum;
layout(location = 1) out float outCharge;

void main() {
    gl_Position = inPos;
    outMomentum = inMomentum;
    outCharge = inCharge;
}

#version 460 core

#include "shaders_config.glsl"

layout (points) in;
#ifdef BENCHMARK
layout (points, max_vertices = NMAX) out;
#else
layout (line_strip, max_vertices = NMAX) out;
#endif

layout(std140, binding = 0) uniform state_state {
    mat4 MVP;
} state;

layout(location = 0) in vec4 inMomentum[];
layout(location = 1) in float inCharge[];

#include "propagator.glsl"

#include "mag_field/field_const_barrel.glsl"

#include "helix_geom_code.glsl"

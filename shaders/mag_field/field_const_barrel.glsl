const float SOL_MIN_Z = -550.f;
const float SOL_MAX_Z = 570.f;
const float SOL_MAX_R = 500.f;

vec3 CarttoCyl(vec3 pos) {
    return vec3(length(pos.xy), atan(pos.y, pos.x), pos.z);
}

vec3 Field(vec3 pos) {
    vec3 cyl = CarttoCyl(pos);
    if(cyl.z > SOL_MIN_Z && cyl.z < SOL_MAX_Z && cyl.x < SOL_MAX_R) {
        return vec3(0.0f, 0.0f, -5.0f);
    }

    return vec3(0.0f);
}

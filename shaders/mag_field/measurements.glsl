//Values must match measurements.h
const float DETECTOR_CENTER_Z = -455.0f;
const float X_DETECTOR = 512.0f;
const float Y_DETECTOR = 512.0f;
const float Z_DETECTOR = 1312.0f;

const vec3 detector_size = vec3(Z_DETECTOR*2, X_DETECTOR*2, Y_DETECTOR*2);
const vec3 offsets = vec3(Z_DETECTOR - DETECTOR_CENTER_Z, X_DETECTOR - 0.0f, Y_DETECTOR - 0.0f);

const vec3 scales = vec3(4.0f, 4.0f, 4.0f);

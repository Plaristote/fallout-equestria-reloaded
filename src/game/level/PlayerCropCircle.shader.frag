#version 440

layout(location = 0) in vec2 coord;
layout(location = 0) out vec4 fragColor;

layout(std140, binding = 0) uniform buf {
    mat4 qt_Matrix;
    float qt_Opacity;
    vec2 centerPoint;  // Center of the circle in texture coordinates
    float radius;      // Radius of the circle in texture coordinates
};

layout(binding = 1) uniform sampler2D source;

void main() {
    // Vector from center to current pixel
    vec2 centerToPixel = coord - centerPoint;

    // Calculate distance from center
    float distance = length(centerToPixel);

    // Sample the texture
    vec4 texColor = texture(source, coord);

    // Determine visibility based on distance to center
    if (distance > radius) {
        // Outside the circle - make transparent
        fragColor = vec4(texColor.rgb, 0.0);
    } else {
        // Inside the circle - keep original color
        fragColor = texColor * qt_Opacity;
    }
}

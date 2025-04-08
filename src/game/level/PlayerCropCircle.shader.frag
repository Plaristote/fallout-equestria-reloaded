#version 440

layout(location = 0) in vec2 coord;
layout(location = 1) in vec2 texCoord;
layout(location = 0) out vec4 fragColor;

layout(std140, binding = 0) uniform buf {
    mat4 qt_Matrix;
    float qt_Opacity;
    vec2 texSize;      // Texture size in pixels
    vec2 centerPoint;  // Center of the circle in pixel coordinates
    float radius;      // Radius of the circle in pixel coordinates
    float smoothEdge;  // Size of the smooth edge zone, in pixels
    bool withClipping;
};

layout(binding = 1) uniform sampler2D source;

void main() {
    if (withClipping) {
    vec2 pixelCoord = coord * texSize;

    // Vector from center to current pixel
    vec2 centerToPixel = pixelCoord - centerPoint;

    // Calculate distance from center
    float distance = length(centerToPixel);

    // Sample the texture
    vec4 texColor = texture(source, coord);

    // Determine visibility based on distance to center
    //fragColor = texColor * (distance < radius ? qt_Opacity : 0.0);

    // Smooth the edge using smoothstep function
    // Returns 1.0 when distance < radius - smoothEdge
    // Returns 0.0 when distance > radius
    // Returns a smoothly interpolated value in between
    //float alpha = smoothstep(radius, radius - smoothEdge, distance);
    float alpha = smoothstep(radius - smoothEdge, radius, distance);

    // Determine visibility based on distance to center
    fragColor = texColor * alpha * qt_Opacity;
    } else {
        fragColor = texture(source, coord) * qt_Opacity;
    }
}

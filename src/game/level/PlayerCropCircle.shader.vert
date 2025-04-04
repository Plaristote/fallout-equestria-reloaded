#version 440

layout(location = 0) in vec4 vertex;
layout(location = 1) in vec2 texCoord;

layout(location = 0) out vec2 coord;

layout(std140, binding = 0) uniform buf {
    mat4 qt_Matrix;
    float qt_Opacity;
};

void main() {
    coord = texCoord;
    gl_Position = qt_Matrix * vertex;
}

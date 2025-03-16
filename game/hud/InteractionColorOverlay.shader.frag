#version 440

layout(location = 0) in vec2 coord;
layout(location = 0) out vec4 fragColor;

layout(std140, binding = 0) uniform buf {
  mat4 qt_Matrix;
  float qt_Opacity;
  vec4 color;
};

layout(binding = 1) uniform sampler2D source;

void main() {
  vec4 tex = texture(source, coord);
  if (tex.a != 0.0) {
    fragColor = color * qt_Opacity;
  }
  else {
    fragColor = tex * qt_Opacity;
  }
}

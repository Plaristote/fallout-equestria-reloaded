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
  float withAmbientColor = tex.a != 0.0 ? 1.0 : 0.0;
  fragColor = vec4(
    tex.r + withAmbientColor * (color.r + tex.r + tex.r) * 0.3,
    tex.g + withAmbientColor * (color.g + tex.g + tex.g) * 0.3,
    tex.b + withAmbientColor * (color.b + tex.b + tex.b) * 0.3,
    tex.a
  ) * qt_Opacity;
}

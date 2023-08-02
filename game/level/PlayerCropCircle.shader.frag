#version 440

layout(location = 0) in vec2 coord;
layout(location = 0) out vec4 fragColor;

layout(std140, binding = 0) uniform buf {
  mat4 qt_Matrix;
  float qt_Opacity;
  float centerX;
  float centerY;
  float diameter;
  float width;
  float height;
  int shWithClipping;
  int shWithAmbientColor;
  vec4 ambientColor;
};

layout(binding = 1) uniform sampler2D source;

void main() {
  vec4 tex = texture(source, coord);
  float x = coord.x * width;
  float y = coord.y * height;
  float a = pow(x - centerX, 2.0);
  float b = pow(y - centerY, 2.0);
  float radius = diameter / 2.0;
  float pixelClipped     = shWithClipping == 1 && tex.a != 0.0 && a + b < pow(radius, 2.0) ? 0.0 : 1.0;
  float withAmbientColor = shWithAmbientColor == 1 && tex.a != 0.0 ? 1.0 : 0.0;

  fragColor = vec4(
    pixelClipped * (tex.r + withAmbientColor * (ambientColor.r + tex.r + tex.r) * 0.3),
    pixelClipped * (tex.g + withAmbientColor * (ambientColor.g + tex.g + tex.g) * 0.3),
    pixelClipped * (tex.b + withAmbientColor * (ambientColor.b + tex.b + tex.b) * 0.3),
    pixelClipped * tex.a
  ) * qt_Opacity;
}

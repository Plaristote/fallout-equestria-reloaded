#version 440

layout(location = 0) in vec2 coord;
layout(location = 0) out vec4 fragColor;

layout(std140, binding = 0) uniform buf {
  mat4 qt_Matrix;
  float qt_Opacity;
  vec4 color;
  vec2 textureSize;
};

layout(binding = 1) uniform sampler2D source;

void main() {
  vec4 tex = texture(source, coord);
  vec2 pixelSize = vec2(1.0 / 20, 1.0 / 20);

  vec4 pixelRight = texture(source, coord + vec2(pixelSize.x, 0.0));
  vec4 pixelLeft = texture(source, coord - vec2(pixelSize.x, 0.0));
  vec4 pixelTop = texture(source, coord + vec2(0.0, pixelSize.y));
  vec4 pixelBottom = texture(source, coord - vec2(0.0, pixelSize.y));

  bool isBorder = (tex.a > 0.0) && (
    pixelRight.a == 0.0 ||
    pixelLeft.a == 0.0 ||
    pixelTop.a == 0.0 ||
    pixelBottom.a == 0.0
  );

  if (isBorder) {
    fragColor = color * qt_Opacity;
  }
  else {
    fragColor = tex * 0;
  }
}

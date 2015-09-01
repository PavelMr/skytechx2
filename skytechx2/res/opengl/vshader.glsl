#version 150

uniform mat4 mvp_matrix;
uniform float u_mag_limit;

in vec4 a_position;
in vec4 a_colorMagnitude;

out vec4 v_color;
out float v_size;

#pragma include ":/res/opengl/common.glsl"

void main()
{
  v_size = getSize(a_colorMagnitude.w);
  v_color = vec4(a_colorMagnitude.rgb, 1.0);
  gl_Position = mvp_matrix * a_position;
}


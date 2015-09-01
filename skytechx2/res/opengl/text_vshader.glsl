#version 150

uniform mat4 mvp_matrix;

in vec4  a_position;
in vec4  a_uv;
in vec2  a_wh;
in float a_mag;
in float a_align;

out vec4  v_uv;
out vec2  v_wh;
flat out float v_align;
out float v_mag;

void main()
{
  gl_Position = mvp_matrix * a_position;
  v_uv = a_uv;
  v_wh = a_wh;
  v_align = a_align;
  v_mag = a_mag;
}


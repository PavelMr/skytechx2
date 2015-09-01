#version 150

uniform mat4  mvp_matrix;
uniform float scale;

in vec4  a_position;
in float a_u;

out vec4  v_pos;
out float v_u;

void main()
{
  gl_Position = mvp_matrix * a_position;
  v_pos = gl_Position;
  v_u = a_u * 1.0 / scale;
}


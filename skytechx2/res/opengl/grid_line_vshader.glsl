#version 150

uniform mat4 mvp_matrix;

in vec4  a_position;

void main()
{
  gl_Position = mvp_matrix * a_position;
}


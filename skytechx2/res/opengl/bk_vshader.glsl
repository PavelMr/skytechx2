#version 150

uniform mat4 mvp_matrix;

in vec4 a_position;

out vec3 g_texCoord0;

void main()
{
  g_texCoord0 = vec3(-a_position.x, a_position.y, a_position.z);
  gl_Position = mvp_matrix * a_position;
}


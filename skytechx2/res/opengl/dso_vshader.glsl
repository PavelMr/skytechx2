#version 150

uniform mat4 mvp_matrix;
uniform mat4 v_matrix;
uniform mat4 p_matrix;
uniform float max_magnitude;

in vec4  a_position;
in vec4  a_params;
in float a_int;

flat out vec4 g_params;
flat out float g_north;
flat out int  intVal;

void main()
{
  gl_Position = mvp_matrix * a_position;
  g_params = a_params;
  g_north = a_position.y;
  intVal = int(a_int);
}


#version 150

uniform vec3 u_pos;
uniform vec3 u_scale;
uniform mat4 mvp_matrix;
uniform mat4 proj_matrix;
uniform mat4 view_matrix;
uniform mat4 inv_view_matrix;

in vec4 a_position;
in vec2 a_uv;

out vec2 g_uv;

void main()
{
  vec4 tmp = mvp_matrix * vec4(u_pos, 1);
  vec4 pos = inv_view_matrix * (a_position / vec4(u_scale / tmp.w, 1));
  pos.xyz += u_pos;

  gl_Position = mvp_matrix * pos;
  g_uv = a_uv;
}


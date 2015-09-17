#version 150

//uniform vec3 u_pos;
uniform vec3 u_scale;
uniform mat4 mvp_matrix;
uniform mat4 proj_matrix;
uniform mat4 view_matrix;
uniform mat4 inv_view_matrix;

in vec4 a_position;
in vec3 a_uvo;
in vec4 a_center;

out vec2 g_uv;

void main()
{
  vec4 tmp = mvp_matrix * a_center;
  vec4 pos = inv_view_matrix * (a_position / vec4(u_scale / tmp.w, 1));
  pos += a_center;

  gl_Position = mvp_matrix * pos;
  gl_Position.y -= a_uvo[2];
  g_uv = a_uvo.xy;
}


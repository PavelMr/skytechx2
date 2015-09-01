#version 150

uniform mat4 mvp_matrix;
uniform mat4 u_projection;
uniform mat4 u_modelview;

uniform float u_scr_width;
uniform float u_scr_height;

uniform float max_magnitude;

layout(points) in;
layout(triangle_strip, max_vertices = 8) out;

in vec4 v_color[];
in float v_size[];

out vec4 f_color;
out vec2 f_uv;
flat out int  f_texture;

void main()
{
  if (v_size[0] < 0.001)
  {
    return;
  }

  vec4 scale = vec4(v_size[0] * u_scr_width,
                    v_size[0] * u_scr_height, 0, 0) * gl_in[0].gl_Position.w;


  vec4 p[4];

  vec4 P = gl_in[0].gl_Position;

  p[0] = vec4(-1, 1, 0, 0);
  p[1] = vec4(1, 1, 0, 0);
  p[2] = vec4(-1, -1, 0, 0);
  p[3] = vec4(1, -1, 0, 0);

  /*
  for (int i = 0; i < 4; i++)
  {
    float x, y;
    float s = sin(P.x);
    float c = cos(P.x);

    x = p[i].x * c - p[i].y * s;
    y = p[i].x * s + p[i].y * c;

    p[i].x = x;
    p[i].y = y;
  }
  */

  gl_Position = P + p[0] * scale;
  f_uv = vec2(0, 0);
  f_color = v_color[0];
  f_texture = 0;
  EmitVertex();

  gl_Position = P + p[1] * scale;
  f_uv = vec2(1, 0);
  f_color = v_color[0];
  f_texture = 0;
  EmitVertex();

  gl_Position = P + p[2] * scale;
  f_uv = vec2(0, 1);
  f_color = v_color[0];
  f_texture = 0;
  EmitVertex();

  gl_Position = P + p[3] * scale;
  f_uv = vec2(1 ,1);
  f_color = v_color[0];
  f_texture = 0;
  EmitVertex();

  EndPrimitive();


  if (v_size[0] > 15)
  {
    float a = 0.5 * (v_size[0] - 15) / (38 - 15);

    gl_Position = P + p[0] * scale * 3;
    f_uv = vec2(0, 0);
    f_color = vec4(v_color[0].rgb, a);
    f_texture = 1;
    EmitVertex();

    gl_Position = P + p[1] * scale * 3;
    f_uv = vec2(1, 0);
    f_color = vec4(v_color[0].rgb, a);
    f_texture = 1;
    EmitVertex();

    gl_Position = P + p[2] * scale * 3;
    f_uv = vec2(0, 1);
    f_color = vec4(v_color[0].rgb, a);
    f_texture = 1;
    EmitVertex();

    gl_Position = P + p[3] * scale * 3;
    f_uv = vec2(1 ,1);
    f_color = vec4(v_color[0].rgb, a);
    f_texture = 1;
    EmitVertex();

    EndPrimitive();
  }
}



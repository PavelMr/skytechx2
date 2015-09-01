#version 150

uniform float u_scr_width;
uniform float u_scr_height;
uniform float u_mag_limit;

layout(points) in;
layout(triangle_strip, max_vertices = 4) out;

in vec4  v_uv[];
in vec2  v_wh[];
flat in float v_align[];
in float v_mag[];

out vec2 f_uv;
out vec4 f1_color;

const float c_text_offset = 10;

#pragma include ":/res/opengl/common.glsl"

void main()
{
  vec4 scale = vec4(v_wh[0].x * u_scr_width,
                    v_wh[0].y * u_scr_height, 0, 0) * gl_in[0].gl_Position.w;


  if (v_mag[0] > u_mag_limit)
  {
    return;
  }

  vec4 p[4];

  vec4 P = gl_in[0].gl_Position;
  vec4 offset;

  p[0] = vec4(-1, 1, 0, 0);
  p[1] = vec4(1, 1, 0, 0);
  p[2] = vec4(-1, -1, 0, 0);
  p[3] = vec4(1, -1, 0, 0);

  vec4 cc = vec4(1,1,1,1);

  float dist = 0.5 * getSize(v_mag[0]) + c_text_offset;

  switch (int(v_align[0]))
  {
    case 0:
      offset = vec4(0,0,0,0);
      cc = vec4(1,1,1,0.5);
      break;

    case 1:
      offset = vec4(0,1,0,0);
      P += vec4(0, u_scr_height * dist, 0, 0) * gl_in[0].gl_Position.w;
      cc = vec4(1, 0, 0, 1);
      break;

    case 2:
      offset = vec4(0, -1, 0, 0);
      P += vec4(0, -u_scr_height * dist, 0, 0) * gl_in[0].gl_Position.w;
      cc = vec4(0, 1, 0, 1);
      break;

    case 3:
      offset = vec4(1,0,0,0);
      P += vec4(u_scr_width * dist, 0, 0, 0) * gl_in[0].gl_Position.w;
      cc = vec4(0, 1, 1, 1);
      break;

    case 4:
      offset = vec4(-1,0,0,0);
      P += vec4(-u_scr_width * dist, 0, 0, 0) * gl_in[0].gl_Position.w;
      cc = vec4(0, 1, 1, 1);
      break;

    default:
      offset = vec4(0,0,0,0);
      cc = vec4(1,1,0,1);
  }

  p[0] += offset;
  p[1] += offset;
  p[2] += offset;
  p[3] += offset;

  gl_Position = P + p[0] * scale;
  f_uv = vec2(v_uv[0].x, v_uv[0].y);
  f1_color = cc;
  EmitVertex();

  gl_Position = P + p[1] * scale;
  f_uv = vec2(v_uv[0].z, v_uv[0].y);
  f1_color = cc;
  EmitVertex();

  gl_Position = P + p[2] * scale;
  f_uv = vec2(v_uv[0].x, v_uv[0].w);
  f1_color = cc;
  EmitVertex();

  gl_Position = P + p[3] * scale;
  f_uv = vec2(v_uv[0].z, v_uv[0].w);
  f1_color = cc;
  EmitVertex();

  EndPrimitive();
}



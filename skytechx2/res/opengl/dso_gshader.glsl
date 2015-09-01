#version 150

uniform float pixel_ratio;

layout(points) in;
layout(line_strip, max_vertices = 51) out;

void main()
{
  float r360 = 2 * 3.141592;
  float step = r360 / 50;

  float scale = 0.005;

  gl_Position = gl_in[0].gl_Position + vec4(-scale, -scale * pixel_ratio, 0, 0);
  EmitVertex();
  gl_Position = gl_in[0].gl_Position + vec4(+scale, -scale * pixel_ratio, 0, 0);
  EmitVertex();
  gl_Position = gl_in[0].gl_Position + vec4(+scale, +scale * pixel_ratio, 0, 0);
  EmitVertex();
  gl_Position = gl_in[0].gl_Position + vec4(-scale, +scale * pixel_ratio, 0, 0);
  EmitVertex();
  gl_Position = gl_in[0].gl_Position + vec4(-scale, -scale * pixel_ratio, 0, 0);
  EmitVertex();

  /*
  for (float a = 0; a < r360; a += step)
  {
    gl_Position = gl_in[0].gl_Position + vec4(scale * cos(a), pixel_ratio * scale * sin(a), 0, 0);
    EmitVertex();
  }
  gl_Position = gl_in[0].gl_Position + vec4(scale * cos(0), pixel_ratio * scale * sin(0), 0, 0);
  EmitVertex();
  */

  EndPrimitive();
}



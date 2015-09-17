#version 150

uniform float pixel_ratio;
uniform float u_fov;
uniform mat4 mvp_matrix;
uniform float u_scr_width; // 1 / width

uniform vec3 u_galaxy_color;
uniform vec3 u_nebula_color_srm;
uniform vec3 u_nebula_color_bright;
uniform vec3 u_nebula_color_dark;
uniform vec3 u_planetary_nebula_color;
uniform vec3 u_globular_cluster_color;
uniform vec3 u_open_cluster_color;
uniform vec3 u_other_color;

flat in vec4 g_params[];
flat in float g_north[];
flat in int intVal[];

layout(points) in;
layout(line_strip, max_vertices = 120) out;

out vec3 f_color;

const float M_PI = 3.1415926535897932384626433832795;
const float r360 = 2 * M_PI;
const float maxSize = 100;

void main()
{
  float minSize = u_scr_width * 10;
  float step;// = r360 / 50;

  vec2 scale = vec2(g_params[0].y, g_params[0].z) / 3600. / (180 / M_PI) / u_fov;

  scale.x = clamp(scale.x, minSize, maxSize);
  scale.y = clamp(scale.y, minSize, maxSize);

  if (scale.x < 0.05) step = r360 / 20.;
  else
  if (scale.x < 0.2) step = r360 / 40.;
  else
  if (scale.x < 0.5) step = r360 / 60.;
  else step = r360 / 80.;

  float angle;
  vec4 up;
  if (g_north[0] > 0)
  {
    up = mvp_matrix * vec4(0, 1, 0, 1);
    angle = g_params[0].x + atan(up.x - gl_in[0].gl_Position.x,
                                 up.y - gl_in[0].gl_Position.y);
  }
  else
  {
    up = mvp_matrix * vec4(0, -1, 0, 1);
    angle = g_params[0].x + atan(up.x - gl_in[0].gl_Position.x,
                                 up.y - gl_in[0].gl_Position.y);
  }

  if (scale.x <= minSize)
  {
    angle = 0;
    scale.y = scale.x * 0.5;
  }

  vec3 color = vec3(1, 0, 0);
  mat4 RotationMatrix = mat4( cos(angle), -sin(angle), 0.0, 0.0,
                              sin(angle),  cos(angle), 0.0, 0.0,
                              0.0,         0.0,        1.0, 0.0,
                              0.0,         0.0,        0.0, 1.0 );


  if (intVal[0] == 1)
  { // galaxy
    color = u_galaxy_color;
    for (float a = 0; a < r360; a += step)
    {
      vec4 v = RotationMatrix * vec4(scale.x * cos(a), scale.y * sin(a), 0, 0);
      gl_Position = gl_in[0].gl_Position + vec4(v.x, v.y * pixel_ratio, 0, 0);
      f_color = color;
      EmitVertex();
    }

    vec4 v = RotationMatrix * vec4(scale.x * cos(0), scale.y * sin(0), 0, 0);
    gl_Position = gl_in[0].gl_Position + vec4(v.x, v.y * pixel_ratio, 0, 0);
    f_color = color;
    EmitVertex();
  }
  else
  if (intVal[0] == 3 || intVal[0] == 4 || intVal[0] == 5 || intVal[0] == 7)
  { // nebula
    switch (intVal[0])
    {
      case 3:
      case 5:
        color = u_nebula_color_bright;
        break;

      case 7:
        color = u_nebula_color_srm;
        break;

      case 4:
        color = u_nebula_color_dark;
        break;
    }

    gl_Position = gl_in[0].gl_Position + vec4(-scale.x, -scale.x * pixel_ratio, 0, 0);
    f_color = color;
    EmitVertex();
    gl_Position = gl_in[0].gl_Position + vec4(+scale.x, -scale.x * pixel_ratio, 0, 0);
    f_color = color;
    EmitVertex();
    gl_Position = gl_in[0].gl_Position + vec4(+scale.x, +scale.x * pixel_ratio, 0, 0);
    f_color = color;
    EmitVertex();
    gl_Position = gl_in[0].gl_Position + vec4(-scale.x, +scale.x * pixel_ratio, 0, 0);
    f_color = color;
    EmitVertex();
    gl_Position = gl_in[0].gl_Position + vec4(-scale.x, -scale.x * pixel_ratio, 0, 0);
    f_color = color;
    EmitVertex();
  } else
  if (intVal[0] == 12)
  { // globular cluster
    color = u_globular_cluster_color;

    for (float a = 0; a < r360; a += step)
    {
      vec4 v = vec4(scale.x * cos(a), scale.x * sin(a), 0, 0);
      gl_Position = gl_in[0].gl_Position + vec4(v.x, v.y * pixel_ratio, 0, 0);
      f_color = color;
      EmitVertex();
    }

    vec4 v = vec4(scale.x * cos(0), scale.x * sin(0), 0, 0);
    gl_Position = gl_in[0].gl_Position + vec4(v.x, v.y * pixel_ratio, 0, 0);
    f_color = color;
    EmitVertex();

    EndPrimitive();

    gl_Position = gl_in[0].gl_Position + vec4(-scale.x, 0, 0, 0);
    f_color = color;
    EmitVertex();

    gl_Position = gl_in[0].gl_Position + vec4(scale.x, 0, 0, 0);;
    f_color = color;
    EmitVertex();

    EndPrimitive();

    gl_Position = gl_in[0].gl_Position + vec4(0, -scale.y * pixel_ratio, 0, 0);
    f_color = color;
    EmitVertex();

    gl_Position = gl_in[0].gl_Position + vec4(0, scale.y * pixel_ratio, 0, 0);
    f_color = color;
    EmitVertex();
  }
  else
  if (intVal[0] == 6)
  { // planetary nebula
    color = u_planetary_nebula_color;

    //if (step >= r360 / 30.)
    {
      step = r360 / 50.0;
    }

    for (float a = 0; a < r360; a += step)
    {
      vec4 v = vec4(scale.x * cos(a), scale.x * sin(a), 0, 0);
      gl_Position = gl_in[0].gl_Position + vec4(v.x, v.y * pixel_ratio, 0, 0);
      f_color = color;
      EmitVertex();
    }

    vec4 v = vec4(scale.x * cos(0), scale.x * sin(0), 0, 0);
    gl_Position = gl_in[0].gl_Position + vec4(v.x, v.y * pixel_ratio, 0, 0);
    f_color = color;
    EmitVertex();

    EndPrimitive();

    float s = 0.8;

    for (float a = 0; a < r360; a += step)
    {
      vec4 v = vec4(s * scale.x * cos(a), s * scale.x * sin(a), 0, 0);
      gl_Position = gl_in[0].gl_Position + vec4(v.x, v.y * pixel_ratio, 0, 0);
      f_color = color;
      EmitVertex();
    }

    v = vec4(s * scale.x * cos(0), s * scale.x * sin(0), 0, 0);
    gl_Position = gl_in[0].gl_Position + vec4(v.x, v.y * pixel_ratio, 0, 0);
    f_color = color;
    EmitVertex();
  }
  else
  if (intVal[0] == 10 || intVal[0] == 11)
  { // open cluster
    color = u_open_cluster_color;

    int i = 0;
    for (float a = 0; a < r360; a += step, i++)
    {
      vec4 v = RotationMatrix * vec4(scale.x * cos(a), scale.x * sin(a), 0, 0);
      gl_Position = gl_in[0].gl_Position + vec4(v.x, v.y * pixel_ratio, 0, 0);
      f_color = color;
      EmitVertex();

      if (i % 2 == 0)
      {
        EndPrimitive();
      }
    }

    vec4 v = RotationMatrix * vec4(scale.x * cos(0), scale.x * sin(0), 0, 0);
    gl_Position = gl_in[0].gl_Position + vec4(v.x, v.y * pixel_ratio, 0, 0);
    f_color = color;
    EmitVertex();
  }
  else
  { // other (+)
    color = u_other_color;

    scale.x = minSize;
    scale.y = minSize;

    gl_Position = gl_in[0].gl_Position + vec4(-scale.x, 0, 0, 0);
    f_color = color;
    EmitVertex();

    gl_Position = gl_in[0].gl_Position + vec4(scale.x, 0, 0, 0);
    f_color = color;
    EmitVertex();

    EndPrimitive();

    gl_Position = gl_in[0].gl_Position + vec4(0, -scale.y * pixel_ratio, 0, 0);
    f_color = color;
    EmitVertex();

    gl_Position = gl_in[0].gl_Position + vec4(0, scale.y * pixel_ratio, 0, 0);
    f_color = color;
    EmitVertex();
  }

  EndPrimitive();
}



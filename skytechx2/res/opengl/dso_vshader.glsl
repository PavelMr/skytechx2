#version 150

uniform mat4 mvp_matrix;
uniform float max_magnitude;

attribute vec4 a_position;
attribute vec4 a_colorMagnitude;

varying vec4 v_color;

/*
float getSize(float mag)
{
  float r;
  float vm;
  float a = 32.;
  float brMag = 0.;
  float faMag = max_magnitude;

  if (mag < brMag) mag = brMag;
  else
  if (mag > faMag) mag = faMag;

  vm = mag / (faMag - brMag);
  vm = a - (vm * a);
  r = vm;

  if (r < 1.0)
      r = 1.0;
  else
  if (r >= a) {
      r = a;
  }
  return r;
}
*/

void main()
{
  //gl_PointSize = getSize(a_colorMagnitude.w);
  gl_Position = mvp_matrix * a_position;

  if (a_colorMagnitude.w > max_magnitude)
  {
    v_color = vec4(0.,0.,0.,0.);
  }
  else
  {
    v_color = vec4(1., 1., 1., 1.);
  }
}


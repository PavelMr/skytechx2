float getSize(float mag)
{
  float a = 38.0;
  float brMag = 0;
  float faMag = u_mag_limit;

  if (mag > faMag || mag < -500)
  {
    return 0.0;
  }

  mag = clamp(mag, brMag, faMag);
  float linear = mag / (faMag - brMag);
  float cubic = pow (linear - 1, 3) + 1;

  float size = mix(linear, cubic, 0.8);

  return 1.5 + a * clamp(1 - size, 0, 1);
}

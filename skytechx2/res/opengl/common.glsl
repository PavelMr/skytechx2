float getSize(float mag)
{
  float vm;
  float a = 38.0;
  float brMag = 0;
  float faMag = u_mag_limit;

  if (mag > faMag || mag < -500)
  {
    return 0.0;
  }

  mag = clamp(mag, brMag, faMag);
  vm = mag / (faMag - brMag);
  vm = pow (vm - 1, 3) + 1;

  return 1 + a * clamp(1 - vm, 0, 1);
}

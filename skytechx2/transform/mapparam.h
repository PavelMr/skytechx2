#ifndef MAPPARAM_H
#define MAPPARAM_H

#include <QtGlobal>

class MapParam
{
public:
  MapParam();

  double m_fov;
  double m_x;
  double m_y;
  double m_roll;
  double m_maxStarMag;
  double m_starMagAdd;
  bool   m_flipX;
  bool   m_flipY;
};

#endif // MAPPARAM_H

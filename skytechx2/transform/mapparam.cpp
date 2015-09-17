#include "mapparam.h"
#include "skmath.h"

MapParam::MapParam()
{
  m_x = 0;
  m_y = 0;
  m_fov = SkMath::toRad(90);
  m_roll = 0;
  m_maxStarMag = 10;
  m_starMagAdd = 0;
  m_flipX = false;
  m_flipY = false;
}


#include "mapobject.h"

MapObject::MapObject()
{

}

void MapObject::newFrame()
{
  m_mapList.clear();
}

void MapObject::addTycho2(int region, int index, double ra, double dec, float mag)
{
  mapObject_t mo;

  mo.radec = RaDec(ra, dec);
  mo.magnitude = mag;
  mo.param = decode(region, index);
  mo.type = TYCHO_STAR;

  m_mapList.append(mo);
}

void MapObject::addUCAC4(int region, int index, double ra, double dec, float mag)
{
  mapObject_t mo;

  mo.radec = RaDec(ra, dec);
  mo.magnitude = mag;
  mo.param = decode(region, index);
  mo.type = TYCHO_STAR;

  m_mapList.append(mo);
}

int MapObject::getObjectCount()
{
  return m_mapList.size();
}


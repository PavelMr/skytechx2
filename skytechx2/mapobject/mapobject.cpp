#include "mapobject.h"

MapObject::MapObject()
{

}

void MapObject::newFrame()
{
  m_mapList.clear();
}

int MapObject::getObjectCount()
{
  return m_mapList.count();
}


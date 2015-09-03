#ifndef MAPOBJECT_H
#define MAPOBJECT_H

#include "transform.h"

#include <QList>

class MapObject
{
  enum MapTypeObject
  {
    ANY  = 0,
    TYCHO_STAR = 1,
  };

  typedef struct
  {
    MapTypeObject type;
    RaDec         radec;      // J2000.0
    float         magnitude;
    float         radius;     // in pixels
    qint64        param;
  } mapObject_t;

public:
  MapObject();
  void newFrame();

  void addTycho2(int region, int index, float mag, int radius);

  int findNearestObject(Transform *transform, RaDec &pos, MapTypeObject type = ANY);

  int getObjectCount();

private:
  qint64 tycho2ToParam(int region, int index);
  void paramToTycho2(qint64 param, int &region, int &index);


  QList <mapObject_t> m_mapList;
};

#endif // MAPOBJECT_H

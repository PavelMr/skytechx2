#ifndef MAPOBJECT_H
#define MAPOBJECT_H

#include "transform.h"

#include <QVector>

#define CENTER_MAP           -1

enum MOType
{
  TYCHO_STAR = 0,
  UCAC4_STAR = 1,
};

typedef struct
{
  QString label;
  double  magnitude;
  MOType  type;

  int     param1;
  int     param2;
} mapObject_t;

class MapObject : public QObject
{
public:
  MapObject();
  void newFrame();

  void addRegion(int region);
  void objList(const RaDec pos, Transform *transform);

private:
  QVector <int> m_regionList;
};

#endif // MAPOBJECT_H

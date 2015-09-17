#ifndef MAPOBJECT_H
#define MAPOBJECT_H

#include "transform.h"

#include <QVector>

#define CENTER_MAP           -1

enum MOType
{
  DSO_OBJECT = 0,
  TYCHO_STAR = 1,
  GSC_STAR   = 2,
  UCAC4_STAR = 3,
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
  void addDSO(int index);
  void objList(const RaDec pos, Transform *transform);

private:
  QVector <int> m_regionList;
  QVector <int> m_dsoList;
};

#endif // MAPOBJECT_H

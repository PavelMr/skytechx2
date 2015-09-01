#ifndef CONSTELLATIONS_H
#define CONSTELLATIONS_H

#include "transform.h"

#include <QList>

typedef struct
{
   double ra, dec;
   qint32 cmd;     // 0-data, 1-main line to, 2-sec. line to
} constellationLines_t;

typedef struct
{
    RaDec rd;
    short p;
} constBndLines_t;

typedef struct
{
  RaDec   rd;
  QString longName;
  QString shortName;
  QString name2Nd;
} constNames_t;

class Constellations
{
public:
  Constellations();
  bool load();
  QList <constellationLines_t> getLines();
  constNames_t getNames(int index);

private:
  bool loadLines();
  bool loadBoundaries();
  bool loadNames();

public:
  QList <constellationLines_t> m_constellationLines;
  qint32 m_numConstelNBnd;
  constBndLines_t *m_constelNBnd;
  QList <constNames_t> m_constellationNames;
};

#endif // CONSTELLATIONS_H

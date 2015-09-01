#include "constellations.h"
#include "skfile.h"
#include "skmath.h"

#include <QDebug>

Constellations::Constellations()
{

}

bool Constellations::load()
{
  if (!loadLines())
  {
    return false;
  }

  if (!loadBoundaries())
  {
    return false;
  }

  if (!loadNames())
  {
    return false;
  }

  return true;
}

QList<constellationLines_t> Constellations::getLines()
{
  return m_constellationLines;
}

constNames_t Constellations::getNames(int index)
{
  return m_constellationNames[index];
}

bool Constellations::loadLines()
{
  SkFile f("resources/constellations/constellations.lin");

  m_constellationLines.clear();

  if (f.open(SkFile::ReadOnly))
  {
    qint32 cnt;
    f.read((char *)&cnt, sizeof(qint32));

    for (int i = 0; i < cnt; i++)
    {
      constellationLines_t l;

      f.read((char *)&l.ra, sizeof(double));
      f.read((char *)&l.dec, sizeof(double));
      f.read((char *)&l.cmd, sizeof(qint32));

      m_constellationLines.append(l);
    }
    f.close();
    return true;
  }
  return false;
}

bool Constellations::loadBoundaries()
{
  SkFile f("resources/constellations/constellations.bnd");
  if (f.open(SkFile::ReadOnly))
  {
    f.read((char *)&m_numConstelNBnd, sizeof(qint32));
    m_constelNBnd = (constBndLines_t *)malloc(m_numConstelNBnd * sizeof(constBndLines_t));
    for (int i = 0; i < m_numConstelNBnd; i++)
    {
      f.read((char *)&m_constelNBnd[i].rd.ra, sizeof(double));
      f.read((char *)&m_constelNBnd[i].rd.dec, sizeof(double));
      f.read((char *)&m_constelNBnd[i].p, sizeof(short));
    }
    f.close();
    qDebug() << m_numConstelNBnd;
    return true;
  }
  return false;
}

bool Constellations::loadNames()
{
  SkFile f("resources/constellations/constellations.nam");
  if (f.open(SkFile::ReadOnly | SkFile::Text))
  {
    int         i = 0;
    QString     str;
    QStringList list;

    do
    {
      str = f.readLine();
      if (str.isEmpty())
        break;
      list = str.split("|");
      if (list.count() != 5)
      {
        qDebug("constLoadNames fail!");
        exit(1);
      }

      if (i >= 88)
      {
        qDebug("constLoadNames fail! (too many constellations)");
        exit(1);
      }

      constNames_t name;

      name.longName = list[0].simplified();
      name.name2Nd = list[1].simplified();
      name.shortName = list[2].simplified();

      QStringList ra = list[3].simplified().split(" ");

      name.rd.ra = HMS2RAD(ra[0].toInt(), ra[1].toInt(), 0);
      name.rd.dec = SkMath::toRad(list[4].simplified().toDouble());

      m_constellationNames.append(name);

      i++;
    } while(1);

    f.close();
    return true;
  }

  return false;

}


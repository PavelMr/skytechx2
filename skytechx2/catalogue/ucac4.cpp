#include "ucac4.h"

#include "skmath.h"
#include "dataresources.h"
#include "gscregions.h"

UCAC4::UCAC4() :
  m_memoryUsage(0)
{
  m_folder = "resources/stars/ucac4";
}

ucac4Region_t *UCAC4::getRegion(int region)
{
  if (m_regionMap.contains(region))
  {
    return &m_regionMap[region];
  }

  ucac4Region_t regionData;
  ucac4Region_t *regionPtr = &regionData;

  int minRa  = (int)(SkMath::toDeg(g_dataResource->getGscRegions()->gscRegionSector[region].RaMin) * 3600. * 1000.);
  int maxRa  = (int)(SkMath::toDeg(g_dataResource->getGscRegions()->gscRegionSector[region].RaMax) * 3600. * 1000.);
  int minDec = (int)((SkMath::toDeg(g_dataResource->getGscRegions()->gscRegionSector[region].DecMin) + 90.) * 3600. * 1000.);
  int maxDec = (int)((SkMath::toDeg(g_dataResource->getGscRegions()->gscRegionSector[region].DecMax) + 90.) * 3600. * 1000.);

  if (minRa > maxRa)
  {
    maxRa = 360 * 3600 * 1000;
  }

  const double zone_height = 0.2;  // zones are .2 degrees each
  double dDecMax = SkMath::toDeg(g_dataResource->getGscRegions()->gscRegionSector[region].DecMax);
  double dDecMin = SkMath::toDeg(g_dataResource->getGscRegions()->gscRegionSector[region].DecMin);

  int zoneEnd = (int)((dDecMax + 90.) / zone_height) + 1;
  int zoneStart = (int)((dDecMin + 90.) / zone_height) + 1;

  zoneEnd = CLAMP(zoneEnd, 1, 900);
  zoneStart = CLAMP(zoneStart, 1, 900);

  if (zoneStart > zoneEnd)
  {
    qSwap(zoneStart, zoneEnd);
  }

  if (maxDec < minDec)
  {
    qSwap(maxDec, minDec);
  }

  for (int z = zoneStart; z <= zoneEnd; z++)
  {
    SkFile f(m_folder + QString("/z%1").arg(z, 3, 10, QChar('0')));
    SkFile acc(m_folder + QString("/z%1.acc").arg(z, 3, 10, QChar('0')));

    m_accList.clear();

    if (!readAccFile(acc))
    {
      if (acc.open(QFile::WriteOnly))
      { // create accelerated index file
        if (f.open(QFile::ReadOnly))
        {
          quint64 lastOffset = 0;
          int lastRa = 0;
          int index = 1;
          int step = 2.5 * 3600 * 1000; // 2.5 deg. step
          while (!f.atEnd())
          {
            UCAC4_Star_t star;

            f.read((char *)&star, sizeof(star));

            if (star.ra >= lastRa)
            {
              lastOffset = f.pos() - sizeof(star);

              acc.write((char *)&lastRa, sizeof(lastRa));
              acc.write((char *)&index, sizeof(index));
              acc.write((char *)&lastOffset, sizeof(lastOffset));

              ucac4AccFile_t acc;

              acc.ra = lastRa;
              acc.index = index;
              acc.offset = lastOffset;

              m_accList.append(acc);

              lastRa += step;
            }
            index++;
          }
          f.close();
          acc.close();
        }
      }
    }

    if (f.open(QFile::ReadOnly))
    {
      quint64 last = 0;
      int lastIndex = 1;
      foreach (const ucac4AccFile_t &acc, m_accList)
      {
        if (acc.ra >= minRa)
        {
          f.seek(last);
          break;
        }
        last = acc.offset;
        lastIndex = acc.index;
      }

      int ucac4Index = lastIndex;
      while (!f.atEnd())
      {
        UCAC4_Star_t star;

        f.read((char *)&star, sizeof(star));

        if (star.ra >= maxRa)
        {
          break;
        }

        if (star.spd > minDec && star.spd < maxDec &&
            star.ra > minRa)
        {
          /*
          skStar.rd.Ra = D2R(star.ra / 3600. / 1000.0);
          skStar.rd.Dec = D2R((star.spd / 3600. / 1000.0) - 90.0);
          skStar.mag = star.mag2 / 1000.0;
          skStar.number = ucac4Index;
          skStar.zone = z;
          */

          star.ucac4_zone = z;
          star.ucac4_number = ucac4Index;

          regionPtr->m_stars.append(star);
        }
        ucac4Index++;
      }
    }
  }

  if (regionPtr->m_stars.count() > 0)
  {
    regionPtr->m_size = regionPtr->m_stars.count() * sizeof(UCAC4_Star_t);
    regionPtr->m_timer = QDateTime::currentMSecsSinceEpoch();

    m_memoryUsage += regionPtr->m_size;

    //qSort(regionPtr->stars.begin(), regionPtr->stars.end(), ucacCompare);
  }
  else
  {
    return NULL;
  }

  if (m_regionMap.size() >= 200)
  {
    deleteOldestRegion();
  }

  m_regionMap[region] = regionData;
  return &m_regionMap[region];
}

int UCAC4::getMemoryUsage()
{
  return m_memoryUsage;
}

int UCAC4::getRegionUsed()
{
  return m_regionMap.size();
}

void UCAC4::clearCache()
{
  m_regionMap.clear();
  m_memoryUsage = 0;
}


bool UCAC4::readAccFile(SkFile &file)
{
  if (file.open(QFile::ReadOnly))
  {
    while (!file.atEnd())
    {
      ucac4AccFile_t acc;

      file.read((char *)&acc.ra, sizeof(acc.ra));
      file.read((char *)&acc.index, sizeof(acc.index));
      file.read((char *)&acc.offset, sizeof(acc.offset));

      m_accList.append(acc);
    }
    return true;
  }
  return false;
}

void UCAC4::deleteOldestRegion()
{
  QMap <int, ucac4Region_t>::iterator i;
  int key = -1;
  qint64 oldestTime = std::numeric_limits<qint64>::max();

  for (i = m_regionMap.begin(); i != m_regionMap.end(); ++i)
  {
    if (i.value().m_timer < oldestTime)
    {
      oldestTime = i.value().m_timer;
      key = i.key();
    }
  }

  if (key >= 0)
  {
    m_memoryUsage -= m_regionMap[key].m_size;
    m_regionMap.remove(key);
  }
  else
  {
    qDebug() << "UCAC4 deleteOldestRegion() failed()";
  }
}





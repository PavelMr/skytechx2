#include "mapobject.h"
#include "tycho.h"
#include "ucac4.h"
#include "dataresources.h"
#include "skstring.h"

#include <QMenu>
#include <QDebug>

MapObject::MapObject()
{

}

void MapObject::newFrame()
{
  m_regionList.clear();
}


void MapObject::addRegion(int region)
{
  m_regionList.append(region);
}

static bool mapObjectSort(const mapObject_t &a, const mapObject_t &b)
{
  if (a.type < b.type)
  {
    return true;
  }

  return a.magnitude < b.magnitude;
}

void MapObject::objList(const RaDec pos, Transform *transform)
{
  QList <mapObject_t> list;
  mapObject_t mapObject;

  Tycho *tycho = g_dataResource->getTycho();
  UCAC4 *ucac4 = g_dataResource->getUCAC4();

  double maxStarMag = transform->getMapParam()->m_maxStarMag;
  double angDistance = transform->m_fov / 100.0;

  //qDebug() << "--------------------";

  foreach (int i, m_regionList)
  {
    // tycho2 ///////////////////////////
    for (int j = 0; j < tycho->m_region[i].region.numStars; j++)
    {
      tychoStar_t *star = &tycho->m_region[i].stars[j];

      if (tycho->getMagnitude(*star) > maxStarMag)
      {
        continue;
      }

      if (SkMath::distance(pos.ra, pos.dec, star->ra, star->dec) <= angDistance)
      {
        QString name;

        tychoSupp_t *supp = star->supIndex == -1 ? NULL : &tycho->m_supplement[star->supIndex];
        if (supp)
        {
          bool found;
          name = tycho->getStarName(supp);
          if (name == "")
          {
            name = tycho->getBayerString2(supp, found);
            if (!found)
            {
              name = tycho->getFlamsteedString(supp, found);
              if (!found)
              {
                name = QString("HD %1").arg(supp->hd);
              }
            }
          }
        }
        else
        {
          name = QString("TYC %1-%2-%3").arg(star->tyc3).arg(star->tyc1).arg(star->tyc2);
        }
        //qDebug() << name << star->tyc1 << star->tyc2 << star->tyc3 << tycho->getMagnitude(*star);
        mapObject.label = name;
        mapObject.magnitude = tycho->getMagnitude(*star);
        mapObject.type = TYCHO_STAR;
        mapObject.param1 = i; // region
        mapObject.param2 = j; // index in region

        list.append(mapObject);
      }
    }

    if (transform->getMapParam()->m_fov <= SkMath::toRad(10))
    {
      // ucac4 //////////////////////////////////////////
      ucac4Region_t *ucac4region = ucac4->getRegion(i);

      for (int j = 0; j < ucac4region->m_stars.count(); j++)
      {
        UCAC4_Star_t *star = &ucac4region->m_stars[j];

        if ((star->mag2 / 1000.) > maxStarMag || (star->mag2 / 1000.0 < 11))
        {
          continue;
        }

        if (SkMath::distance(pos.ra, pos.dec, SkMath::toRad(star->ra / 3600. / 1000.0), SkMath::toRad((star->spd / 3600. / 1000.0) - 90.0)) <= angDistance)
        {
          //qDebug() << star->id_number << (star->mag2 / 1000.);

          mapObject.label = "UCAC4 " + QString::number(star->id_number);
          mapObject.magnitude = star->mag2 / 1000.0;
          mapObject.type = UCAC4_STAR;
          mapObject.param1 = i; // region
          mapObject.param2 = j; // index in region

          list.append(mapObject);
        }
      }
    }

  }

  qSort(list.begin(), list.end(), mapObjectSort);

  QMenu menu;
  QAction *action;

  action = menu.addAction(tr("Center map"));
  action->setData(CENTER_MAP);

  int count = list.count() > 16 ? 16 : list.count();

  if (count > 0)
  {
    menu.addSeparator();
  }

  for (int i = 0; i < count; i++)
  {
    action = menu.addAction(list[i].label + SkString::getMagnitude(list[i].magnitude, true));
    action->setData(i);

    if (i + 1 < list.count() && list[i].type != list[i + 1].type)
    {
      menu.addSeparator();
    }
  }

  QAction *selectedItem = menu.exec(QCursor::pos());
}


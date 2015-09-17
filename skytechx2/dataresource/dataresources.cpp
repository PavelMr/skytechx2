#include "dataresources.h"
#include "tycho.h"
#include "gsc.h"
#include "ucac4.h"
#include "dso.h"
#include "constellations.h"
#include "gscregions.h"
#include "mapobject.h"

DataResources *g_dataResource;

DataResources::DataResources() :
  m_tycho(0)
{
  m_tycho = new Tycho();
  m_tycho->load();

  m_gsc = new GSC();
  m_ucac4 = new UCAC4();

  m_constellations = new Constellations();
  m_constellations->load();

  m_gscRegion = new GscRegion(4);
  m_gscRegion->loadRegions();

  m_dso = new DSO();
  m_dso->load();

  m_mapObject = new MapObject();
}

Tycho *DataResources::getTycho()
{
  return m_tycho;
}

GSC *DataResources::getGSC()
{
  return m_gsc;
}

UCAC4 *DataResources::getUCAC4()
{
  return m_ucac4;
}

DSO *DataResources::getDSO()
{
  return m_dso;
}

Constellations *DataResources::getConstellations()
{
  return m_constellations;
}

MapObject *DataResources::getMapObject()
{
  return m_mapObject;
}

GscRegion *DataResources::getGscRegions()
{
  return m_gscRegion;
}


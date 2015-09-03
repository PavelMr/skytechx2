#include "dataresources.h"
#include "tycho.h"
#include "gsc.h"
#include "constellations.h"
#include "gscregions.h"

DataResources *g_dataResource;

DataResources::DataResources() :
  m_tycho(0)
{
  m_tycho = new Tycho();
  m_tycho->load();

  m_gsc = new GSC();

  m_constellations = new Constellations();
  m_constellations->load();

  m_gscRegion = new GscRegion(4);
  m_gscRegion->loadRegions();


}

Tycho *DataResources::getTycho()
{
  return m_tycho;
}

GSC *DataResources::getGSC()
{
  return m_gsc;
}

Constellations *DataResources::getConstellations()
{
  return m_constellations;
}

GscRegion *DataResources::getGscRegions()
{
  return m_gscRegion;
}


#ifndef DATARESOURCES_H
#define DATARESOURCES_H

class DataResources;
class Tycho;
class Constellations;
class GscRegion;
class UCAC4;
class GSC;
class MapObject;
class DSO;

extern DataResources *g_dataResource;

class DataResources
{
public:
  DataResources();

  Tycho *getTycho();
  GSC *getGSC();
  UCAC4 *getUCAC4();

  DSO *getDSO();

  Constellations *getConstellations();
  MapObject *getMapObject();
  GscRegion *getGscRegions();

private:
  Tycho *m_tycho;
  GSC   *m_gsc;
  UCAC4 *m_ucac4;
  DSO   *m_dso;

  Constellations *m_constellations;
  GscRegion *m_gscRegion;

  MapObject *m_mapObject;
};


#endif // DATARESOURCES_H

#ifndef DATARESOURCES_H
#define DATARESOURCES_H

class DataResources;
class Tycho;
class Constellations;
class GscRegion;
class GSC;

extern DataResources *g_dataResource;

class DataResources
{
public:
  DataResources();

  Tycho *getTycho();
  GSC *getGSC();

  Constellations *getConstellations();


  GscRegion *getGscRegions();

private:
  Tycho *m_tycho;
  GSC   *m_gsc;

  Constellations *m_constellations;
  GscRegion *m_gscRegion;
};


#endif // DATARESOURCES_H

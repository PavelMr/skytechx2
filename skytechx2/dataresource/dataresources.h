#ifndef DATARESOURCES_H
#define DATARESOURCES_H

class DataResources;
class Tycho;
class Constellations;
class GscRegion;

extern DataResources *g_dataResource;

class DataResources
{
public:
  DataResources();

  Tycho *getTycho();
  Constellations *getConstellations();
  GscRegion *getGscRegions();

private:
  Tycho *m_tycho;
  Constellations *m_constellations;
  GscRegion *m_gscRegion;
};


#endif // DATARESOURCES_H

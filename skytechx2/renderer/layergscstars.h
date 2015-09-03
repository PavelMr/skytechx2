#ifndef LAYERGSCSTARS_H
#define LAYERGSCSTARS_H

#include "gsc.h"

class Transform;
class Renderer;

class LayerGSCStars
{
public:
  LayerGSCStars();
  void render(Transform *transform, Renderer *renderer);

private:
  void createRegionBuffer(gscStarRegion_t *region);
};

#endif // LAYERGSCSTARS_H

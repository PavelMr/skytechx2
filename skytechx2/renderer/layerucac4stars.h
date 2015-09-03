#ifndef LAYERUCAC4STARS_H
#define LAYERUCAC4STARS_H

#include "ucac4.h"

class Transform;
class Renderer;

class LayerUCAC4Stars
{
public:
  LayerUCAC4Stars();
  void render(Transform *transform, Renderer *renderer);

private:
  void createRegionBuffer(ucac4Region_t *region);
};

#endif // LAYERUCAC4STARS_H

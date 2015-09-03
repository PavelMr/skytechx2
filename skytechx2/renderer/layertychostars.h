#ifndef LAYERTYCHOSTARS_H
#define LAYERTYCHOSTARS_H

#include "gscregions.h"
#include "textrenderer.h"

#include <QOpenGLBuffer>
#include <QList>

class Transform;
class Renderer;

class LayerTychoStars
{
public:
  LayerTychoStars();
  void createReources();
  void render(Transform *transform, Renderer *renderer);

private:
  TextRenderer m_textRendererNames;
  TextRenderer m_textRendererIdFlm;
  TextRenderer m_textRendererIdBayer;
  QOpenGLBuffer *m_regionGlVertices;
};

#endif // LAYERTYCHOSTARS_H

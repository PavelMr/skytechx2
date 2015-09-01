#ifndef LAYERTYCHOSTARS_H
#define LAYERTYCHOSTARS_H

#include "gscregions.h"
#include "textrenderer.h"

#include <QOpenGLBuffer>
#include <QList>

#define TYCHO_GL_MAG_RANGE   15 * 4 // 15 * 0.25 mag
#define TYCHO_GL_MAG_STEP    0.25

class Transform;

class LayerTychoStars
{
public:
  LayerTychoStars();
  void createReources();
  void render(Transform *transform);

private:
  TextRenderer m_textRendererNames;
  TextRenderer m_textRendererIdFlm;
  TextRenderer m_textRendererIdBayer;
  QOpenGLBuffer *m_regionGlVertices;
  ushort m_magRegionCount[NUM_GSC_REGS][TYCHO_GL_MAG_RANGE];
};

#endif // LAYERTYCHOSTARS_H

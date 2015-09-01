#ifndef LAYERCONSTELLATIONS_H
#define LAYERCONSTELLATIONS_H

#include "textrenderer.h"

#include <QOpenGLBuffer>
#include <QOpenGLShaderProgram>

class Transform;

class LayerConstellations
{
public:
  LayerConstellations();
  void createResources();
  void render(Transform *transform);

private:
  void createResourcesLines();
  void createResourcesBoundaries();
  void createResourcesNames();

  QOpenGLBuffer m_glLines;
  int           m_glLinesCount;
  QOpenGLShaderProgram m_lineProgram;

  QOpenGLBuffer m_glBoundaries;
  int           m_glBoundariesCount;

  TextRenderer  m_namesRenderer;
};

#endif // LAYERCONSTELLATIONS_H

#ifndef LAYERBACKGROUND_H
#define LAYERBACKGROUND_H

#include <QOpenGLBuffer>
#include <QOpenGLShaderProgram>

class Transform;

class LayerBackground
{
public:
  LayerBackground();
  void createResources();
  void render(Transform *transform);

private:
  QOpenGLBuffer m_vertices;
  QOpenGLShaderProgram m_program;
};

#endif // LAYERBACKGROUND_H

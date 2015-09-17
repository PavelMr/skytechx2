#ifndef LAYERDSO_H
#define LAYERDSO_H

#include "dso.h"

#include <QOpenGLBuffer>
#include <QOpenGLShaderProgram>

class Transform;

class LayerDSO
{
public:
  LayerDSO();
  void createResources();
  void render(Transform *transform);
  static bool checkShowDSO(Transform *transform, dso_t *dso);

private:
  QOpenGLBuffer        m_buffer;
  QOpenGLShaderProgram m_program;
};

#endif // LAYERDSO_H

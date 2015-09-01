#ifndef LAYERGRID_H
#define LAYERGRID_H

#include <transform.h>

#include <QOpenGLShaderProgram>
#include <QOpenGLBuffer>

#define MAX_GRID_POINTS           115000

class LayerGrid
{
public:
  LayerGrid();
  void createResources();
  void render(Transform *transform);

private:
  QOpenGLShaderProgram m_program;
  void render(Transform *transform, QMatrix4x4 &matrix, int type);
  int generateGrid(Transform *transform, QMatrix4x4 &matrix, int type);
  int interpolateLine(QMatrix4x4 &mat, int c, RaDec *rd1, RaDec *rd2, int count, Transform *transform);

  QOpenGLBuffer m_buffer;
  QVector3D     m_vertices[MAX_GRID_POINTS];
};

#endif // LAYERGRID_H

#ifndef PAINTEROVERLAY_H
#define PAINTEROVERLAY_H

#include <QOpenGLBuffer>
#include <QOpenGLShaderProgram>
#include <QOpenGLTexture>

class Transform;

class PainterOverlay
{
public:
  PainterOverlay();
  void render(Transform *transform, QImage *image);
  void createTexture(QImage *image);

private:
  QOpenGLTexture      *m_texture;
  QOpenGLBuffer        m_buffer;
  QOpenGLShaderProgram m_program;
};

#endif // PAINTEROVERLAY_H

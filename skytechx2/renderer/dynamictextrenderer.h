#ifndef DYNAMICTEXTRENDERER_H
#define DYNAMICTEXTRENDERER_H

#include "transform.h"

#include <QImage>
#include <QOpenGLBuffer>
#include <QOpenGLTexture>
#include <QOpenGLShaderProgram>
#include <QMap>
#include <QVector3D>

#define MAX_TEXT_CHARACTERS      100

class DynamicTextRenderer
{
  typedef struct
  {
    QVector3D pos;
    float     uv[2];
  } dtVertex_t;

public:
  DynamicTextRenderer();
  void createTexture(int size, QString fontName, int fontSize);
  void render(Transform *transform, const RaDec &raDec, const QString &text, int align); // TODO: align vzit z texrendereru
  void begin(Transform *transform);
  void end(Transform *transform);

private:
  QOpenGLShaderProgram m_program;
  QOpenGLBuffer        m_buffer;
  QOpenGLTexture      *m_texture;

  int               m_from, m_to;
  QMap <QChar, QRect> m_rectMap;
};

#endif // DYNAMICTEXTRENDERER_H

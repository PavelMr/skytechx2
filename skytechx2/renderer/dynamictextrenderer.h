#ifndef DYNAMICTEXTRENDERER_H
#define DYNAMICTEXTRENDERER_H

#include "transform.h"

#include <QImage>
#include <QOpenGLBuffer>
#include <QOpenGLTexture>
#include <QOpenGLShaderProgram>
#include <QMap>
#include <QVector3D>

#define MAX_TEXT_CHARACTERS      10000

class DynamicTextRenderer
{
  typedef struct
  {
    QVector3D pos;
    QVector3D center;
    float     uvo[3];
  } dtVertex_t;

public:
  DynamicTextRenderer();
  void createTexture(int size, QString fontName, int fontSize, const QColor &color);
  void render(Transform *transform, const RaDec &raDec, const QString &text, float bottomOffset); // TODO: align vzit z texrendereru
  void begin(Transform *transform);
  void end(Transform *transform);

private:
  void draw(Transform *transform);

  QOpenGLShaderProgram m_program;
  QOpenGLBuffer        m_buffer;
  QOpenGLTexture      *m_texture;

  int                  m_offset;
  int                  m_from, m_to;
  QMap <QChar, QRect>  m_rectMap;
  QMap <QChar, QRectF> m_rectFMap;
};

#endif // DYNAMICTEXTRENDERER_H

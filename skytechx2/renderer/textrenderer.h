#ifndef TEXTRENDERER_H
#define TEXTRENDERER_H

#include "transform.h"
#include "skmath.h"

#include <QOpenGLBuffer>
#include <QOpenGLShaderProgram>
#include <QOpenGLTexture>
#include <QColor>

#define GL_TA_CENTER        0
#define GL_TA_BOTTOM        1
#define GL_TA_TOP           2
#define GL_TA_RIGHT         3
#define GL_TA_LEFT          4

typedef struct
{
  float  x, y, z;
  float  uv[4];
  float  wh[2];
  float  mag;
  float  align;
} textVertex_t;

typedef struct
{
  QString text;
  QRect   rect;
} texts_t;

class TextRenderer
{
public:
  TextRenderer();
  void createTexture(QString objName, int size, QString fontName, int fontSize, QColor color = Qt::white);
  void addText(const QString &text, const RaDec &rd, int align = GL_TA_CENTER, float mag = NO_MAG_LIMIT);
  void createResources();
  void render(Transform *transform, float magLimit = MAX_MAG_LIMIT);
  void save();

private:
  QImage *m_image;
  int m_lastX;
  int m_lastY;
  int m_margin;

  QString m_objName;
  QString m_fontName;
  int m_fontSize;

  QColor m_color;

  QList <texts_t> m_texts;

  int m_glCount;
  QOpenGLBuffer m_glBuffer;
  QVector <textVertex_t> m_vertices;
  QOpenGLTexture *m_texture;
};

#endif // TEXTRENDERER_H

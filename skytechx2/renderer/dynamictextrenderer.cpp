#include "dynamictextrenderer.h"
#include "renderer.h"
#include "transform.h"

#include <QPainter>

DynamicTextRenderer::DynamicTextRenderer() :
  m_from(32),
  m_to(128)
{
}

void DynamicTextRenderer::createTexture(int size, QString fontName, int fontSize, const QColor &color)
{
  m_program.addShaderFromSourceCode(QOpenGLShader::Vertex, Renderer::readShader(":/res/opengl/dtext_vshader.glsl"));
  m_program.addShaderFromSourceCode(QOpenGLShader::Fragment, Renderer::readShader(":/res/opengl/dtext_fshader.glsl"));

  if (!m_program.link())
  {
    qDebug() << m_program.log();
    exit(1);
  }

  m_rectMap.clear();

  // TODO: destroy buffer
  m_buffer.create();
  m_buffer.bind();
  m_buffer.allocate(MAX_TEXT_CHARACTERS * 4 * sizeof(dtVertex_t));

  QFont font = QFont(fontName, fontSize);
  QImage image(size, size, QImage::Format_ARGB32_Premultiplied);

  float ts = size;

  image.fill(Qt::transparent);

  QPainter p(&image);

  p.setPen(color);
  p.setFont(font);
  QFontMetrics fm(p.font());

  int x = 0;
  int y = 0;

  for (int i = m_from; i < m_to; i++)
  {
    int charWidth = fm.width(QChar(i));
    if (x + charWidth >= size)
    {
      x = 0;
      y += fm.height();
    }
    QRect rc = QRect(x, y, charWidth, fm.height());
    m_rectMap[i] = rc;
    m_rectFMap[i] = QRectF(rc.left() / ts,
                          rc.top() / ts,
                          rc.width() / ts,
                          rc.height() / ts);

    p.drawText(rc, QChar(i));
    x += rc.width() + 1;
  }

  p.end();

  image.save("text.png", "PNG");

  m_texture = new QOpenGLTexture(image, QOpenGLTexture::DontGenerateMipMaps);
}

void DynamicTextRenderer::render(Transform *transform, const RaDec &raDec, const QString &text, float bottomOffset)
{
  float hm = (transform->m_height / transform->m_width);
  float x = 0, y = 0;
  float z = 0;
  dtVertex_t *vertex;

  if (m_offset + (4 * text.size()) >= MAX_TEXT_CHARACTERS * 4)
  {
    draw(transform);
    m_offset = 0;
  }

  m_buffer.bind();
  vertex = (dtVertex_t *)m_buffer.map(QOpenGLBuffer::WriteOnly);
  vertex += m_offset;

  Vector3 vec;
  Transform::rdToVector(raDec, vec);


  float offsetx, offsety;

  float width = 0, height = 0;
  foreach (QChar ch, text)
  {
    QRect  rc = m_rectMap[ch];
    float w, h;

    width += rc.width();
    if (h > height)
    {
      height = h;
    }
  }

  offsetx = -width * 0.5;
  offsety = 0;

  foreach (QChar ch, text)
  {
    QRectF rcf = m_rectFMap[ch];
    QRect  rc = m_rectMap[ch];
    float w, h;

    w = rc.width();
    h = rc.height() * hm;

    vertex->pos = QVector3D(x + offsetx, y + offsety, z);
    vertex->center = vec.toQVector();
    vertex->uvo[0] = rcf.left();
    vertex->uvo[1] = rcf.top();
    vertex->uvo[2] = bottomOffset;
    vertex++;

    vertex->pos = QVector3D(x + offsetx, y - h +  + offsety, z);
    vertex->center = vec.toQVector();
    vertex->uvo[0] = rcf.left();
    vertex->uvo[1] = rcf.bottom();
    vertex->uvo[2] = bottomOffset;
    vertex++;

    vertex->pos = QVector3D(x + w + offsetx, y - h + offsety, z);
    vertex->center = vec.toQVector();
    vertex->uvo[0] = rcf.right();
    vertex->uvo[1] = rcf.bottom();
    vertex->uvo[2] = bottomOffset;
    vertex++;

    vertex->pos = QVector3D(x + w +  + offsetx, y + offsety, z);
    vertex->center = vec.toQVector();
    vertex->uvo[0] = rcf.right();
    vertex->uvo[1] = rcf.top();
    vertex->uvo[2] = bottomOffset;
    vertex++;

    x += w;
  }

  m_buffer.unmap();

  int count = 4 * text.size();
  m_offset += count;
}

void DynamicTextRenderer::begin(Transform *transform)
{
  m_program.bind();

  transform->getGl()->glActiveTexture(GL_TEXTURE0);
  transform->getGl()->glEnable(GL_TEXTURE_2D);
  m_program.setUniformValue("texture", 0);
  m_texture->bind();

  m_program.setUniformValue("u_scale", QVector3D(transform->m_width / transform->m_fov, transform->m_height / transform->m_fov, 1));
  m_program.setUniformValue("proj_matrix", transform->getProjectionMatrix());
  m_program.setUniformValue("view_matrix", transform->getViewMatrix());
  m_program.setUniformValue("inv_view_matrix", transform->getViewMatrix().inverted());
  m_program.setUniformValue("mvp_matrix", transform->getMVPMatrix());

  transform->getGl()->glEnable(GL_BLEND);
  transform->getGl()->glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

  m_offset = 0;
}

void DynamicTextRenderer::end(Transform *transform)
{
  if (m_offset > 0)
  {
    draw(transform);
  }

  m_program.release();
  m_texture->release();

  transform->getGl()->glDisable(GL_BLEND);
}

void DynamicTextRenderer::draw(Transform *transform)
{
  m_buffer.bind();
  int vertexLocation = m_program.attributeLocation("a_position");
  m_program.enableAttributeArray(vertexLocation);
  m_program.setAttributeBuffer(vertexLocation, GL_FLOAT, 0, 3, sizeof(dtVertex_t));

  int vertexLocation1 = m_program.attributeLocation("a_uvo");
  m_program.enableAttributeArray(vertexLocation1);
  m_program.setAttributeBuffer(vertexLocation1, GL_FLOAT, offsetof(dtVertex_t, uvo), 3, sizeof(dtVertex_t));

  int vertexLocation2 = m_program.attributeLocation("a_center");
  m_program.enableAttributeArray(vertexLocation2);
  m_program.setAttributeBuffer(vertexLocation2, GL_FLOAT, offsetof(dtVertex_t, center), 3, sizeof(dtVertex_t));

  transform->getGl()->glDrawArrays(GL_QUADS, 0, m_offset);
}


#include "dynamictextrenderer.h"
#include "renderer.h"
#include "transform.h"

#include <QPainter>

DynamicTextRenderer::DynamicTextRenderer() :
  m_from(32),
  m_to(128)
{
}

void DynamicTextRenderer::createTexture(int size, QString fontName, int fontSize)
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
  m_buffer.allocate(MAX_TEXT_CHARACTERS * 6 * sizeof(dtVertex_t));

  QFont font = QFont(fontName, fontSize);
  QImage image(size, size, QImage::Format_ARGB32_Premultiplied);

  image.fill(Qt::transparent);
  //image.fill(Qt::black);

  QPainter p(&image);

  p.setPen(Qt::white);
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

    p.setPen(Qt::red);
    //p.drawRect(rc);
    m_rectMap[i] = rc;

    p.setPen(Qt::white);
    p.drawText(rc, QChar(i));
    x += rc.width() + 1;
  }

  p.end();

  image.save("text.png", "PNG");

  m_texture = new QOpenGLTexture(image, QOpenGLTexture::DontGenerateMipMaps);
}

void DynamicTextRenderer::render(Transform *transform, const RaDec &raDec, const QString &text, int align)
{
  float wm = 1;
  float hm = (transform->m_height / transform->m_width);
  float x = 0, y = 0;
  float z = 0;
  float tfs = m_texture->width();
  dtVertex_t *vertex;

  m_buffer.bind();
  vertex = (dtVertex_t *)m_buffer.map(QOpenGLBuffer::WriteOnly);

  foreach (QChar ch, text)
  {
    QRect rc = m_rectMap[ch];
    float w, h;

    w = rc.width() * wm;
    h = rc.height() * hm;

    vertex->pos = QVector3D(x, y, z);
    vertex->uv[0] = (rc.left() - 1) / tfs;
    vertex->uv[1] = (rc.top() + 1) / tfs;
    vertex++;

    vertex->pos = QVector3D(x + w, y - h, z);
    vertex->uv[0] = (rc.right() + 1) / tfs;
    vertex->uv[1] = (rc.bottom() + 1) / tfs;
    vertex++;

    vertex->pos = QVector3D(x, y - h, z);
    vertex->uv[0] = (rc.left() - 1) / tfs;
    vertex->uv[1] = (rc.bottom() + 1) / tfs;
    vertex++;

    vertex->pos = QVector3D(x, y, z);
    vertex->uv[0] = (rc.left() - 1) / tfs;
    vertex->uv[1] = (rc.top() + 1) / tfs;
    vertex++;

    vertex->pos = QVector3D(x + w, y, z);
    vertex->uv[0] = (rc.right() + 1) / tfs;
    vertex->uv[1] = (rc.top() + 1) / tfs;
    vertex++;

    vertex->pos = QVector3D(x + w, y - h, z);
    vertex->uv[0] = (rc.right() + 1) / tfs;
    vertex->uv[1] = (rc.bottom() + 1) / tfs;
    vertex++;

    x += w;
  }

  int count = 6 * text.size();

  m_buffer.unmap();


  Vector3 vec;
  Transform::rdToVector(raDec, vec);
  m_program.setUniformValue("u_pos", vec.toQVector());

  m_buffer.bind();
  int vertexLocation = m_program.attributeLocation("a_position");
  m_program.enableAttributeArray(vertexLocation);
  m_program.setAttributeBuffer(vertexLocation, GL_FLOAT, 0, 3, sizeof(dtVertex_t));

  int vertexLocation1 = m_program.attributeLocation("a_uv");
  m_program.enableAttributeArray(vertexLocation1);
  m_program.setAttributeBuffer(vertexLocation1, GL_FLOAT, offsetof(dtVertex_t, uv), 2, sizeof(dtVertex_t));

  transform->getGl()->glDrawArrays(GL_TRIANGLES, 0, count);
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
}

void DynamicTextRenderer::end(Transform *transform)
{
  m_program.release();
  m_texture->release();

  transform->getGl()->glDisable(GL_BLEND);
}


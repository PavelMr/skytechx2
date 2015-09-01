#include "textrenderer.h"
#include "renderer.h"

#include <QImage>
#include <QPainter>
#include <QOpenGLFunctions>

static bool firstTime = true;
static QOpenGLShaderProgram m_program;


TextRenderer::TextRenderer() :
  m_image(0)
{
}

void TextRenderer::createTexture(QString objName, int size, QString fontName, int fontSize, QVector4D color)
{
  if (m_image)
  {
    delete m_image;
  }

  m_image = new QImage(size, size, QImage::Format_ARGB32_Premultiplied);
  m_image->fill(Qt::transparent);
  //m_image->fill(Qt::black);

  m_lastX = 0;
  m_lastY = 0;
  m_margin = 1;
  m_color = color;
  m_objName = objName;

  m_fontName = fontName;
  m_fontSize = fontSize;

  m_texts.clear();
  m_vertices.clear();
  m_glBuffer.destroy();
}

void TextRenderer::addText(const QString &text, const RaDec &rd, int align, float mag)
{
  if (text.length() == 0)
  {
    return;
  }

  QPainter p;
  int foundAt = -1;

  p.begin(m_image);
  p.setRenderHint(QPainter::Antialiasing);
  p.setRenderHint(QPainter::TextAntialiasing);
  p.setFont(QFont(m_fontName, m_fontSize));

  // TODO: kontrola stejneho textu

  int i = 0;
  foreach (texts_t t, m_texts)
  {
    if (t.text == text)
    {
      foundAt = i;
      break;
    }
    i++;
  }

  QRect br;

  if (foundAt == -1)
  {
    QFontMetrics fm(p.font());
    br = QRect(0, 0, fm.width(text) + m_margin * 2, fm.height() + m_margin * 2);

    if (br.width() >= m_image->width())
    {
      qDebug() << "addText() text width exceed image width" << m_image->width() << br.width();
      return;
    }

    while (1)
    {
      br.moveTopLeft(QPoint(m_lastX, m_lastY));
      if (br.right() > m_image->width())
      {
        m_lastX = 0;
        m_lastY += fm.height() + m_margin * 2;
        if (m_lastY + fm.height() + m_margin * 2 > m_image->height())
        {
          qDebug() << "addText() too many texts!!!" << text;
          return;
        }
      }
      else
      {
        break;
      }
    }

    texts_t texts;

    texts.text = text;
    texts.rect = br;
    m_texts.append(texts);

    p.setPen(Qt::white);
    p.drawText(br, Qt::AlignCenter, text);
    m_lastX = br.right() + 2;
  }
  else
  {
    br = m_texts[foundAt].rect;
  }

  textVertex_t v;
  Vector3 vec;

  Transform::rdToVector(rd, vec);

  v.x = vec.x;
  v.y = vec.y;
  v.z = vec.z;

  v.uv[0] = br.left() / (float)m_image->width();
  v.uv[1] = br.top() / (float)m_image->height();
  v.uv[2] = br.right() / (float)m_image->width();
  v.uv[3] = br.bottom() / (float)m_image->height();

  v.wh[0] = br.width();
  v.wh[1] = br.height();

  v.mag = mag;

  v.align = align;

  m_vertices.append(v);

  p.end();
}


void TextRenderer::createResources()
{
  m_glCount = m_vertices.count();

  for (int i = 0; i < m_glCount; i++)
  {
    //qDebug() << i << m_vertices[i].align;
  }

  m_glBuffer.create();
  m_glBuffer.bind();
  m_glBuffer.allocate(m_vertices.constData(), m_glCount * sizeof(textVertex_t));
  m_vertices.clear();

  m_texture = new QOpenGLTexture(*m_image, QOpenGLTexture::DontGenerateMipMaps);

  //delete m_image;
  m_texts.clear();

  if (firstTime)
  {
    // Compile vertex shader
    m_program.addShaderFromSourceCode(QOpenGLShader::Vertex, Renderer::readShader(":/res/opengl/text_vshader.glsl"));
    // Compile geometry shader
    m_program.addShaderFromSourceCode(QOpenGLShader::Geometry, Renderer::readShader(":/res/opengl/text_gshader.glsl"));
    // Compile fragment shader
    m_program.addShaderFromSourceCode(QOpenGLShader::Fragment, Renderer::readShader(":/res/opengl/text_fshader.glsl"));
    // Link shader pipeline
    if (!m_program.link())
    {
      qDebug() << m_program.log();
      exit(1);
    }
    firstTime = false;
  }
}

void TextRenderer::render(Transform *transform, float magLimit)
{
  if (!m_program.bind())
  {
    qDebug() << m_program.log();
    exit(1);
  }

  m_program.setUniformValue("mvp_matrix", transform->getMVPMatrix());

  if (!m_glBuffer.bind())
  {
    qDebug() << "ee";
    exit(1);
  }

  transform->getGl()->glActiveTexture(GL_TEXTURE0);
  glEnable(GL_TEXTURE_2D);
  m_program.setUniformValue("texture", 0);
  m_texture->bind();

  m_program.setUniformValue("u_mag_limit", magLimit);
  m_program.setUniformValue("f_texture", 0);
  m_program.setUniformValue("f_color", m_color);

  m_program.setUniformValue("u_scr_width", 1.f / (float)transform->m_width);
  m_program.setUniformValue("u_scr_height", 1.f / (float)transform->m_height);

  int vertexLocation = m_program.attributeLocation("a_position");
  m_program.enableAttributeArray(vertexLocation);
  m_program.setAttributeBuffer(vertexLocation, GL_FLOAT, 0, 3, sizeof(textVertex_t));

  int vertexLocation1 = m_program.attributeLocation("a_uv");
  m_program.enableAttributeArray(vertexLocation1);
  m_program.setAttributeBuffer(vertexLocation1, GL_FLOAT, offsetof(textVertex_t, uv), 4, sizeof(textVertex_t));

  int vertexLocation2 = m_program.attributeLocation("a_wh");
  m_program.enableAttributeArray(vertexLocation2);
  m_program.setAttributeBuffer(vertexLocation2, GL_FLOAT, offsetof(textVertex_t, wh), 2, sizeof(textVertex_t));

  int vertexLocation4 = m_program.attributeLocation("a_mag");
  m_program.enableAttributeArray(vertexLocation4);
  m_program.setAttributeBuffer(vertexLocation4, GL_FLOAT, offsetof(textVertex_t, mag), 1, sizeof(textVertex_t));

  int vertexLocation3 = m_program.attributeLocation("a_align");
  m_program.enableAttributeArray(vertexLocation3);
  m_program.setAttributeBuffer(vertexLocation3,  GL_FLOAT, offsetof(textVertex_t, align), 1, sizeof(textVertex_t));

  /*
  qDebug() << "offset uv" << offsetof(textVertex_t, uv);
  qDebug() << "offset wh" << offsetof(textVertex_t, wh);
  qDebug() << "offset mag" << offsetof(textVertex_t, mag);
  qDebug() << "offset al" << offsetof(textVertex_t, align);
  qDebug() << "size" << sizeof(textVertex_t);
  */


  glEnable(GL_BLEND);
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

  glDrawArrays(GL_POINTS, 0, m_glCount);

  glDisable(GL_BLEND);

  m_program.release();

}

void TextRenderer::save()
{
  m_image->save(m_objName + ".png", "PNG");
}


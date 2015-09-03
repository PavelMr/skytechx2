#include "painteroverlay.h"
#include "renderer.h"
#include "transform.h"

PainterOverlay::PainterOverlay() :
  m_texture(0)
{
  QVector4D data[4] = {QVector4D(-1,-1, 0, 1),
                       QVector4D(-1, 1, 0, 0),
                       QVector4D( 1,-1, 1, 1),
                       QVector4D( 1, 1, 1, 0)};


  m_buffer.create();
  m_buffer.bind();
  m_buffer.allocate(data, 4 * sizeof(QVector4D));

  m_program.addShaderFromSourceCode(QOpenGLShader::Vertex, Renderer::readShader(":/res/opengl/overlay2d_vshader.glsl"));
  m_program.addShaderFromSourceCode(QOpenGLShader::Fragment, Renderer::readShader(":/res/opengl/overlay2d_fshader.glsl"));

  m_program.link();

  //m_texture = new QOpenGLTexture(QOpenGLTexture::Target2D);
}

void PainterOverlay::render(Transform *transform, QImage *image)
{
  if (m_texture)
  {
    delete m_texture;
  }

  m_texture = new QOpenGLTexture(*image, QOpenGLTexture::DontGenerateMipMaps);

  m_program.bind();

  transform->getGl()->glActiveTexture(GL_TEXTURE0);
  transform->getGl()->glEnable(GL_TEXTURE_2D);
  m_program.setUniformValue("texture", 0);
  m_texture->bind();

  m_buffer.bind();

  int vertexLocation = m_program.attributeLocation("a_positionWithUV");
  m_program.enableAttributeArray(vertexLocation);
  m_program.setAttributeBuffer(vertexLocation, GL_FLOAT, 0, 4, sizeof(QVector4D));

  transform->getGl()->glEnable(GL_BLEND);
  transform->getGl()->glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

  transform->getGl()->glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

  transform->getGl()->glDisable(GL_BLEND);

  m_program.release();
  m_texture->release();
}

void PainterOverlay::createTexture(QImage *image)
{
  /*
  if (m_texture)
  {
    delete m_texture;
  }

  m_texture = new QOpenGLTexture(*image, QOpenGLTexture::DontGenerateMipMaps);
  */
}


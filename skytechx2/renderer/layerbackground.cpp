#include "layerbackground.h"
#include "renderer.h"
#include "transform.h"

#include <QVector3D>
#include <QOpenGLTexture>

QOpenGLTexture *m_texture;

LayerBackground::LayerBackground()
{

}

void LayerBackground::createResources()
{
  QVector3D vertices[8] = {QVector3D(-0.5, -0.5,  0.5),
                           QVector3D(0.5, -0.5,  0.5),
                           QVector3D(-0.5,  0.5,  0.5),
                           QVector3D(0.5,  0.5,  0.5),
                           QVector3D(-0.5, -0.5, -0.5),
                           QVector3D(0.5, -0.5, -0.5),
                           QVector3D(-0.5,  0.5, -0.5),
                           QVector3D(0.5,  0.5, -0.5),
                          };

  m_vertices.create();
  m_vertices.bind();
  m_vertices.allocate((void *)vertices, 8 * sizeof(QVector3D));

  // Compile vertex shader
  m_program.addShaderFromSourceCode(QOpenGLShader::Vertex, Renderer::readShader(":/res/opengl/bk_vshader.glsl"));
  // Compile fragment shader
  m_program.addShaderFromSourceCode(QOpenGLShader::Fragment, Renderer::readShader(":/res/opengl/bk_fshader.glsl"));
  // Link shader pipeline
  if (!m_program.link())
  {
    qDebug() << m_program.log();
    exit(1);
  }

  m_texture = new QOpenGLTexture(QOpenGLTexture::TargetCubeMap);
  m_texture->create();
  m_texture->setSize(1024, 1024);
  m_texture->setFormat(QOpenGLTexture::RGBA8_UNorm);
  m_texture->allocateStorage();

  QImage img_pX = QImage("resources/milkyway/milkyway2_outright.png");
  m_texture->setData(0, 0, QOpenGLTexture::CubeMapPositiveX, QOpenGLTexture::RGBA, QOpenGLTexture::UInt8, img_pX.bits());
  QImage img_nX = QImage("resources/milkyway/milkyway2_outleft.png");
  m_texture->setData(0, 0, QOpenGLTexture::CubeMapNegativeX, QOpenGLTexture::RGBA, QOpenGLTexture::UInt8, img_nX.bits());

  QImage img_pZ = QImage("resources/milkyway/milkyway2_outtop.png");
  m_texture->setData(0, 0, QOpenGLTexture::CubeMapPositiveY, QOpenGLTexture::RGBA, QOpenGLTexture::UInt8, img_pZ.bits());
  QImage img_nZ = QImage("resources/milkyway/milkyway2_outbottom.png");
  m_texture->setData(0, 0, QOpenGLTexture::CubeMapNegativeY, QOpenGLTexture::RGBA, QOpenGLTexture::UInt8, img_nZ.bits());

  QImage img_pY = QImage("resources/milkyway/milkyway2_outfront.png");
  m_texture->setData(0, 0, QOpenGLTexture::CubeMapPositiveZ, QOpenGLTexture::RGBA, QOpenGLTexture::UInt8, img_pY.bits());
  QImage img_nY = QImage("resources/milkyway/milkyway2_outback.png");
  m_texture->setData(0, 0, QOpenGLTexture::CubeMapNegativeZ, QOpenGLTexture::RGBA, QOpenGLTexture::UInt8, img_nY.bits());

  m_texture->setWrapMode(QOpenGLTexture::ClampToEdge);

  m_texture->bind();

}

void LayerBackground::render(Transform *transform)
{
  m_texture->bind();

  m_program.bind();

  m_program.setUniformValue("mvp_matrix", transform->getMVPMatrix());

  m_vertices.bind();

  int vertexLocation = m_program.attributeLocation("a_position");
  m_program.enableAttributeArray(vertexLocation);
  m_program.setAttributeBuffer(vertexLocation, GL_FLOAT, 0, 3, sizeof(QVector3D));

  static const GLushort indices[] = {
      0, 1, 2, 3, 7, 1, 5, 4, 7, 6, 2, 4, 0, 1
  };

  transform->getGl()->glDrawElements(GL_TRIANGLE_STRIP, 14, GL_UNSIGNED_SHORT, indices);

}


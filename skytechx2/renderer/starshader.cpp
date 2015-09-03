#include "starshader.h"
#include "renderer.h"

#include <QOpenGLTexture>
#include <QOpenGLShaderProgram>

StarShader::StarShader()
{

}

void StarShader::createResources()
{
  m_program.addShaderFromSourceCode(QOpenGLShader::Vertex, Renderer::readShader(":/res/opengl/vshader.glsl"));
  m_program.addShaderFromSourceCode(QOpenGLShader::Geometry, Renderer::readShader(":/res/opengl/gshader.glsl"));
  m_program.addShaderFromSourceCode(QOpenGLShader::Fragment, Renderer::readShader(":/res/opengl/fshader.glsl"));

  m_program.link();

  m_texture = new QOpenGLTexture(QImage(":/res/stars/star.png"));
  m_textureHalo = new QOpenGLTexture(QImage(":/res/stars/star_halo.png"));
}


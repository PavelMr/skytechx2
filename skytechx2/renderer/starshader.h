#ifndef STARSHADER_H
#define STARSHADER_H

#include <QOpenGLShaderProgram>

class QOpenGLTexture;

class StarShader
{
public:
  StarShader();
  void createResources();

public:
  QOpenGLShaderProgram m_program;
  QOpenGLTexture *m_texture;
  QOpenGLTexture *m_textureHalo;
};

#endif // STARSHADER_H

#include "layerconstellations.h"
#include "dataresources.h"
#include "constellations.h"
#include "transform.h"
#include "renderer.h"

#include <QVector>
#include <QOpenGLTexture>
#include <QGLFunctions>

#include <math.h>

QOpenGLTexture *texture;

typedef struct
{
  float x, y, z;
  float u;
} lineVertex_t;

LayerConstellations::LayerConstellations()
{
}

void LayerConstellations::createResources()
{
  createResourcesNames();
  createResourcesLines();
  createResourcesBoundaries();

  texture = new QOpenGLTexture(QImage(":/res/stars/dot.png"));
  texture->setWrapMode(QOpenGLTexture::Repeat);
}

void LayerConstellations::createResourcesLines()
{
  constellationLines_t from, to;

  // suppress warning
  from.rd = RaDec(0, 0);
  to.rd = RaDec(0, 0);

  QVector <lineVertex_t> vertex;

  m_glLinesCount = 0;
  foreach (constellationLines_t l, g_dataResource->getConstellations()->getLines())
  {
    switch (l.cmd)
    {
      case 0: // data from
        from = l;
        break;

      case 1: // 1-main line to
        to = l;
        break;

      case 2: // 2-sec. line to
        to = l;
        break;
    }

    if (l.cmd > 0)
    {
      lineVertex_t v1, v2;
      Vector3 vec;

      Transform::rdToVector(from.rd, vec);
      v1.x = vec.x;
      v1.y = vec.y;
      v1.z = vec.z;
      v1.u = 0;
      m_glLinesCount++;

      Transform::rdToVector(to.rd, vec);
      v2.x = vec.x;
      v2.y = vec.y;
      v2.z = vec.z;
      v2.u = 0;
      m_glLinesCount++;

      vertex.append(v1);
      vertex.append(v2);

      from = to;
    }
  }


  m_glLines.setUsagePattern(QOpenGLBuffer::StaticDraw);
  m_glLines.create();
  m_glLines.bind();
  m_glLines.allocate(vertex.constData(), sizeof(lineVertex_t) * vertex.count());

  // programs

  // Compile vertex shader
  m_lineProgram.addShaderFromSourceCode(QOpenGLShader::Vertex, Renderer::readShader(":/res/opengl/const_line_vshader.glsl"));
  // Compile fragment shader
  m_lineProgram.addShaderFromSourceCode(QOpenGLShader::Fragment, Renderer::readShader(":/res/opengl/const_line_fshader.glsl"));
  // Link shader pipeline
  if (!m_lineProgram.link())
  {
    qDebug() << m_lineProgram.log();
    exit(1);
  }
}

void LayerConstellations::createResourcesBoundaries()
{
  m_glBoundariesCount = 0;
  Constellations *constellation = g_dataResource->getConstellations();

  QVector <lineVertex_t> vertex;

  int       inter;
  //double    df;
  //SKPOINT   p1, p2;
  //SKMATRIX  prec;


   //precessMatrix(2405889.25855, JD2000, &prec);

  /*
   df = RAD2DEG(view->fov);
   if (df < 15)
     inter = 5;
   else
     inter = 1;
     */

   constBndLines_t *pCon;
   constBndLines_t *pCon2;

   for (int a = 0; a < constellation->m_numConstelNBnd - 1; a++)
   {
     pCon  = &constellation->m_constelNBnd[a];
     pCon2 = &constellation->m_constelNBnd[a + 1];

     if (pCon->p)
     {
       lineVertex_t v1, v2;
       Vector3 vec;

       Transform::rdToVector(pCon->rd, vec);
       v1.x = vec.x;
       v1.y = vec.y;
       v1.z = vec.z;
       v1.u = 0;
       m_glBoundariesCount++;

       Transform::rdToVector(pCon2->rd, vec);
       v2.x = vec.x;
       v2.y = vec.y;
       v2.z = vec.z;

       Vector3 v;

       SKVecSub(&v, Vector3(v1.x, v1.y, v1.z), Vector3(v2.x, v2.y, v2.z));
       v2.u = 50 * sqrt(v.x * v.x + v.y * v.y + v.z * v.z);;
       m_glBoundariesCount++;

       vertex.append(v1);
       vertex.append(v2);
     }
   }

   m_glBoundaries.create();
   m_glBoundaries.bind();
   m_glBoundaries.allocate(vertex.constData(), sizeof(lineVertex_t) * vertex.count());
}

void LayerConstellations::createResourcesNames()
{
  Constellations *constellation = g_dataResource->getConstellations();

  m_namesRenderer.createTexture("const_name" ,256, "arial", 12);

  for (int i = 0; i < 88; i++)
  {
    constNames_t names = constellation->getNames(i);
    m_namesRenderer.addText(names.shortName, names.rd, GL_TA_CENTER);
  }

  m_namesRenderer.createResources();
  m_namesRenderer.save();
}

void LayerConstellations::render(Transform *transform)
{
  if (!m_lineProgram.bind())
  {
    qDebug() << m_lineProgram.log();
    exit(1);
  }

  transform->getGl()->glActiveTexture(GL_TEXTURE0);
  transform->getGl()->glEnable(GL_TEXTURE_2D);
  m_lineProgram.setUniformValue("texture", 0);
  texture->bind();

  //glLineWidth(2);
  transform->getGl()->glEnable(GL_BLEND);
  transform->getGl()->glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

  m_lineProgram.setUniformValue("mvp_matrix",  transform->getMVPMatrix());
  m_lineProgram.setUniformValue("scale", (float)(transform->m_fov));

  m_glLines.bind();

  int vertexLocation = m_lineProgram.attributeLocation("a_position");
  m_lineProgram.enableAttributeArray(vertexLocation);
  m_lineProgram.setAttributeBuffer(vertexLocation, GL_FLOAT, 0, 3, sizeof(lineVertex_t));

  int vertexLocation1 = m_lineProgram.attributeLocation("a_u");
  m_lineProgram.enableAttributeArray(vertexLocation1);
  m_lineProgram.setAttributeBuffer(vertexLocation1, GL_FLOAT, 3 * sizeof(float), 1, sizeof(lineVertex_t));

  transform->getGl()->glDrawArrays(GL_LINES, 0, m_glLinesCount);
  //glDisable(GL_BLEND);

  ///////////

  //glLineWidth(1);
  m_glBoundaries.bind();

  vertexLocation = m_lineProgram.attributeLocation("a_position");
  m_lineProgram.enableAttributeArray(vertexLocation);
  m_lineProgram.setAttributeBuffer(vertexLocation, GL_FLOAT, 0, 3, sizeof(lineVertex_t));

  vertexLocation1 = m_lineProgram.attributeLocation("a_u");
  m_lineProgram.enableAttributeArray(vertexLocation1);
  m_lineProgram.setAttributeBuffer(vertexLocation1, GL_FLOAT, 3 * sizeof(float), 1, sizeof(lineVertex_t));

  transform->getGl()->glDrawArrays(GL_LINES, 0, m_glBoundariesCount);


  transform->getGl()->glDisable(GL_BLEND);
  m_lineProgram.release();
  texture->release();

  m_namesRenderer.render(transform);
}



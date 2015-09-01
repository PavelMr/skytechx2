#include "layertychostars.h"
#include "dataresources.h"
#include "starvertices.h"
#include "tycho.h"
#include "transform.h"
#include "gscregions.h"
#include "textrenderer.h"
#include "renderer.h"

#include <QOpenGLShaderProgram>
#include <QOpenGLDebugLogger>

static QOpenGLShaderProgram program;
static QOpenGLTexture *texture;
static QOpenGLTexture *texture1;

LayerTychoStars::LayerTychoStars()
{

}

void LayerTychoStars::createReources()
{
  starVertex_t *vertex;
  Tycho *tycho = g_dataResource->getTycho();

  m_regionGlVertices = new QOpenGLBuffer[tycho->m_head.regCount];
  memset(m_magRegionCount, 0, sizeof(m_magRegionCount));

  qDebug() << "init start" << sizeof(m_magRegionCount);

  m_textRendererNames.createTexture("names", 512, "arial", 12, QVector4D(0, 1, 0, 1));
  m_textRendererIdFlm.createTexture("flm", 256, "Arial", 9, QVector4D(0, 1, 1, 1));
  m_textRendererIdBayer.createTexture("bayer", 256, "Symbol", 10, QVector4D(0, 1, 1, 1));

  for (int region = 0; region < tycho->m_head.regCount; region++)
  {
    tychoRegion2_t *reg = &tycho->m_region[region];
    vertex = new starVertex_t[reg->region.numStars];

    for (int i = 0; i < reg->region.numStars; i++)
    {
      float mag = Tycho::getMagnitude(reg->stars[i]);
      Vector3 vec;
      Transform::rdToVector(RaDec(reg->stars[i].ra, reg->stars[i].dec), vec);

      if (reg->stars[i].supIndex != -1)
      {
        tychoSupp_t *supp = &tycho->m_supplement[reg->stars[i].supIndex];
        if (supp->pnOffs != 0xffff)
        {
          m_textRendererNames.addText(tycho->getStarName(supp), RaDec(reg->stars[i].ra, reg->stars[i].dec), GL_TA_BOTTOM, mag);
        }

        bool    bFlamsteed;
        QString flamsteed = tycho->getFlamsteedString(supp, bFlamsteed);

        if (bFlamsteed)
        {
          m_textRendererIdFlm.addText(flamsteed, RaDec(reg->stars[i].ra, reg->stars[i].dec), GL_TA_TOP, mag);
        }

        bool    bBayer;
        QString bayer = tycho->getBayerString(supp, bBayer);

        if (bBayer)
        {
          m_textRendererIdBayer.addText(bayer, RaDec(reg->stars[i].ra, reg->stars[i].dec), GL_TA_LEFT, mag);
        }
      }

      vertex[i].x = vec.x;
      vertex[i].y = vec.y;
      vertex[i].z = vec.z;

      vertex[i].colorMagnitude[0] = 1;
      vertex[i].colorMagnitude[1] = 1;
      vertex[i].colorMagnitude[2] = 1;

      vertex[i].colorMagnitude[3] = mag;
      //m_magRegionCount[region][(int)(mag * 4)] = i; clamp
    }

    m_regionGlVertices[region].create();
    m_regionGlVertices[region].bind();
    m_regionGlVertices[region].allocate(vertex, reg->region.numStars * sizeof(starVertex_t));

    delete vertex;
  }

  m_textRendererNames.createResources();
  m_textRendererIdFlm.createResources();
  m_textRendererIdBayer.createResources();

  m_textRendererNames.save();
  m_textRendererIdFlm.save();
  m_textRendererIdBayer.save();

  bool b;


  b = program.addShaderFromSourceCode(QOpenGLShader::Vertex, Renderer::readShader(":/res/opengl/vshader.glsl"));
  b = program.addShaderFromSourceCode(QOpenGLShader::Geometry, Renderer::readShader(":/res/opengl/gshader.glsl"));
  b = program.addShaderFromSourceCode(QOpenGLShader::Fragment, Renderer::readShader(":/res/opengl/fshader.glsl"));

  // Link shader pipeline
  b = program.link();

  texture = new QOpenGLTexture(QImage(":/res/stars/star.png"));
  texture1 = new QOpenGLTexture(QImage(":/res/stars/star_halo.png"));

  qDebug("init done");
}

#define FRAC(f, from, to)      ((((f) - (from)) / (double)((to) - (from))))


void LayerTychoStars::render(Transform *transform)
{
  float magMag = 15.5;
  float starPlus = 5;

  QEasingCurve curve(QEasingCurve::InExpo);
  magMag = starPlus + 5 + 5.0 * curve.valueForProgress(FRAC(transform->getMapParam()->m_fov, SkMath::toRad(90), SkMath::toRad(0.05)));

  Tycho *tycho = g_dataResource->getTycho();

  program.bind();

  program.setUniformValue("mvp_matrix", transform->getMVPMatrix());

  program.setUniformValue("u_modelview", transform->getViewMatrix());
  program.setUniformValue("u_projection", transform->getProjectionMatrix());

  program.setUniformValue("u_scr_width", 1.f / (float)transform->m_width);
  program.setUniformValue("u_scr_height", 1.f / (float)transform->m_height);

  program.setUniformValue("u_mag_limit", (float)magMag);
  program.setUniformValue("pixel_ratio", (float)transform->m_ratio);

  transform->getGl()->glActiveTexture(GL_TEXTURE0);
  glEnable(GL_TEXTURE_2D);
  program.setUniformValue("texture", 0);
  texture->bind();

  transform->getGl()->glActiveTexture(GL_TEXTURE1);
  glEnable(GL_TEXTURE_2D);
  program.setUniformValue("texture1", 1);
  texture1->bind();


  glEnable(GL_BLEND);
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

  QList <int> *list = g_dataResource->getGscRegions()->getVisibleRegions();

  foreach (int i, *list)
  {
    m_regionGlVertices[i].bind();

    int vertexLocation = program.attributeLocation("a_position");
    program.enableAttributeArray(vertexLocation);
    program.setAttributeBuffer(vertexLocation, GL_FLOAT, 0, 3, sizeof(starVertex_t));

    int vertexLocation1 = program.attributeLocation("a_colorMagnitude");
    program.enableAttributeArray(vertexLocation1);
    program.setAttributeBuffer(vertexLocation1, GL_FLOAT, 3 * sizeof(float), 4, sizeof(starVertex_t));

    glDrawArrays(GL_POINTS, 0, tycho->m_region[i].region.numStars);
  }

  glDisable(GL_BLEND);
  glDisable(GL_POINT_SPRITE);
  glDisable(GL_VERTEX_PROGRAM_POINT_SIZE);

  texture->release();
  texture1->release();
  program.release();

  m_textRendererNames.render(transform, magMag);

  if (transform->getMapParam()->m_fov <= SkMath::toRad(70))
    m_textRendererIdBayer.render(transform, magMag);

  if (transform->getMapParam()->m_fov <= SkMath::toRad(50))
    m_textRendererIdFlm.render(transform, magMag);
}

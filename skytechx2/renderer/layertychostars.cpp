#include "layertychostars.h"
#include "dataresources.h"
#include "starvertices.h"
#include "tycho.h"
#include "transform.h"
#include "gscregions.h"
#include "textrenderer.h"
#include "renderer.h"
#include "starshader.h"
#include "skmath.h"
#include "mapobject.h"

#include <QOpenGLShaderProgram>
#include <QOpenGLDebugLogger>
#include <QImage>


static Vector3 colorTable[256];

static QVector3D colorSP[8] = {QVector3D(255, 255, 255),
                               QVector3D(155, 176, 255),
                               QVector3D(170, 191, 255),
                               QVector3D(202, 212, 255),
                               QVector3D(248, 247, 255),
                               QVector3D(255, 244, 234),
                               QVector3D(255, 210, 161),
                               QVector3D(255, 204, 111)};


LayerTychoStars::LayerTychoStars()
{

}

static int BvToColorIndex(double b_v) {
  b_v = CLAMP(b_v, -0.5, 2.0);

  return FRAC(b_v, -0.5, 2.0) * 255;
}


Vector3 bv2rgb(double bv)    // RGB <0,1> <- BV <-0.4,+2.0> [-]
{
  int index = BvToColorIndex(bv);

  return colorTable[index];
}

void LayerTychoStars::createReources()
{
  QImage *m_colorIndexImage = new QImage(":/res/stars/colorindex.png");
  quint32 *data = (quint32 *)m_colorIndexImage->bits();

  for (int i = 0; i < 256; i++)
  {
    QColor color = QColor(data[i]);

    colorTable[i].x = color.red() / 255.f;
    colorTable[i].y = color.green() / 255.f;
    colorTable[i].z = color.blue() / 255.f;

    //qDebug() << i << color;
  }


  starVertex_t *vertex;
  Tycho *tycho = g_dataResource->getTycho();

  m_regionGlVertices = new QOpenGLBuffer[tycho->m_head.regCount];

  m_textRendererNames.createTexture("names", 512, "arial", 12, QVector4D(0, 1, 0, 1));
  m_textRendererIdFlm.createTexture("flm", 256, "Arial", 9, QVector4D(0, 1, 1, 1));
  m_textRendererIdBayer.createTexture("bayer", 256, "Symbol", 12, QVector4D(0, 1, 1, 1));

  int maxx = 0;

  for (int region = 0; region < tycho->m_head.regCount; region++)
  {
    tychoRegion2_t *reg = &tycho->m_region[region];
    vertex = new starVertex_t[reg->region.numStars];
    int spectralIndex = 0;

    if (reg->region.numStars > maxx)
      maxx = reg->region.numStars;

    for (int i = 0; i < reg->region.numStars; i++)
    {
      float mag = Tycho::getMagnitude(reg->stars[i]);

      Vector3 vec;
      Transform::rdToVector(RaDec(reg->stars[i].ra, reg->stars[i].dec), vec);

      if (reg->stars[i].supIndex != -1)
      {
        tychoSupp_t *supp = &tycho->m_supplement[reg->stars[i].supIndex];

        spectralIndex = supp->spt;

        if (supp->pnOffs != 0xffff)
        {
          m_textRendererNames.addText(tycho->getStarName(supp), RaDec(reg->stars[i].ra, reg->stars[i].dec), GL_TA_BOTTOM, mag);
        }

        if (supp->hd == 148478)
        {
          int a= 1;
          a++;
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

      float bv = (reg->stars[i].BTmag - reg->stars[i].VTmag) / 1000.0;

      Vector3 col;
      col = bv2rgb(bv);

      vertex[i].colorMagnitude[0] = col.x;
      vertex[i].colorMagnitude[1] = col.y;
      vertex[i].colorMagnitude[2] = col.z;

      vertex[i].colorMagnitude[3] = mag;
    }

    m_regionGlVertices[region].create();
    m_regionGlVertices[region].bind();
    m_regionGlVertices[region].allocate(vertex, reg->region.numStars * sizeof(starVertex_t));

    delete vertex;
  }

  qDebug() << "max" << maxx;

  m_textRendererNames.createResources();
  m_textRendererIdFlm.createResources();
  m_textRendererIdBayer.createResources();

  m_textRendererNames.save();
  m_textRendererIdFlm.save();
  m_textRendererIdBayer.save();

  qDebug("init done");
}

void LayerTychoStars::render(Transform *transform, Renderer *renderer)
{
  Tycho *tycho = g_dataResource->getTycho();

  QOpenGLShaderProgram *program = &renderer->getStarShaderProgram()->m_program;
  QOpenGLTexture *texture= renderer->getStarShaderProgram()->m_texture;
  QOpenGLTexture *textureHalo = renderer->getStarShaderProgram()->m_textureHalo;

  program->bind();

  program->setUniformValue("mvp_matrix", transform->getMVPMatrix());

  program->setUniformValue("u_modelview", transform->getViewMatrix());
  program->setUniformValue("u_projection", transform->getProjectionMatrix());

  program->setUniformValue("u_scr_width", 1.f / (float)transform->m_width);
  program->setUniformValue("u_scr_height", 1.f / (float)transform->m_height);

  program->setUniformValue("u_mag_limit", (float)transform->getMapParam()->m_maxStarMag);
  program->setUniformValue("pixel_ratio", (float)transform->m_ratio);

  transform->getGl()->glActiveTexture(GL_TEXTURE0);
  transform->getGl()->glEnable(GL_TEXTURE_2D);
  program->setUniformValue("texture", 0);
  texture->bind();

  transform->getGl()->glActiveTexture(GL_TEXTURE1);
  transform->getGl()->glEnable(GL_TEXTURE_2D);
  program->setUniformValue("texture1", 1);
  textureHalo->bind();

  transform->getGl()->glEnable(GL_BLEND);
  transform->getGl()->glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

  QList <int> *list = g_dataResource->getGscRegions()->getVisibleRegions();

  float maxMag = transform->getMapParam()->m_maxStarMag;

  foreach (int i, *list)
  {
    m_regionGlVertices[i].bind();

    int vertexLocation = program->attributeLocation("a_position");
    program->enableAttributeArray(vertexLocation);
    program->setAttributeBuffer(vertexLocation, GL_FLOAT, 0, 3, sizeof(starVertex_t));

    int vertexLocation1 = program->attributeLocation("a_colorMagnitude");
    program->enableAttributeArray(vertexLocation1);
    program->setAttributeBuffer(vertexLocation1, GL_FLOAT, 3 * sizeof(float), 4, sizeof(starVertex_t));

    transform->getGl()->glDrawArrays(GL_POINTS, 0, tycho->m_region[i].region.numStars);

    g_dataResource->getMapObject()->addRegion(i);
  }

  transform->getGl()->glDisable(GL_BLEND);
  transform->getGl()->glDisable(GL_POINT_SPRITE);
  transform->getGl()->glDisable(GL_VERTEX_PROGRAM_POINT_SIZE);

  texture->release();
  textureHalo->release();
  program->release();

  m_textRendererNames.render(transform, transform->getMapParam()->m_maxStarMag);

  if (transform->getMapParam()->m_fov <= SkMath::toRad(60))
    m_textRendererIdBayer.render(transform, transform->getMapParam()->m_maxStarMag);

  if (transform->getMapParam()->m_fov <= SkMath::toRad(40))
    m_textRendererIdFlm.render(transform, transform->getMapParam()->m_maxStarMag);
}

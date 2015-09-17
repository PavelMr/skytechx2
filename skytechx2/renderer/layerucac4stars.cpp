#include "layerucac4stars.h"
#include "dataresources.h"
#include "skmath.h"
#include "transform.h"
#include "starshader.h"
#include "gscregions.h"
#include "renderer.h"
#include "starvertices.h"
#include "mapobject.h"

#include <QOpenGLShaderProgram>
#include <QOpenGLTexture>
#include <QDebug>
#include <QEasingCurve>
#include <QColor>

#include <QCursor>

static Vector3 colorTable[256];

static int BvToColorIndex(double b_v) {
  b_v = CLAMP(b_v, -0.5, 2.0);

  return FRAC(b_v, -0.5, 2.0) * 255;
}


static Vector3 bv2rgb(double bv)    // RGB <0,1> <- BV <-0.4,+2.0> [-]
{
  int index = BvToColorIndex(bv);

  return colorTable[index];
}

LayerUCAC4Stars::LayerUCAC4Stars()
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

  delete m_colorIndexImage;
}

void LayerUCAC4Stars::render(Transform *transform, Renderer *renderer)
{
  UCAC4 *ucac4 = g_dataResource->getUCAC4();

  QList <int> *list = g_dataResource->getGscRegions()->getVisibleRegions();

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

  float maxMag = transform->getMapParam()->m_maxStarMag;

  foreach (int i, *list)
  {
    ucac4Region_t *region = ucac4->getRegion(i);

    if (region)
    {
      if (!region->m_buffer.isCreated())
      {
        //qDebug() << "create ucac4" << i;
        createRegionBuffer(region);
      }

      region->m_buffer.bind();

      int vertexLocation = program->attributeLocation("a_position");
      program->enableAttributeArray(vertexLocation);
      program->setAttributeBuffer(vertexLocation, GL_FLOAT, 0, 3, sizeof(starVertex_t));

      int vertexLocation1 = program->attributeLocation("a_colorMagnitude");
      program->enableAttributeArray(vertexLocation1);
      program->setAttributeBuffer(vertexLocation1, GL_FLOAT, 3 * sizeof(float), 4, sizeof(starVertex_t));

      transform->getGl()->glDrawArrays(GL_POINTS, 0, region->m_buffer.size() / sizeof(starVertex_t));

      /*
      UCAC4_Star_t *star = (UCAC4_Star_t *)region->m_stars.constData();
      for (int j = 0; j < region->m_stars.count(); j++, star++)
      {
        if (star->mag2 <= maxMag)
        {
          g_dataResource->getMapObject()->addUCAC4(i, j, star->ra / 1000., star->spd / 1999.9, star->mag2);
        }
      }
      */
    }
  }

  transform->getGl()->glDisable(GL_BLEND);
  transform->getGl()->glDisable(GL_POINT_SPRITE);
  transform->getGl()->glDisable(GL_VERTEX_PROGRAM_POINT_SIZE);

  texture->release();
  textureHalo->release();
  program->release();
}

void LayerUCAC4Stars::createRegionBuffer(ucac4Region_t *region)
{
  QVector <starVertex_t> vertices;

  region->m_buffer.create();
  region->m_buffer.bind();

  for (int i = 0; i < region->m_stars.count(); i++)
  {
    UCAC4_Star_t *star = &region->m_stars[i];

    if (star->mag2 / 1000.0 >= 11)
    {
      starVertex_t data;
      Vector3 vec;
      Transform::rdToVector(RaDec(SkMath::toRad(star->ra / 3600. / 1000.0),
                                  SkMath::toRad((star->spd / 3600. / 1000.0) - 90.0)), vec);


      data.x = vec.x;
      data.y = vec.y;
      data.z = vec.z;

      int b = star->apass_mag[1];
      int v = star->apass_mag[2];
      if (b < 30000 && v < 30000)
      {
        double bv = (b - v) / 1000.0;
        Vector3 col = bv2rgb(bv);

        data.colorMagnitude[0] = col.x;
        data.colorMagnitude[1] = col.y;
        data.colorMagnitude[2] = col.z;
      }
      else
      {
        data.colorMagnitude[0] = 1;
        data.colorMagnitude[1] = 0;
        data.colorMagnitude[2] = 0;
      }

      data.colorMagnitude[3] = star->mag2 / 1000.f;

      vertices.append(data);
    }
  }

  region->m_buffer.allocate(vertices.constData(), vertices.count() * sizeof(starVertex_t));
}


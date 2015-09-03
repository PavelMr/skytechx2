#include "layergscstars.h"
#include "transform.h"
#include "dataresources.h"
#include "gscregions.h"
#include "gsc.h"
#include "starvertices.h"
#include "renderer.h"
#include "starshader.h"
#include "skmath.h"

#include <QOpenGLShaderProgram>
#include <QOpenGLTexture>
#include <QDebug>

LayerGSCStars::LayerGSCStars()
{

}

void LayerGSCStars::render(Transform *transform, Renderer *renderer)
{
  GSC *gsc = g_dataResource->getGSC();

  float magMag = 15.5;
  float starPlus = 5;

  QEasingCurve curve(QEasingCurve::InExpo);
  magMag = starPlus + 5 + 5.0 * curve.valueForProgress(FRAC(transform->getMapParam()->m_fov, SkMath::toRad(90), SkMath::toRad(0.1)));

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

  program->setUniformValue("u_mag_limit", magMag);
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

  foreach (int i, *list)
  {
    gscStarRegion_t *region = gsc->getRegion(i);

    if (region)
    {
      if (!region->m_buffer.isCreated())
      {
        //qDebug() << "create gsc" << i;
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
    }
  }

  transform->getGl()->glDisable(GL_BLEND);
  transform->getGl()->glDisable(GL_POINT_SPRITE);
  transform->getGl()->glDisable(GL_VERTEX_PROGRAM_POINT_SIZE);

  texture->release();
  textureHalo->release();
  program->release();
}

void LayerGSCStars::createRegionBuffer(gscStarRegion_t *region)
{
  QVector <starVertex_t> vertices;

  region->m_buffer.create();
  region->m_buffer.bind();

  for (int i = 0; i < region->h.nobj; i++)
  {
    gscStar_t *star = &region->gsc[i];

    if (star->pMag >= 11)
    {
      starVertex_t data;
      Vector3 vec;
      Transform::rdToVector(star->rd, vec);

      data.x = vec.x;
      data.y = vec.y;
      data.z = vec.z;

      data.colorMagnitude[0] = 1;
      data.colorMagnitude[1] = 1;
      data.colorMagnitude[2] = 1;

      data.colorMagnitude[3] = star->pMag;

      vertices.append(data);
    }
  }

  region->m_buffer.allocate(vertices.constData(), vertices.count() * sizeof(starVertex_t));
}


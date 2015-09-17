#include "layerdso.h"
#include "dso.h"
#include "dataresources.h"
#include "renderer.h"
#include "mapobject.h"
#include "debugwidget.h"

#include "dynamictextrenderer.h"

DynamicTextRenderer dtr;

#include <QVector3D>

#define MAX_DSO_ON_SCREEN   50000

typedef struct
{
  QVector3D  pos;
  float      params[4]; // galaxy - PA
  float      intVal;
} dsoVertex_t;

LayerDSO::LayerDSO()
{
}

void LayerDSO::createResources()
{
  dtr.createTexture(512, "arial", 12, Qt::red);

  m_program.addShaderFromSourceCode(QOpenGLShader::Vertex, Renderer::readShader(":/res/opengl/dso_vshader.glsl"));
  m_program.addShaderFromSourceCode(QOpenGLShader::Geometry, Renderer::readShader(":/res/opengl/dso_gshader.glsl"));
  m_program.addShaderFromSourceCode(QOpenGLShader::Fragment, Renderer::readShader(":/res/opengl/dso_fshader.glsl"));
  if (!m_program.link())
  {
    qDebug() << "error" << m_program.log();
  }

  qDebug() << "link";

  //DSO *dso = g_dataResource->getDSO();

  m_buffer.create();
  m_buffer.setUsagePattern(QOpenGLBuffer::DynamicDraw);
  m_buffer.bind();
  m_buffer.allocate(MAX_DSO_ON_SCREEN * sizeof(dsoVertex_t));
}


void LayerDSO::render(Transform *transform)
{
  QVector <int> dsoTexts;
  //int ccq = 0;
  int count = 0;
  DSO *dso = g_dataResource->getDSO();

  m_buffer.bind();
  dsoVertex_t *ptr = (dsoVertex_t *)m_buffer.map(QOpenGLBuffer::WriteOnly);

  float minSize = (1.f / (float)transform->m_width) * 10;

  for (int y = 0; y < NUM_DSO_SEG_Y; y++)
  {
    for (int x = 0; x < NUM_DSO_SEG_X; x++)
    {
      if (!SKPLANECheckFrustumToPolygon(transform->getFrustum(), dso->m_sectors[y][x].p, 4))
      {
        continue;
      }

      for (int i = 0; i < dso->m_dsoSectors[y][x].count(); i++)
      {
        int index = dso->m_dsoSectors[y][x].at(i);
        dso_t *dsoItem = &dso->m_dso[index];

        if (1)//dsoItem->type == DSOT_GALAXY)
        {
          if (DSO_MAG(dsoItem->mag) <= transform->getMapParam()->m_maxStarMag)
          {
            float scale = dsoItem->sx / 3600. / (180 / M_PI) / transform->getMapParam()->m_fov;

            if (scale > minSize)
            {
              dsoTexts.append(index);
            }

            Vector3 vec;
            Transform::rdToVector(dsoItem->rd, vec);

            ptr->pos = vec.toQVector();
            ptr->params[0] = (dsoItem->pa == NO_DSO_PA) ? 0.0 : (SkMath::toRad(dsoItem->pa) + (transform->getMapParam()->m_flipX ? -R90 : 0)
                                                                                            + (transform->getMapParam()->m_flipY ? -R90 : 0));
            ptr->params[1] = dsoItem->sx;
            ptr->params[2] = dsoItem->sy;
            ptr->intVal = dsoItem->type;
            ptr++;
            count++;

            g_dataResource->getMapObject()->addDSO(index);
          }
        }
      }
    }
  }
  m_buffer.unmap();

  Q_ASSERT(count < MAX_DSO_ON_SCREEN);

  m_program.bind();
  m_program.setUniformValue("mvp_matrix", transform->getMVPMatrix());
  m_program.setUniformValue("v_matrix", transform->getViewMatrix());
  m_program.setUniformValue("p_matrix", transform->getProjectionMatrix());

  m_program.setUniformValue("u_modelview", transform->getViewMatrix());
  m_program.setUniformValue("u_projection", transform->getProjectionMatrix());

  m_program.setUniformValue("u_scr_width", 1.f / (float)transform->m_width);
  m_program.setUniformValue("u_scr_height", 1.f / (float)transform->m_height);

  m_program.setUniformValue("u_fov", (float)transform->getMapParam()->m_fov);
  m_program.setUniformValue("u_mag_limit", (float)transform->getMapParam()->m_maxStarMag);
  m_program.setUniformValue("pixel_ratio", (float)transform->m_ratio);

  m_program.setUniformValue("u_galaxy_color", QVector3D(1, 1, 0));
  m_program.setUniformValue("u_nebula_color_srm", QVector3D(1, 1, 1));
  m_program.setUniformValue("u_nebula_color_bright", QVector3D(1, 1, 1));
  m_program.setUniformValue("u_nebula_color_dark", QVector3D(0.5, 0.5, 0.5));
  m_program.setUniformValue("u_planetary_nebula_color", QVector3D(0.5, 0.5, 1));
  m_program.setUniformValue("u_globular_cluster_color", QVector3D(0.5, 1, 0.5));
  m_program.setUniformValue("u_open_cluster_color", QVector3D(1, 1, 0));
  m_program.setUniformValue("u_other_color", QVector3D(1, 0, 1));

  m_buffer.bind();
  int vertexLocation = m_program.attributeLocation("a_position");
  m_program.enableAttributeArray(vertexLocation);
  m_program.setAttributeBuffer(vertexLocation, GL_FLOAT, 0, 3, sizeof(dsoVertex_t));

  int vertexLocation1 = m_program.attributeLocation("a_params");
  m_program.enableAttributeArray(vertexLocation1);
  m_program.setAttributeBuffer(vertexLocation1, GL_FLOAT, offsetof(dsoVertex_t, params), 4, sizeof(dsoVertex_t));

  int vertexLocation2 = m_program.attributeLocation("a_int");
  m_program.enableAttributeArray(vertexLocation2);
  m_program.setAttributeBuffer(vertexLocation2, GL_FLOAT, offsetof(dsoVertex_t, intVal), 1, sizeof(dsoVertex_t));

  transform->getGl()->glDrawArrays(GL_POINTS, 0 ,count);

  m_buffer.release();
  m_program.release();

  g_debugWidget->addText("DSO rendered", QString::number(count));
  g_debugWidget->addText("DSO texts", QString::number(dsoTexts.count()));
  //qDebug() << count;

  dtr.begin(transform);

  float aspectRatio = transform->m_width / transform->m_height;

  foreach (int index, dsoTexts)
  {
    dso_t *dsoItem = &dso->m_dso[index];

    float bottomOffset = 1.1 * SkMath::toRad(dsoItem->sx / 3600.) / transform->m_fov;
    dtr.render(transform, dsoItem->rd, dso->getObjectName(index), bottomOffset * aspectRatio);
  }

  dtr.end(transform);
}

bool LayerDSO::checkShowDSO(Transform *transform, dso_t *dso)
{

}



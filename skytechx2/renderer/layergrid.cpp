#include "layergrid.h"

#include "transform.h"
#include "skmath.h"
#include "renderer.h"
#include "skdebug.h"
#include "plane.h"

#include "textrenderer.h"

TextRenderer rr;

LayerGrid::LayerGrid()
{

}

void LayerGrid::createResources()
{
  m_program.addShaderFromSourceCode(QOpenGLShader::Vertex, Renderer::readShader(":/res/opengl/grid_line_vshader.glsl"));
  m_program.addShaderFromSourceCode(QOpenGLShader::Fragment, Renderer::readShader(":/res/opengl/grid_line_fshader.glsl"));

  m_program.link();

  m_buffer.create();
  m_buffer.setUsagePattern(QOpenGLBuffer::DynamicDraw);
  m_buffer.bind();
  m_buffer.allocate(MAX_GRID_POINTS * sizeof(QVector3D));

}

void LayerGrid::render(Transform *transform)
{
  QMatrix4x4 mat;

  rr.createTexture("grid", 512, "arial", 10, QVector4D(1, 0, 0, 1) );

  glEnable(GL_BLEND);
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

  render(transform, mat, 0);

  glDisable(GL_BLEND);

  rr.createResources();
  rr.render(transform);
}

void LayerGrid::render(Transform *transform, QMatrix4x4 &matrix, int type)
{
  m_program.bind();

  m_program.setUniformValue("mvp_matrix", transform->getMVPMatrix());

  int count = generateGrid(transform, matrix, type);

  m_buffer.bind();
  m_buffer.write(0, m_vertices, count * sizeof(QVector3D));

  int vertexLocation = m_program.attributeLocation("a_position");
  m_program.enableAttributeArray(vertexLocation);
  m_program.setAttributeBuffer(vertexLocation, GL_FLOAT, 0, 3, sizeof(QVector3D));

  glDrawArrays(GL_LINES, 0, count);

  m_program.release();
  m_buffer.release();
}

int LayerGrid::generateGrid(Transform *transform, QMatrix4x4 &matrix, int type)
{
  int count = 0;
  RaDec rd[4];
  RaDec srd[4];
  Vector3 vec[4];

  double spcx;
  double spcy;
  double fov = transform->getMapParam()->m_fov;
  int stepx = 1;
  int stepy = 5;

  /*
  if (fov > SkMath::toRad(45)) {stepx = 1;
    else
  if (fov > SkMath::toRad(10)) stepx = 2;
    else
  if (fov > SkMath::toRad(5)) stepx = 4;
    else
  if (fov > SkMath::toRad(1)) stepx = 10;
    else
  if (fov > SkMath::toRad(4)) stepx = 20;
    else stepx = 40;
    */

  spcx = SkMath::toRad(10 / (double)stepx);
  spcy = SkMath::toRad(10 / (double)stepy);


  //qDebug() << SkMath::toDeg(spc);

  for (int y = -90; y < 90; y += 10)
  {
    for (int x = 0; x < 360; x += 10)
    {
      rd[0].ra = SkMath::toRad(x);
      rd[0].dec = SkMath::toRad(y);

      rd[1].ra = SkMath::toRad(x + 10);
      rd[1].dec = SkMath::toRad(y);

      rd[2].ra = rd[1].ra;
      rd[2].dec = SkMath::toRad(y + 10);

      rd[3].ra = SkMath::toRad(x);
      rd[3].dec = rd[2].dec;

      for (int i = 0; i < 4; i++)
      { // TODO: matrix
        transform->rdToVector(rd[i], vec[i]);
      }

      if (!SKPLANECheckFrustumToPolygon(transform->getFrustum(), vec, 4))
      {
        continue;
      }

      //double sy = 0;
      //double sx = 0;

      for (int sy = 0; sy < stepy; sy++)
      {
        for (int sx = 0; sx < stepx; sx++)
        {
          srd[0].ra = rd[0].ra + (sx * spcx);
          srd[0].dec = rd[0].dec + (sy * spcy);

          srd[1].ra = rd[0].ra + (sx * spcx + spcx);
          srd[1].dec = srd[0].dec;

          srd[2].ra = srd[0].ra;
          srd[2].dec = rd[0].dec + (sy * spcy + spcy);

          srd[3].ra = rd[0].ra + (sx * spcx);
          srd[3].dec = srd[2].dec;

          for (int i = 0; i < 4; i++)
          { // TODO: matrix
            transform->rdToVector(srd[i], vec[i]);
          }

          if (!SKPLANECheckFrustumToPolygon(transform->getFrustum(), vec, 4))
          {
            continue;
          }

          if (qAbs(srd[0].dec) < SkMath::toRad(80))
          {
            count = interpolateLine(matrix, 5, &srd[0], &srd[1], count, transform);

            rr.addText(QString::number(SkMath::toDeg(srd[0].dec)), srd[0], GL_TA_LEFT);
            rr.addText(QString::number(SkMath::toDeg(srd[1].dec)), srd[0], GL_TA_RIGHT);
            //rr.addText(QString::number(SkMath::toDeg(srd[0].ra)), srd[0], GL_TA_RIGHT);

            m_vertices[count++] = vec[0].toQVector();
            m_vertices[count++] = vec[3].toQVector();

            if (count >= MAX_GRID_POINTS)
            {
              return count;
            }
          }
        }
      }
    }
  }

  //rr.save();

  //qDebug() << count;

  return count;
}


int LayerGrid::interpolateLine(QMatrix4x4 &mat, int c, RaDec *rd1, RaDec *rd2, int count, Transform *transform)
{
  double dx = (rd2->ra - rd1->ra) / (double)(c - 1);
  RaDec rd = RaDec(rd1->ra, rd1->dec);

  Vector3 pt1;
  Vector3 pt2;

  transform->rdToVector(rd, pt1);//, mat);

  for (int i = 0; i < c - 1; i++)
  {
    rd.ra += dx;

    transform->rdToVector(rd, pt2);//, mat);

    m_vertices[count++] = pt1.toQVector();
    m_vertices[count++] = pt2.toQVector();

    pt1 = pt2;
  }

  return count;
}


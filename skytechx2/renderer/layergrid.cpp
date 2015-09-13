#include "layergrid.h"

#include "transform.h"
#include "skmath.h"
#include "renderer.h"
#include "skdebug.h"
#include "plane.h"

#include "textrenderer.h"

//TextRenderer rr;

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
  m_buffer.allocate(MAX_GRID_POINTS * sizeof(gridLine_t));

}

void LayerGrid::render(Transform *transform)
{
  QMatrix4x4 mat;

  //rr.createTexture("grid", 1024, "arial", 10, QVector4D(1, 0, 0, 1) );

  transform->getGl()->glEnable(GL_BLEND);
  transform->getGl()->glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

  render(transform, mat, 0);

  transform->getGl()->glDisable(GL_BLEND);

  //rr.createResources();
  //rr.render(transform);
}

void LayerGrid::render(Transform *transform, QMatrix4x4 &matrix, int type)
{
  m_program.bind();

  m_program.setUniformValue("mvp_matrix", transform->getMVPMatrix());

  int count = generateGrid(transform, matrix, type);

  m_buffer.bind();
  m_buffer.write(0, m_vertices, count * sizeof(gridLine_t));

  int vertexLocation = m_program.attributeLocation("a_position");
  m_program.enableAttributeArray(vertexLocation);
  m_program.setAttributeBuffer(vertexLocation, GL_FLOAT, 0, 3, sizeof(gridLine_t));

  int vertexLocation1 = m_program.attributeLocation("a_color");
  m_program.enableAttributeArray(vertexLocation1);
  m_program.setAttributeBuffer(vertexLocation1, GL_FLOAT, offsetof(gridLine_t, color), 4, sizeof(gridLine_t));

  transform->getGl()->glLineWidth(2);
  transform->getGl()->glDrawArrays(GL_LINES, 0, count);
  transform->getGl()->glLineWidth(1);

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
  int stepy = 1;

  if (fov > SkMath::toRad(45)) stepx = 1;
    else
  if (fov > SkMath::toRad(10)) stepx = 2;
    else
  if (fov > SkMath::toRad(5)) stepx = 4;
    else
  if (fov > SkMath::toRad(1)) stepx = 10;
    else
  if (fov > SkMath::toRad(4)) stepx = 20;
    else stepx = 40;

  stepy = stepx;

  if (qAbs(transform->getMapParam()->m_y) > SkMath::toRad(45))
    stepx /= 2;
  else
  if (qAbs(transform->getMapParam()->m_y) > SkMath::toRad(60))
    stepx /= 4;

  stepx = qBound(1, stepx, 40);
  stepy = qBound(1, stepy, 40);

  spcx = SkMath::toRad(10 / (double)stepx);
  spcy = SkMath::toRad(10 / (double)stepy);

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

          QVector4D  cc1 = QVector4D(1, 1, 1, 0.25);

          if (y == 0 && sy == 0)
          {
            cc1 = QVector4D(1, 0, 0, 1);
          }

          count = interpolateLine(matrix, 6, &srd[0], &srd[1], count, transform, cc1);

          //rr.addText(QString::number(SkMath::toDeg(srd[0].dec)), srd[0], GL_TA_LEFT);
          //rr.addText(QString::number(SkMath::toDeg(srd[0].ra)), srd[0], GL_TA_RIGHT);
          //rr.addText(QString::number(vec[0].x) + ", " +
          //           QString::number(vec[0].y) + ", " +
          //           QString::number(vec[0].z), srd[0], GL_TA_RIGHT);

          if ((y >= -80 && y < 80) ||
             ((srd[0].ra == 0) || (srd[0].ra == R90) || (srd[0].ra == R180) || (srd[0].ra == R270)))
          {
           QVector4D  cc1 = QVector4D(1, 1, 1, 0.25);

            m_vertices[count].pos = vec[0].toQVector();
            m_vertices[count++].color = cc1;

            m_vertices[count].pos = vec[3].toQVector();
            m_vertices[count++].color = cc1;
          }

          if (count >= MAX_GRID_POINTS)
          {
            qDebug() << "MAX_GRID_POINTS" << count;
            return count;
          }

        }
      }
    }
  }

  //qDebug() << count;

  return count;
}


int LayerGrid::interpolateLine(QMatrix4x4 &mat, int c, RaDec *rd1, RaDec *rd2, int count, Transform *transform, QVector4D &color)
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

    m_vertices[count].pos = pt1.toQVector();
    m_vertices[count++].color = color;

    m_vertices[count].pos = pt2.toQVector();
    m_vertices[count++].color = color;

    pt1 = pt2;
  }

  return count;
}


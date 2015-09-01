#include "renderer.h"
#include "layertychostars.h"
#include "layerconstellations.h"
#include "layergrid.h"
#include "transform.h"
#include "dataresources.h"
#include "gscregions.h"
#include "skfile.h"

#include <QDebug>

Renderer::Renderer()
{
}


QString Renderer::readTextFile(const QString &fileName)
{
  SkFile file(fileName);

  if (file.open(SkFile::ReadOnly | SkFile::Text))
  {
    QString code;

    code = file.readAll();

    return code;
  }
  else
  {
    qDebug() << "readTextFile failed" << fileName;
  }

  return QString();
}

QString Renderer::readShader(const QString &fileName)
{
  QString newCode;
  QString code = readTextFile(fileName);
  QStringList lines = code.split("\n");

  foreach (QString line, lines)
  {
    QString pragma = "#pragma include";
    int i = line.indexOf(pragma);

    if (i >= 0)
    {
      line = line.mid(pragma.size()).simplified();
      line = line.remove("\"");

      line = readTextFile(line);
    }

    newCode += line + "\n";
  }

  //qDebug() << newCode;

  return newCode;
}

void Renderer::createStaticResources()
{
  m_layerTychoStars = new LayerTychoStars();
  m_layerTychoStars->createReources();

  m_layerConstellations = new LayerConstellations();
  m_layerConstellations->createResources();

  m_layerGrid = new LayerGrid();
  m_layerGrid->createResources();
}

void Renderer::render(Transform *transform)
{
  g_dataResource->getGscRegions()->checkVisibleRegions(transform->getFrustum());

  m_layerGrid->render(transform);
  //m_layerConstellations->render(transform);
  //m_layerTychoStars->render(transform);
}


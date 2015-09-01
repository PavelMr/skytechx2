#include "text2drenderer.h"

#include <QDebug>

static int instanceCount = 0;

Text2DRenderer::Text2DRenderer()
{
  if (++instanceCount > 1)
  {
    qDebug() << "Text2DRenderer() too many instances!!!!!!!";
  }
}

void Text2DRenderer::createResources()
{

}


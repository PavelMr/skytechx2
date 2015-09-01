#ifndef RENDERER_H
#define RENDERER_H

#include <QString>

class LayerTychoStars;
class LayerConstellations;
class LayerGrid;
class Transform;

class Renderer
{
public:
  Renderer();
  void createStaticResources();
  void render(Transform *transform);

  static QString readShader(const QString &fileName);
private:
  LayerTychoStars *m_layerTychoStars;
  LayerConstellations *m_layerConstellations;
  LayerGrid *m_layerGrid;

  static QString readTextFile(const QString &fileName);
};

#endif // RENDERER_H

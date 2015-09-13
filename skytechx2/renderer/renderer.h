#ifndef RENDERER_H
#define RENDERER_H

#include <QString>

class LayerTychoStars;
class LayerConstellations;
class LayerGSCStars;
class LayerUCAC4Stars;
class LayerBackground;

class StarShader;

class LayerGrid;
class Transform;

class Renderer
{
public:
  Renderer();
  void createStaticResources();
  void render(Transform *transform);

  StarShader *getStarShaderProgram();

  static QString readShader(const QString &fileName);
private:
  LayerTychoStars *m_layerTychoStars;
  LayerGSCStars *m_layerGSCStars;
  LayerUCAC4Stars *m_layerUCAC4Stars;

  LayerConstellations *m_layerConstellations;
  LayerGrid *m_layerGrid;

  LayerBackground *m_background;

  StarShader *m_starShader;

  static QString readTextFile(const QString &fileName);
};

#endif // RENDERER_H

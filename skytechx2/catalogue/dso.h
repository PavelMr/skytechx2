#ifndef DSO_H
#define DSO_H

#include "dso_def.h"
#include "vecmath.h"

#include <QMap>
#include <QVector>
#include <QScriptContext>
#include <QScriptEngine>

#define   DSO_SEG_SIZE     7.5
#define   NUM_DSO_SEG_Y    24
#define   NUM_DSO_SEG_X    48

typedef struct
{
  Vector3 p[4];
} dsoRect_t;

class DSO : public QObject
{
  Q_OBJECT
public:
  DSO();
  void load();

  QString getName(dso_t *dso, int index = 0);
  QString getObjectName(int dsoIndex);

private:
  static bool dsoCompare(dso_t &a, dso_t &b);
  void assignToSectors();
  void createSectors();
  void createNames(QByteArray *names);
  void loadCommonNames();
  QStringList getName(dso_t *dso, QByteArray *names);
  bool addCustomFile(const QString &fileName);
  double parseRA(const QString str);
  double parseDec(const QString str);
  double parseSize(const QString str);
  bool findObject(const QString &str, int &index);

  QString m_lastCustomError;

public:
  QHash <int, QStringList> m_namesMap;        // index to name
  QMap <QString, int>      m_nameMapReverse;  // name to index

  QVector <dso_t> m_dso;
  QVector <int>   m_dsoSectors[NUM_DSO_SEG_Y][NUM_DSO_SEG_X];
  dsoRect_t       m_sectors[NUM_DSO_SEG_Y][NUM_DSO_SEG_X];

  QByteArray          m_names;
  QByteArray          m_classes;
  QByteArray          m_catNnames;
  QMap <int, QString> m_commonNamesMap;

  int  m_maxObjectInRegion;
};

#endif // DSO_H

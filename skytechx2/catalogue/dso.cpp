#include "dso.h"
#include "skfile.h"

#include <QtGlobal>
#include <QDebug>
#include <QScriptEngine>
#include <QScriptValueIterator>
#include <QTime>

typedef struct
{
  QString desc;
  QString name;
  int     guiType;
} scriptType_t;

QList <scriptType_t> scriptNewTypes;


DSO::DSO()
{
}

void DSO::load()
{
  dsoHeader_t  dsoHead;
  SkFile pf("resources/dso/dso.dat");

  if (!pf.open(SkFile::ReadOnly))
  {
    return;
  }

  pf.read((char *)&dsoHead.id, 4);
  pf.read((char *)&dsoHead.numDso, sizeof(qint32));
  pf.read((char *)&dsoHead.textSegSize, sizeof(qint32));
  pf.read((char *)&dsoHead.galClassSize, sizeof(qint32));
  pf.read((char *)&dsoHead.catNamesSize, sizeof(qint32));

  m_names = pf.read(dsoHead.textSegSize);
  m_classes = pf.read(dsoHead.galClassSize);
  m_catNnames = pf.read(dsoHead.catNamesSize);

  for (qint32 i = 0; i < dsoHead.numDso; i++)
  {
    dso_t dso;

    pf.read((char *)&dso.nameOffs, sizeof(qint32));
    pf.read((char *)&dso.rd.ra, sizeof(double));
    pf.read((char *)&dso.rd.dec, sizeof(double));
    pf.read((char *)&dso.mag, sizeof(signed short));
    pf.read((char *)&dso.pa, sizeof(unsigned short));
    pf.read((char *)&dso.sx, sizeof(quint32));
    pf.read((char *)&dso.sy, sizeof(quint32));
    pf.read((char *)&dso.type, sizeof(unsigned char));
    pf.read((char *)&dso.cataloque, sizeof(unsigned char));
    pf.read((char *)&dso.shape, sizeof(unsigned short));
    pf.read((char *)&dso.galType, sizeof(unsigned short));

    SkMath::range(dso.rd.ra, R360);
    if (dso.sx > 0 && dso.sy == 0)
    {
      dso.sy = dso.sx;
    }

    m_dso.append(dso);
  }

  qDebug() << "dso" << dsoHead.numDso;

  pf.close();

  /*
  if (!addCustomFile("data/custom.dst"))
  {
    qDebug() << m_lastCustomError;
  }
  */

  // sort dso by size
  qSort(m_dso.begin(), m_dso.end(), dsoCompare);

  assignToSectors();
  createSectors();
  createNames(&m_names);
  loadCommonNames();
}


bool DSO::dsoCompare(dso_t &a, dso_t &b)
{
  return a.sx > b.sx;
}

void DSO::assignToSectors()
{
  for (qint32 i = 0; i < m_dso.count(); i++)
  {
    int ra  = (int)((SkMath::toDeg(m_dso[i].rd.ra)) / DSO_SEG_SIZE);
    int dec = (int)((SkMath::toDeg(m_dso[i].rd.dec) + 90.) / DSO_SEG_SIZE);
    m_dsoSectors[dec][ra].append(i);
  }

  m_maxObjectInRegion = 0;

  for (int y = 0; y < NUM_DSO_SEG_Y; y++)
  {
    for (int x = 0; x < NUM_DSO_SEG_X; x++)
    {
      if (m_dsoSectors[y][x].count() > m_maxObjectInRegion)
      {
        m_maxObjectInRegion = m_dsoSectors[y][x].count();
      }
    }
  }
  qDebug() << "m_maxObjectInRegion" << m_maxObjectInRegion;
}

void DSO::createSectors()
{
  double  yy;
  double  xx = 0;
  Vector3 p;
  RaDec  rd;

  yy = -11;
  for (int y = 0; y < NUM_DSO_SEG_Y; y++, yy++)
  {
    for (int x = 0; x < NUM_DSO_SEG_X; x++, xx++)
    {
      rd.ra = SkMath::toRad(xx * DSO_SEG_SIZE);
      rd.dec = SkMath::toRad(yy * DSO_SEG_SIZE);
      Transform::rdToVector(rd, p);

      m_sectors[y][x].p[0].x = p.x;
      m_sectors[y][x].p[0].y = p.y;
      m_sectors[y][x].p[0].z = p.z;

      rd.ra = SkMath::toRad((xx + 1) * DSO_SEG_SIZE);
      Transform::rdToVector(rd, p);

      m_sectors[y][x].p[1].x = p.x;
      m_sectors[y][x].p[1].y = p.y;
      m_sectors[y][x].p[1].z = p.z;

      rd.ra = SkMath::toRad((xx + 1) * DSO_SEG_SIZE);
      rd.dec = SkMath::toRad((yy - 1) * DSO_SEG_SIZE);
      Transform::rdToVector(rd, p);

      m_sectors[y][x].p[2].x = p.x;
      m_sectors[y][x].p[2].y = p.y;
      m_sectors[y][x].p[2].z = p.z;

      rd.ra = SkMath::toRad(xx * DSO_SEG_SIZE);
      rd.dec = SkMath::toRad((yy - 1) * DSO_SEG_SIZE);
      Transform::rdToVector(rd, p);

      m_sectors[y][x].p[3].x = p.x;
      m_sectors[y][x].p[3].y = p.y;
      m_sectors[y][x].p[3].z = p.z;
    }
  }
}

void DSO::createNames(QByteArray *names)
{
  for (qint32 i = 0; i < m_dso.count(); i++)
  {
    m_namesMap[m_dso[i].nameOffs].append(getName(&m_dso[i], names));
    Q_ASSERT(m_namesMap[m_dso[i].nameOffs].size() > 0);
  }

  for (qint32 i = 0; i < m_dso.count(); i++)
  {
    int j = 0;
    do
    {
      QString name = getName(&m_dso[i], j++);
      if (!name.isEmpty())
      {
        m_nameMapReverse[name.toLower().simplified()] = i;
      }
      else
      {
        break;
      }
    } while (true);
  }
}

void DSO::loadCommonNames()
{
  QTime t;

  t.start();

  SkFile f("resources/dso/names.dat");
  if (f.open(SkFile::ReadOnly | SkFile::Text))
  {
    QString str;
    QTextStream s(&f);

    while (true)
    {
      str = s.readLine();
      if (str.isNull())
      {
        break;
      }

      QStringList list = str.split("|");

      if (list.count() == 2)
      {
        int index;
        if (findObject(list.at(0).simplified(), index))
        {
          m_commonNamesMap[index] = list.at(1).simplified();
        }
        else
        {
          qDebug() << "not found" << list.at(0).simplified();
        }
      }
    }
  }

  qDebug() << t.elapsed() / 1000.0 << "sec";
}

QStringList DSO::getName(dso_t *dso, QByteArray *names)
{
  const char *p = names->constData() + dso->nameOffs;
  QString str = QString(p);

  str = str.remove(" ");

  return str.split("\t");
}

QString DSO::getName(dso_t *dso, int index)
{
  if (index >= m_namesMap[dso->nameOffs].count())
  {
    return "";
  }
  return m_namesMap[dso->nameOffs].at(index);
}

QString DSO::getObjectName(int dsoIndex)
{
  if (m_commonNamesMap.contains(dsoIndex))
  {
    return m_commonNamesMap[dsoIndex];
  }

  return getName(&m_dso[dsoIndex]);
}

static QScriptValue DEFINE_CUSTOM_OBJECT(QScriptContext *context, QScriptEngine *engine)
{
   QScriptValue a = context->argument(0);
   QScriptValue b = context->argument(1);
   QScriptValue c = context->argument(2);

   scriptType_t v;

   v.desc = a.toString();
   v.name = b.toString();
   v.guiType = c.toInt32();

   scriptNewTypes.append(v);

   return QScriptValue();
}

bool DSO::addCustomFile(const QString &fileName)
{
  m_lastCustomError = "";

  SkFile file(fileName);

  if (!file.open(SkFile::ReadOnly | SkFile::Text))
  {
    m_lastCustomError = tr("File not found!");
    return false;
  }

  QString code;

  do
  {
    if (file.atEnd())
    {
      break;
    }
    QString line = file.readLine();

    if (line.simplified() == "*DATA")
    {
      break;
    }

    code += line;
  } while (true);

  QScriptEngine engine;

  scriptNewTypes.clear();

  engine.globalObject().setProperty("AUTHOR", "");
  engine.globalObject().setProperty("EPOCH", 2000);
  engine.globalObject().setProperty("DESCRIPTION", "");

  engine.globalObject().setProperty("POINT_SOURCE", 0);
  engine.globalObject().setProperty("CIRCLE", 1);
  engine.globalObject().setProperty("CROSS", 2);
  engine.globalObject().setProperty("RECTANGLE", 3);

  QScriptValue fun = engine.newFunction(DEFINE_CUSTOM_OBJECT, 3);
  engine.globalObject().setProperty("DEFINE_CUSTOM_OBJECT", fun);

  QScriptValue result = engine.evaluate(code);
  if (engine.hasUncaughtException())
  {
    int line = engine.uncaughtExceptionLineNumber();
    m_lastCustomError = QString("uncaught exception at line ") + line + " : " + result.toString();
    return false;
  }

  QString author;
  QString description;
  double epoch;

  int nameOffset = 0; // TODO: dat globalne

  QScriptValueIterator it(engine.globalObject());
  while (it.hasNext())
  {
    it.next();

    if (it.name() == "AUTHOR")
    {
      author = it.value().toString();
    }
    else
    if (it.name() == "DESCRIPTION")
    {
      description = it.value().toString();
    }
    else
    if (it.name() == "EPOCH")
    {
      epoch = it.value().toNumber();
    }
  }

  do
  {
    if (file.atEnd())
    {
      break;
    }
    QString line = file.readLine().simplified();

    if (line.startsWith("//"))
    {
      continue;
    }

    if (line.size() == 0)
    {
      continue;
    }

    QStringList list = line.split("|");

    //qDebug() << line << list.count();

    if (list.count() != 10)
    {
      m_lastCustomError = "Invalid record";
      return false;
    }

    QString type = list[0].simplified();
    QString name1 = list[1].simplified();
    QString name2 = list[2].simplified();
    double ra = parseRA(list[3]);
    double dec = parseDec(list[4]);
    double sizeA = parseSize(list[5]);
    double sizeB = parseSize(list[6]);
    double mag = list[7].simplified().toDouble();
    double pa = list[8].simplified().toDouble();
    QString cls = list[9].simplified();

    dso_t dso;

    if (qIsNaN(ra) || qIsNaN(dec))
    {
      m_lastCustomError = "Invalid RA/Dec value";
      return false;
    }

    m_namesMap[USER_OFFSET + nameOffset].append(name1);
    m_namesMap[USER_OFFSET + nameOffset].append(name2);
    nameOffset++;

    dso.cataloque = 0; // TODO: dodelat
    dso.galType = 0; // TODO: dodelat
    dso.mag = SkMath::isNear(mag, 0) ?  NO_DSO_MAG : (short)(mag * 100.);
    dso.nameOffs = USER_OFFSET + nameOffset;
    dso.pa = pa; //TODO: bez PA
    dso.rd.ra = SkMath::toRad(ra);
    dso.rd.dec = SkMath::toRad(dec); //TODO:epoch
    dso.shape = NO_DSO_SHAPE;
    dso.sx = sizeA;
    dso.sy = sizeB;
    dso.type = DSOT_CUSTOM_FLAG; // TODO: user types

    qDebug() << line;
    qDebug() << type << name1 << name2 << ra << dec << sizeA << sizeB << mag << pa << cls;
  } while (true);

  return true;
}

double DSO::parseRA(const QString str)
{
  // eg. 02h12m33.4s

  int indexH = str.indexOf("h");
  int indexM = str.indexOf("m");
  int indexS = str.indexOf("s");

  if (indexH == -1 || indexM == -1 || indexS == -1)
  {
    return qQNaN();
  }

  QString h, m, s;

  h = str.mid(0, indexH).simplified();
  m = str.mid(indexH + 1, 2).simplified();
  s = str.mid(indexM + 1).simplified();
  s.chop(1);

  double hd = h.toDouble();
  double md = m.toDouble();
  double sd = s.toDouble();

  if ((hd < 0 || hd >= 24) ||
      (md < 0 || md >= 60) ||
      (sd < 0 || sd >= 60))
  {
    return qQNaN();
  }

  return 15 * (hd + (md / 60.0) + (sd / 3600.0));
}

double DSO::parseDec(const QString str)
{
  // eg. -22d12m33.4s

  int indexD = str.indexOf("d");
  int indexM = str.indexOf("m");
  int indexS = str.indexOf("s");

  if (indexD == -1 || indexM == -1 || indexS == -1)
  {
    return qQNaN();
  }

  QString d, m, s;

  d = str.mid(0, indexD).simplified();
  m = str.mid(indexD + 1, 2).simplified();
  s = str.mid(indexM + 1).simplified();
  s.chop(1);

  double dd = d.toDouble();
  double md = m.toDouble();
  double sd = s.toDouble();

  if ((dd < -90 || dd > 90) ||
      (md < 0 || md >= 60) ||
      (sd < 0 || sd >= 60))
  {
    return qQNaN();
  }

  double sign = dd < 0 ? -1 : 1;
  return sign * (qAbs(dd) + (md / 60.0) + (sd / 3600.0));
}

double DSO::parseSize(const QString str)
{
  QString tmp = str.simplified();
  double mul = 1;

  if (tmp.endsWith("\""))
  {
    mul = 1;
    tmp.chop(1);
  }
  else
  if (tmp.endsWith("'"))
  {
    mul = 60;
    tmp.chop(1);
  }

  return tmp.toDouble() * mul;
}

bool DSO::findObject(const QString &str, int &index)
{
  QString findStr = str;
  QString name;

  findStr = findStr.replace(" ", "").toLower();

  if (m_nameMapReverse.contains(findStr))
  {
    index = m_nameMapReverse[findStr];
    //qDebug() << index;
    return true;
  }

  return false;

  for (int i = 0; i < m_dso.count(); i++)
  {
    qint32 nameOffset = m_dso[i].nameOffs;
    char *ptr = m_names.data() + nameOffset;

    do
    {
      char ch = *ptr;
      if (ch == '\0' || ch == '\t')
      {
        if (0 == findStr.compare(name, Qt::CaseInsensitive))
        {
          index = i;
          return true;
        }
        name.clear();
        if (ch == '\0')
        {
          break;
        }
      }
      else
      {
        name += ch;
      }
      ptr++;
    } while (true);
  }

  return false;
}





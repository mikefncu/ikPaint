
// REFACT0R: Remote open/save file logic is duplicated in kpDocument.
// HITODO: Test when remote file support in KDE 4 stabilizes

/* This file is part of the KDE libraries
    Copyright (C) 1999 Waldo Bastian (bastian@kde.org)
    Copyright (C) 2007 Clarence Dang (dang@kde.org)
    Modified by Maikel Diaz <ariguanabosoft@gmail.com>
    Copyright (c) 2015-2018

    This library is free software; you can redistribute it and/or
    modify it under the terms of the GNU Library General Public
    License as published by the Free Software Foundation; either
    version 2 of the License.

    This library is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
    Library General Public License for more details.

    You should have received a copy of the GNU Library General Public License
    along with this library; see the file COPYING.LIB.  If not, write to
    the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
    Boston, MA 02110-1301, USA.
*/
//-----------------------------------------------------------------------------
// KDE color collection

#define DEBUG_KP_COLOR_COLLECTION 0

#include "kpColorCollection.h"

#include <QtCore/QFile>

#include <qglobal.h>

#include <qlocale.h>
#include <qmessagebox.h>

#include <qfile.h>
#include <qdebug.h>

#include <qstring.h>
#include <qsettings.h>
#include <qdatastream.h>
#include <tools.h>

#include <kpDefs.h>
#include <kpUrlFormatter.h>

struct ColorNode
{
    ColorNode(const QColor &c, const QString &n)
        : color(c), name(n) {}

    QColor color;
    QString name;
};


//BEGIN kpColorCollectionPrivate
class kpColorCollectionPrivate
{
public:
    kpColorCollectionPrivate();
    kpColorCollectionPrivate(const kpColorCollectionPrivate&);
    ~kpColorCollectionPrivate() {}

    QList<ColorNode> colorList;
    QString name;
    QString desc;
    kpColorCollection::Editable editable;
};

kpColorCollectionPrivate::kpColorCollectionPrivate()
{
}

kpColorCollectionPrivate::kpColorCollectionPrivate(const kpColorCollectionPrivate& p)
    : colorList(p.colorList), name(p.name), desc(p.desc), editable(p.editable)
{
}
//END kpColorCollectionPrivate


kpColorCollection::kpColorCollection()
{
  d = new kpColorCollectionPrivate();
}

kpColorCollection::kpColorCollection(const kpColorCollection &p)
{
    d = new kpColorCollectionPrivate(*p.d);
}

kpColorCollection::~kpColorCollection()
{
  // Need auto-save?
    delete d;
}

// TODO: Set d->editable?
bool kpColorCollection::open() {

    QSettings cfg;
	cfg.beginGroup(kpSettingPaletteGroup);

    if (!cfg.contains(kpSettingPaletteColors)) return false;

    QByteArray data = cfg.value(kpSettingPaletteColors).toByteArray();
    cfg.endGroup();

    QDataStream str (data);

    bool ok;
    QColor c;

    QList <ColorNode> newColorList;
    while (!str.atEnd()) {
        QString name;
        QString r, g, b;
        //int r, g, b;

        str >> name;
        str >> r;
        str >> g;
        str >> b;

        c = QColor(r.toInt(), g.toInt(), b.toInt(&ok));

        if (!ok || !c.isValid()) return false;

        newColorList.append(ColorNode(c, name));
    }

    d->colorList = newColorList;
    d->name.clear ();
    d->desc = QString();

    return true;
}

bool kpColorCollection::save() const {

	QSettings cfg;
	cfg.beginGroup(kpSettingPaletteGroup);

	QByteArray data;

    QDataStream str (&data, QIODevice::ReadWrite);

	foreach (const ColorNode &node, d->colorList)
	{
		// Added for KolourPaint.
		if(!node.color.isValid ())
			continue;

		int r,g,b;
		node.color.getRgb(&r, &g, &b);
        str << node.name << QString::number(r) << QString::number(g) << QString::number(b);
    }

	cfg.setValue(kpSettingPaletteColors, data);
	cfg.endGroup();

	return true;
}

QString kpColorCollection::description() const
{
    return d->desc;
}

void kpColorCollection::setDescription(const QString &desc)
{
    d->desc = desc;
}

QString kpColorCollection::name() const
{
    return d->name;
}

void kpColorCollection::setName(const QString &name)
{
    d->name = name;
}

kpColorCollection::Editable kpColorCollection::editable() const
{
    return d->editable;
}

void kpColorCollection::setEditable(Editable editable)
{
    d->editable = editable;
}

int kpColorCollection::count() const
{
    return (int) d->colorList.count();
}

void kpColorCollection::resize(int newCount)
{
    if (newCount == count())
        return;
    else if (newCount < count())
    {
        d->colorList.erase(d->colorList.begin() + newCount, d->colorList.end());
    }
    else if (newCount > count())
    {
         while(newCount > count())
         {
             const int ret = addColor(QColor(), QString()/*color name*/);
             Q_ASSERT(ret == count() - 1);
         }
    }
}

kpColorCollection&
kpColorCollection::operator=( const kpColorCollection &p)
{
  if (&p == this) return *this;
    d->colorList = p.d->colorList;
    d->name = p.d->name;
    d->desc = p.d->desc;
    d->editable = p.d->editable;
  return *this;
}

QColor
kpColorCollection::color(int index) const
{
    if ((index < 0) || (index >= count()))
	return QColor();

    return d->colorList[index].color;
}

int
kpColorCollection::findColor(const QColor &color) const
{
    for (int i = 0; i < d->colorList.size(); ++i)
  {
        if (d->colorList[i].color == color)
        return i;
  }
  return -1;
}

QString
kpColorCollection::name(int index) const
{
  if ((index < 0) || (index >= count()))
	return QString();

  return d->colorList[index].name;
}

QString kpColorCollection::name(const QColor &color) const
{
    return name(findColor(color));
}

int
kpColorCollection::addColor(const QColor &newColor, const QString &newColorName)
{
    d->colorList.append(ColorNode(newColor, newColorName));
    return count() - 1;
}

int
kpColorCollection::changeColor(int index,
                      const QColor &newColor,
                      const QString &newColorName)
{
    if ((index < 0) || (index >= count()))
	return -1;

  ColorNode& node = d->colorList[index];
  node.color = newColor;
  node.name  = newColorName;

  return index;
}

int kpColorCollection::changeColor(const QColor &oldColor,
                          const QColor &newColor,
                          const QString &newColorName)
{
    return changeColor( findColor(oldColor), newColor, newColorName);
}


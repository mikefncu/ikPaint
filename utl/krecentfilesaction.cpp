/* This file is part of the KDE libraries
    Copyright (C) 1999 Reginald Stadlbauer <reggie@kde.org>
              (C) 1999 Simon Hausmann <hausmann@kde.org>
              (C) 2000 Nicolas Hadacek <haadcek@kde.org>
              (C) 2000 Kurt Granroth <granroth@kde.org>
              (C) 2000 Michael Koch <koch@kde.org>
              (C) 2001 Holger Freyther <freyther@kde.org>
              (C) 2002 Ellis Whitehead <ellis@kde.org>
              (C) 2002 Joseph Wenninger <jowenn@kde.org>
              (C) 2003 Andras Mantia <amantia@kde.org>
              (C) 2005-2006 Hamish Rodda <rodda@kde.org>

    Modified by Maikel Diaz <ariguanabosoft@gmail.com>
    Copyright (c) 2015-2018

    This library is free software; you can redistribute it and/or
    modify it under the terms of the GNU Library General Public
    License version 2 as published by the Free Software Foundation.

    This library is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
    Library General Public License for more details.

    You should have received a copy of the GNU Library General Public License
    along with this library; see the file COPYING.LIB.  If not, write to
    the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
    Boston, MA 02110-1301, USA.
*/

#include "krecentfilesaction.h"

#include <QtCore/QFile>
#ifdef Q_OS_WIN
#include <QtCore/QDir>
#endif

#include <qsettings.h>
#include <qdebug.h>
#include <qicon.h>
#include <qlocale.h>
#include <tools.h>
#if QT_VERSION >= 0x050000
#include <qstandardpaths.h>
#else
#include <qstandarddirs.h>
#endif

#include "qmenu.h"


KRecentFilesAction::KRecentFilesAction(QObject *parent)
  : KSelectAction(*new KRecentFilesActionPrivate, parent)
{
  Q_D(KRecentFilesAction);
  d->init();
}

KRecentFilesAction::KRecentFilesAction(const QString &text, QObject *parent)
  : KSelectAction(*new KRecentFilesActionPrivate, parent)
{
  Q_D(KRecentFilesAction);
  d->init();

  // Want to keep the ampersands
  setText(text);
}

KRecentFilesAction::KRecentFilesAction(const QIcon &icon, const QString &text, QObject *parent)
  : KSelectAction(*new KRecentFilesActionPrivate, parent)
{
  Q_D(KRecentFilesAction);
  d->init();

  setIcon(icon);
  // Want to keep the ampersands
  setText(text);
}

void KRecentFilesActionPrivate::init()
{
  Q_Q(KRecentFilesAction);
  delete q->menu();
  q->setMenu(new QMenu());
  q->setToolBarMode(KSelectAction::MenuMode);
  m_noEntriesAction=new QAction(i18n("No entries"),q->selectableActionGroup());
  m_noEntriesAction->setEnabled(false);
  q->KSelectAction::addAction(m_noEntriesAction);
  q->connect(q, SIGNAL(triggered(QAction*)), SLOT(_k_urlSelected(QAction*)));
}

KRecentFilesAction::~KRecentFilesAction()
{
}

void KRecentFilesActionPrivate::_k_urlSelected( QAction* action )
{
    Q_Q(KRecentFilesAction);
    emit q->urlSelected(m_urls[action]);
}

int KRecentFilesAction::maxItems() const
{
    Q_D(const KRecentFilesAction);
    return d->m_maxItems;
}

void KRecentFilesAction::setMaxItems( int maxItems )
{
    Q_D(KRecentFilesAction);
    // set new maxItems
    d->m_maxItems = maxItems;

    // remove all excess items
    while( selectableActionGroup()->actions().count() > maxItems )
        delete removeAction(selectableActionGroup()->actions().last());
}

void KRecentFilesAction::addUrl(const QString &_url, const QString& name )
{
    Q_D(KRecentFilesAction);
    /**
     * Create a deep copy here, because if _url is the parameter from
     * urlSelected() signal, we will delete it in the removeAction() call below.
     * but access it again in the addAction call... => crash
     */
    QString url = _url;

    // QDir::fromNativeSeparators
    QString tmpName = name;
    if (name.isEmpty()) {
        tmpName = url.mid(url.lastIndexOf("/") + 1);
    }

    const QString file = url.left(url.lastIndexOf("/"));

    // remove file if already in list
    foreach (QAction* action, selectableActionGroup()->actions())
    { 
      const QString & path = d->m_urls[action];
      if ( path.endsWith(url) )
      {
        removeAction(action)->deleteLater();
        break;
      }
    }

    // remove oldest item if already maxitems in list
    if( d->m_maxItems && selectableActionGroup()->actions().count() == d->m_maxItems )
    {
        // remove oldest added item
        delete removeAction(selectableActionGroup()->actions().first());
    }

    if (d->m_noEntriesAction) removeAction(d->m_noEntriesAction)->deleteLater();
    // add file to list
    const QString title = tmpName + " [" + file + ']';
    QAction* action = new QAction(title, selectableActionGroup());
    addAction(action, url, tmpName);
}

void KRecentFilesAction::addAction(QAction* action, const QString &url, const QString& name)
{
  Q_D(KRecentFilesAction);
  //kDebug (129) << "KRecentFilesAction::addAction(" << action << ")";

  action->setActionGroup(selectableActionGroup());

  // Keep in sync with createToolBarWidget()
  foreach (QToolButton* button, d->m_buttons)
    button->insertAction(button->actions().value(0), action);

  foreach (QComboBox* comboBox, d->m_comboBoxes)
    comboBox->insertAction(comboBox->actions().value(0), action);

  menu()->insertAction(menu()->actions().value(0), action);

  d->m_shortNames.insert( action, name );
  d->m_urls.insert( action, url );
}

QAction* KRecentFilesAction::removeAction(QAction* action)
{
  Q_D(KRecentFilesAction);
  KSelectAction::removeAction( action );

  d->m_shortNames.remove( action );
  d->m_urls.remove( action );

  return action;
}

void KRecentFilesAction::removeUrl(const QString &url )
{
  Q_D(KRecentFilesAction);
  for (QMap<QAction*, QString>::ConstIterator it = d->m_urls.constBegin(); it != d->m_urls.constEnd(); ++it)
    if (it.value() == url) {
      delete removeAction(it.key());
      return;
    }
}

QList<QString> KRecentFilesAction::urls() const
{
  Q_D(const KRecentFilesAction);
  return d->m_urls.values ();
}

void KRecentFilesAction::clear()
{
    Q_D(KRecentFilesAction);
    KSelectAction::clear();
    d->m_shortNames.clear();
    d->m_urls.clear();
    if (d->m_noEntriesAction) KSelectAction::removeAction(d->m_noEntriesAction)->deleteLater();
    d->m_noEntriesAction=new QAction(i18n("No entries"),selectableActionGroup());
    d->m_noEntriesAction->setEnabled(false);
    KSelectAction::addAction(d->m_noEntriesAction);
}

void KRecentFilesAction::loadEntries(const QSettings &config)
{
    Q_D(KRecentFilesAction);
    clear();

    QString     key;
    QString     value;
    QString     nameKey;
    QString     nameValue;
    QString     title;
    QString     url;

    bool thereAreEntries=false;
    // read file list
    for( int i = 1 ; i <= d->m_maxItems ; i++ )
    {
        key = QString( "File%1" ).arg( i );
        value = config.value( key, QString() ).toString();
        if (value.isEmpty()) continue;
        url = value;

        // Don't restore if file doesn't exist anymore
        if (!QFile::exists(url))
          continue;

        // Don't restore where the url is already known (eg. broken config)
        if (d->m_urls.values().contains(url))
          continue;

        nameKey = QString( "Name%1" ).arg( i );
        nameValue = config.value( nameKey, url.mid(url.lastIndexOf("/") + 1) ).toString();
        title = nameValue + " [" + url.left(url.lastIndexOf("/")) + ']';
        if (!value.isNull())
        {
          thereAreEntries=true;
          addAction(new QAction(title, selectableActionGroup()), url, nameValue);
        }
    }
    if (thereAreEntries)
    {
        if (d->m_noEntriesAction) KSelectAction::removeAction(d->m_noEntriesAction)->deleteLater();
    }
}

void KRecentFilesAction::saveEntries(QSettings &config )
{
    Q_D(KRecentFilesAction);
    QString     key;
    QString     value;

    //cg.deleteGroup();
    QStringList lst = config.childKeys();
    foreach (QString key, lst) {
        config.remove(key);
    }

    // write file list
    if ( (selectableActionGroup()->actions().count() > 1) ||
    ( (selectableActionGroup()->actions().count() == 1) && (selectableActionGroup()->actions()[0] != d->m_noEntriesAction) ) )
    for ( int i = 1 ; i <= selectableActionGroup()->actions().count() ; i++ )
    {
        key = QString( "File%1" ).arg( i );
        // i - 1 because we started from 1
        const QString &path = d->m_urls[ selectableActionGroup()->actions()[ i - 1 ] ];
        value = path; //.left(path.indexOf("/", -1));
        config.setValue( key, value );
        key = QString( "Name%1" ).arg( i );
        value = d->m_shortNames[ selectableActionGroup()->actions()[ i - 1 ] ];
        config.setValue( key, value );
    }

}

/* This file was modified from the original file: kfontsizeaction.cpp
 * from KDE project:

   Modified by Maikel Diaz <ariguanabosoft@gmail.com>
   Copyright (c) 2015-2018

   All rights reserved.

   Redistribution and use in source and binary forms, with or without
   modification, are permitted provided that the following conditions
   are met:

   1. Redistributions of source code must retain the above copyright
      notice, this list of conditions and the following disclaimer.
   2. Redistributions in binary form must reproduce the above copyright
      notice, this list of conditions and the following disclaimer in the
      documentation and/or other materials provided with the distribution.

   THIS SOFTWARE IS PROVIDED BY THE AUTHOR ``AS IS'' AND ANY EXPRESS OR
   IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES
   OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
   IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY DIRECT, INDIRECT,
   INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT
   NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
   DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
   THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
   (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF
   THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/

#include "kfontsizeaction.h"

#include <QtGui/QFontDatabase>
#include <QToolBar>
#include <QToolButton>

#include <qdebug.h>
#include <qicon.h>
#include <qlocale.h>

#include "qmenu.h"

class KFontSizeAction::Private
{
    public:
        Private(KFontSizeAction *parent)
            : q(parent)
        {
        }

        void init();

        KFontSizeAction *q;
};

// BEGIN KFontSizeAction
KFontSizeAction::KFontSizeAction(QObject *parent)
  : KSelectAction(parent),
    d(new Private(this))
{
  d->init();
}

KFontSizeAction::KFontSizeAction(const QString &text, QObject *parent)
  : KSelectAction(text, parent),
    d(new Private(this))
{
  d->init();
}

KFontSizeAction::KFontSizeAction(const QIcon &icon, const QString &text, QObject *parent)
  : KSelectAction(icon, text, parent),
    d(new Private(this))
{
  d->init();
}

KFontSizeAction::~KFontSizeAction()
{
  delete d;
}

void KFontSizeAction::Private::init()
{
    q->setEditable( true );
    QFontDatabase fontDB;
    const QList<int> sizes = fontDB.standardSizes();
    QStringList lst;
    for ( QList<int>::ConstIterator it = sizes.begin(); it != sizes.end(); ++it )
        lst.append( QString::number( *it ) );

    q->setItems( lst );
}

void KFontSizeAction::setFontSize( int size )
{
    if ( size == fontSize() ) {
        const QString test = QString::number( size );
        Q_FOREACH(QAction* action, actions())
        {
          if (action->text() == test)
          {
              setCurrentAction(action);
              return;
          }
        }
    }

    if ( size < 1 ) {
        qWarning() << "KFontSizeAction: Size " << size << " is out of range";
        return;
    }

    QAction* a = action( QString::number( size ) );
    if ( !a ) {
        // Insert at the correct position in the list (to keep sorting)
        QList<int> lst;
        // Convert to list of ints
        QStringListIterator itemsIt( items() );
        while ( itemsIt.hasNext() )
            lst.append( itemsIt.next().toInt() );
        // New size
        lst.append( size );
        // Sort the list
        qSort( lst );
        Q_FOREACH( int it, lst ) {
            KAction* const action = addAction( QString::number(it) );
            if (it == size)
              setCurrentAction(action);
        }

    } else {
        setCurrentAction( a );
    }
}

int KFontSizeAction::fontSize() const
{
  return currentText().toInt();
}

void KFontSizeAction::actionTriggered( QAction* action )
{
    emit fontSizeChanged( action->text().toInt() );
    KSelectAction::actionTriggered( action );
}


/* This file was modified from the original file: kfontaction.cpp
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

#include "kfontaction.h"

#include <QToolBar>

#include <qdebug.h>
#include <qfontdialog.h>
#include <qicon.h>
#include <qlocale.h>
#include <qfontdatabase.h>
#include <qfontcombobox.h>


KFontAction::KFontAction(uint fontListCriteria, QObject *parent)
  : KSelectAction(parent), d(new KFontActionPrivate(this))
{
	QFontDatabase fonts;
	QStringList list = fonts.families((QFontDatabase::WritingSystem)fontListCriteria);
    //FontChooser::getFontList( list, fontListCriteria );
    KSelectAction::setItems( list );
    setEditable( true );
}

KFontAction::KFontAction(QObject *parent)
  : KSelectAction(parent), d(new KFontActionPrivate(this))
{
	QFontDatabase fonts;
	QStringList list = fonts.families();
    //KFontChooser::getFontList( list, 0 );
    KSelectAction::setItems( list );
    setEditable( true );
}

KFontAction::KFontAction(const QString & text, QObject *parent)
  : KSelectAction(text, parent), d(new KFontActionPrivate(this))
{
	QFontDatabase fonts;
	QStringList list = fonts.families();
    //KFontChooser::getFontList( list, 0 );
    KSelectAction::setItems( list );
    setEditable( true );
}

KFontAction::KFontAction(const QIcon &icon, const QString &text, QObject *parent)
  : KSelectAction(icon, text, parent), d(new KFontActionPrivate(this))
{
	QFontDatabase fonts;
	QStringList list = fonts.families();
    //KFontChooser::getFontList( list, 0 );
    KSelectAction::setItems( list );
    setEditable( true );
}

KFontAction::~KFontAction()
{
    delete d;
}

QString KFontAction::font() const
{
    return currentText();
}

QWidget* KFontAction::createWidget(QWidget* parent)
{
    //qDebug() << "KFontAction::createWidget()";
#ifdef __GNUC__
#warning FIXME: items need to be converted
#endif
    // This is the visual element on the screen.  This method overrides
    // the KSelectAction one, preventing KSelectAction from creating its
    // regular KComboBox.
    QFontComboBox *cb = new QFontComboBox( parent );

    //qDebug() << "\tset=" << font();
    // Do this before connecting the signal so that nothing will fire.
    cb->setCurrentFont( QFont( font().toLower() ) );
    //qDebug() << "\tspit back=" << cb->currentFont().family();

    connect( cb, SIGNAL( currentFontChanged( const QFont & ) ), SLOT(_k_slotFontChanged( const QFont&  ) ) );
    cb->setMinimumWidth( cb->sizeHint().width() );
    return cb;
}

/*
 * Maintenance note: Keep in sync with KFontComboBox::setCurrentFont()
 */
void KFontAction::setFont( const QString &family )
{
    //qDebug() << "KFontAction::setFont(" << family << ")";

    // Suppress triggered(QString) signal and prevent recursive call to ourself.
    d->settingFont++;

    foreach(QWidget *w, createdWidgets())
    {
        QFontComboBox *cb = qobject_cast<QFontComboBox *>(w);
        //qDebug() << "\tw=" << w << "cb=" << cb;

        if(!cb) continue;

        cb->setCurrentFont(QFont(family.toLower()));
        //qDebug() << "\t\tw spit back=" << cb->currentFont().family();
    }

    d->settingFont--;

    //qDebug() << "\tcalling setCurrentAction()";

    QString lowerName = family.toLower();
    if (setCurrentAction(lowerName, Qt::CaseInsensitive))
       return;

    int i = lowerName.indexOf(" [");
    if (i > -1)
    {
       lowerName = lowerName.left(i);
       i = 0;
       if (setCurrentAction(lowerName, Qt::CaseInsensitive))
          return;
    }

    lowerName += " [";
    if (setCurrentAction(lowerName, Qt::CaseInsensitive))
      return;

    // TODO: Inconsistent state if KFontComboBox::setCurrentFont() succeeded
    //       but setCurrentAction() did not and vice-versa.
    qDebug() << "Font not found " << family.toLower();
}

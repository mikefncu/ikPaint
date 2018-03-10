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

#ifndef KFONTACTION_H
#define KFONTACTION_H

#include <kselectaction.h>

/**
 * An action to select a font family.
 * On a toolbar this will show a combobox with all the fonts on the system.
 */
class KFontAction : public KSelectAction
{
    Q_OBJECT

public:
    KFontAction(uint fontListCriteria, QObject *parent);
    explicit KFontAction(QObject *parent);
    KFontAction(const QString& text, QObject *parent);
    KFontAction(const QIcon &icon, const QString &text, QObject *parent);
    virtual ~KFontAction();

    QString font() const;

    void setFont( const QString &family );

    virtual QWidget* createWidget(QWidget* parent);

private:
    class KFontActionPrivate
    {
        public:
            KFontActionPrivate(KFontAction *parent)
                : q(parent),
                  settingFont(0)
            {
            }

            void _k_slotFontChanged(const QFont &font)
            {
                //qDebug() << "KFontComboBox - slotFontChanged("
                //            << font.family() << ") settingFont=" << settingFont;
                if (settingFont)
                    return;

                q->setFont(font.family());
                q->triggered(font.family());

                //qDebug() << "\tslotFontChanged done";
            }


            KFontAction *q;
            int settingFont;
    };
    KFontActionPrivate * const d;

    Q_PRIVATE_SLOT( d, void _k_slotFontChanged(const QFont&) )
};

#endif

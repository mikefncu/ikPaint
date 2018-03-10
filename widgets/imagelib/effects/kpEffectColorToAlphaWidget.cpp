
/*
   Copyright (c) 2015 Maikel Diaz <mikefncu@yahoo.es>
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

#include "kpEffectColorToAlphaWidget.h"

#include <qboxlayout.h>
#include <qlabel.h>
#include <qpushbutton.h>
#include <qcolordialog.h>
#include <qcolor.h>

#include <qdebug.h>
#include <qlocale.h>
#include <tools.h>

#include <kpEffectColorToAlpha.h>
#include <kpEffectColorToAlphaCommand.h>

kpEffectColorToAlphaWidget::kpEffectColorToAlphaWidget (bool actOnSelection,
                                              QWidget *parent)
    : kpEffectWidgetBase (actOnSelection, parent)
{
    acolor = qRgb(255, 255, 255);

    QHBoxLayout *lay = new QHBoxLayout ();
    lay->setMargin (marginHint ());
    lay->setSpacing (spacingHint ());

    QLabel *colorLabel = new QLabel (i18n ("Color:"));
    m_color = new QPushButton ();
    m_color->setMinimumHeight(50);
    m_color->setMinimumWidth(150);
    m_color->setIconSize(QSize(130, 20));

    lay->addStretch(1);
    lay->addWidget(colorLabel);
    lay->addWidget(m_color);
    lay->addStretch(1);

    setLayout(lay);

    connect (m_color, SIGNAL (clicked ()),
             this, SLOT (changeAlphaColor ()));

    paintButton();
}

kpEffectColorToAlphaWidget::~kpEffectColorToAlphaWidget ()
{
}


// public virtual [base kpEffectWidgetBase]
QString kpEffectColorToAlphaWidget::caption () const
{
    return i18n ("Settings");
}


// public virtual [base kpEffectWidgetBase]
bool kpEffectColorToAlphaWidget::isNoOp () const
{
    return false;
}

// public virtual [base kpEffectWidgetBase]
kpImage kpEffectColorToAlphaWidget::applyEffect (const kpImage &image)
{
    return kpEffectColorToAlpha::applyEffect (image, alphaColor());
}

// public virtual [base kpEffectWidgetBase]
kpEffectCommandBase *kpEffectColorToAlphaWidget::createCommand (
        kpCommandEnvironment *cmdEnviron) const
{
    return new kpEffectColorToAlphaCommand (alphaColor (),
                                       m_actOnSelection,
                                       cmdEnviron);
}

// protected
QRgb kpEffectColorToAlphaWidget::alphaColor() const
{
    return acolor;
}

// protected
void kpEffectColorToAlphaWidget::paintButton() {

    QPixmap pm (130, 20);
    pm.fill(acolor);

    m_color->setIcon(pm);
}

// protected slot
void kpEffectColorToAlphaWidget::changeAlphaColor()
{
    QColor c = QColorDialog::getColor(acolor, this, i18n("Select Color to Alpha"));

    if (c.isValid()) {
        acolor = c.rgb();

        paintButton();

        emit settingsChangedNoWaitCursor();
    }
    // repaint
}



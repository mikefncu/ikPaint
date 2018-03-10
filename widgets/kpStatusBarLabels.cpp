/*
   Copyright (c) 2015-2018 Maikel Diaz <ariguanabosoft@gmail.com>
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

#include "kpStatusBarLabels.h"
#include <qframe.h>

kpStatusBarLabels::kpStatusBarLabels(QWidget *parent)
    : QWidget(parent) {

    hlt = new QHBoxLayout();
    hlt->setMargin(0);
    hlt->setSpacing(5);

    setLayout(hlt);
}

void kpStatusBarLabels::addLabel(int id, int width) {

    if (map.contains(id)) {
        // change size!
        map.value(id)->setMinimumWidth(width);
        map.value(id)->setMaximumWidth(width);

        return;
    }

    QLabel *label = new QLabel(this);
    label->setMinimumWidth(width);
    label->setMaximumWidth(width);
    label->setAlignment(Qt::AlignCenter);

    map.insert(id, label);

    QFrame * f = new QFrame();
    f->setFrameStyle(QFrame::VLine);
    f->setFrameShadow(QFrame::Sunken);
    hlt->addWidget(f);

    hlt->addWidget(label, 0, Qt::AlignCenter);
}

//void kpStatusBarLabels::insertLabel(int id, int index, int width) {}

void kpStatusBarLabels::setText(int index, const QString & text) {

    QLabel *l = map.value(index);
    if (l) {
        l->setText(text);
    }
}

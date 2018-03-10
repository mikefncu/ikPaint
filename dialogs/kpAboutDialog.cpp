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

#include "kpAboutDialog.h"

#include <qboxlayout.h>
#include <qlabel.h>
#include <qdialogbuttonbox.h>

kpAboutDialog::kpAboutDialog(QWidget *parent)
    : QDialog(parent) {

    QVBoxLayout *mainLayout = new QVBoxLayout();

    QHBoxLayout *aboutLayout = new QHBoxLayout();
    QLabel *label = new QLabel();
    label->setPixmap(QPixmap(":/appicon"));
    aboutLayout->addWidget(label);
    label = new QLabel(about());
    label->setWordWrap(true);
    aboutLayout->addWidget(label, 1);

    mainLayout->addLayout(aboutLayout);

    QDialogButtonBox *buttonBox = new QDialogButtonBox(QDialogButtonBox::Ok);
    connect(buttonBox, SIGNAL(accepted()), this, SLOT(accept()));
    mainLayout->addWidget(buttonBox);

    setMaximumSize(600, 550);
    setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);

    setLayout(mainLayout);
}

QString kpAboutDialog::about() {

    return tr("<b>ikPaint</b> is a (light?) modified version of <a href='http://www.kolourpaint.org/about.html'>KolourPaint</a> for macOS.<br><br>"
              "ikPaint removes all KDE dependencies of KolourPaint and it is compiled using only Qt 5 libraries. "
              "Some features from the original software were excluded and few new options were added.<br><br>"
              "LICENCE: ikPaint keeps all licence terms of original KolourPaint.<br><br>"
              "Author: Maikel Diaz (ariguanabosoft@gmail.com). The biggest credits are for the authors of original KolourPaint!");
}

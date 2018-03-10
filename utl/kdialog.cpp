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

#include "kdialog.h"
#include <qdebug.h>

#include <qpushbutton.h>

KDialog::KDialog(QWidget *parent) : QDialog(parent) {

	l = new QVBoxLayout();

	w = 0;
	seftCreated = false;

	b = new QDialogButtonBox();
	l->addWidget(b);

	setLayout(l);

    connect(b, SIGNAL(accepted()), this, SLOT(accept()));
    connect(b, SIGNAL(rejected()), this, SLOT(reject()));
}

void KDialog::setCaption(const QString &name) {

	setWindowTitle(name);
}

void KDialog::setButtons(QDialogButtonBox::StandardButtons btns) {

	b->setStandardButtons(btns);
}

QWidget * KDialog::mainWidget() {

	if (w == 0) {
		w = new QWidget(this);
		l->insertWidget(0, w, 1);

		seftCreated = true;
	}
	return w;
}

void KDialog::setMainWidget(QWidget *nw) {

	if (!nw) return;

	if (w != 0) {
		l->removeWidget(w);
        if (seftCreated) w->deleteLater();
	}

	l->insertWidget(0, nw, 1);

	w = nw;

	seftCreated = false;
}

void KDialog::enableButtonOk(bool e) {

	QPushButton *bt = b->button(QDialogButtonBox::Ok);
	if (bt) bt->setEnabled(e);
}

int KDialog::marginHint() {

	return 5;
}

int KDialog::spacingHint() {

	return 5;
}

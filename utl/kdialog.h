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

#ifndef KDIALOG_H_
#define KDIALOG_H_

#include <qdialog.h>
#include <qdialogbuttonbox.h>
#include <qlayout.h>

class KDialog: public QDialog {

	Q_OBJECT

	private:
		bool seftCreated;

		QWidget * w;
		QVBoxLayout * l;
		QDialogButtonBox *b;

	public:
		KDialog(QWidget *parent = 0);

		void setCaption(const QString &name);
		void setButtons(QDialogButtonBox::StandardButtons btns);

		QWidget * mainWidget();
		void setMainWidget(QWidget *);

		void enableButtonOk(bool e);

		static int marginHint();
		static int spacingHint();
};

#endif /* KDIALOG_H_ */

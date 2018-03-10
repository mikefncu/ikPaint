/* This file was modified from the original file: kaction.cpp
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

#include "kaction.h"
#include "kaction_p.h"
#include "qlocale.h"
#include "qmessagebox.h"

#include <qapplication.h>
#include <QShortcutEvent>

#include <qdebug.h>

#include "qicon.h"

//---------------------------------------------------------------------
// KActionPrivate
//---------------------------------------------------------------------

void KActionPrivate::init(KAction *q_ptr)
{
  q = q_ptr;

  QObject::connect(q, SIGNAL(triggered(bool)), q, SLOT(slotTriggered()));
}

void KActionPrivate::slotTriggered()
{
#ifdef KDE3_SUPPORT
  emit q->activated();
#endif
  emit q->triggered(QApplication::mouseButtons(), QApplication::keyboardModifiers());
}


bool KAction::event(QEvent *event)
{
    return QAction::event(event);
}


//---------------------------------------------------------------------
// KAction
//---------------------------------------------------------------------

KAction::KAction(QObject *parent)
  : QWidgetAction(parent), d(new KActionPrivate)
{
  d->init(this);
}

KAction::KAction(const QString &text, QObject *parent)
  : QWidgetAction(parent), d(new KActionPrivate)
{
  d->init(this);
  setText(text);
}

KAction::KAction(const QIcon &icon, const QString &text, QObject *parent)
  : QWidgetAction(parent), d(new KActionPrivate)
{
  d->init(this);
  setIcon(icon);
  setText(text);
}

KAction::~KAction()
{
    delete d;
}


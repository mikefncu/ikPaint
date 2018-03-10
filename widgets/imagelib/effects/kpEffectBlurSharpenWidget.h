
/*
   Copyright (c) 2003-2007 Clarence Dang <dang@kde.org>
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


#ifndef kpEffectBlurSharpenWidget_H
#define kpEffectBlurSharpenWidget_H


#include <kpColor.h>

#include <kpEffectWidgetBase.h>
#include <kpEffectBlurSharpen.h>


class QLabel;

class QSpinBox;


class kpEffectBlurSharpenWidget : public kpEffectWidgetBase
{
Q_OBJECT

public:
    kpEffectBlurSharpenWidget (bool actOnSelection, QWidget *parent);
    virtual ~kpEffectBlurSharpenWidget ();

    virtual QString caption () const;

    virtual bool isNoOp () const;
    virtual kpImage applyEffect (const kpImage &image);

    virtual kpEffectCommandBase *createCommand (
        kpCommandEnvironment *cmdEnviron) const;

protected slots:
    void slotUpdateTypeLabel ();

protected:
    kpEffectBlurSharpen::Type type () const;
    int strength () const;

    QSpinBox *m_amountInput;
    QLabel *m_typeLabel;
};


#endif  // kpEffectBlurSharpenWidget_H


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


#define DEBUG_KP_EFFECT_INVERT 0


#include <kpEffectInvertCommand.h>

#include <qcheckbox.h>
#include <qimage.h>
#include <qlayout.h>
#include <qpixmap.h>

#include <qdebug.h>
#include <qlocale.h>
#include <tools.h>

#include <kpEffectInvert.h>
#include <kpPixmapFX.h>


kpEffectInvertCommand::kpEffectInvertCommand (int channels,
        bool actOnSelection,
        kpCommandEnvironment *environ)
    : kpEffectCommandBase (channels == kpEffectInvert::RGB ?
                                i18n ("Invert Colors") : i18n ("Invert"),
                            actOnSelection, environ),
      m_channels (channels)
{
}

kpEffectInvertCommand::kpEffectInvertCommand (bool actOnSelection,
                                              kpCommandEnvironment *environ)
    : kpEffectCommandBase (i18n ("Invert Colors"), actOnSelection, environ),
      m_channels (kpEffectInvert::RGB)
{
}

kpEffectInvertCommand::~kpEffectInvertCommand ()
{
}


//
// kpEffectInvertCommand implements kpEffectCommandBase interface
//

// protected virtual [base kpEffectCommandBase]
kpImage kpEffectInvertCommand::applyEffect (const kpImage &image)
{
    return kpEffectInvert::applyEffect (image, m_channels);
}



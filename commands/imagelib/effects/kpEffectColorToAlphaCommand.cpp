/*
   Copyright (c) 2015 MAikel Diaz <mikefncu@yahoo.es>
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

#include "kpEffectColorToAlphaCommand.h"

#include <qdebug.h>
#include <qlocale.h>
#include <tools.h>

#include <kpEffectColorToAlpha.h>

kpEffectColorToAlphaCommand::kpEffectColorToAlphaCommand(
        QRgb color,
        bool actOnSelection,
        kpCommandEnvironment *environ)
    : kpEffectCommandBase (i18n ("Color to Alpha"), actOnSelection, environ),
      toalpha (color)
{
}

kpEffectColorToAlphaCommand::~kpEffectColorToAlphaCommand ()
{
}

// protected virtual [base kpEffectCommandBase]
kpImage kpEffectColorToAlphaCommand::applyEffect (const kpImage &image)
{
    return kpEffectColorToAlpha::applyEffect (image, toalpha);
}

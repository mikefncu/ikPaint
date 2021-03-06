
/*
   Copyright (c) 2003-2007 Clarence Dang <dang@kde.org>
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

#define DEBUG_KP_TOOL_CROP 0


#include <kpTransformCrop.h>
#include <kpTransformCropPrivate.h>

#include <kpEffectClearCommand.h>
#include <kpMacroCommand.h>
#include <kpMainWindow.h>
#include <kpToolSelectionMoveCommand.h>


void kpTransformCrop_TextSelection (kpMainWindow *mainWindow,
        const QString &commandName, kpCommand *resizeDocCommand)
{
    kpCommandEnvironment *environ = mainWindow->commandEnvironment ();


    kpMacroCommand *macroCmd = new kpMacroCommand (commandName, environ);

    macroCmd->addCommand (resizeDocCommand);

#if DEBUG_KP_TOOL_CROP
    kDebug () << "\tisText";
    kDebug () << "\tclearing doc with trans cmd";
#endif
    macroCmd->addCommand (
        new kpEffectClearCommand (
            false/*act on doc*/,
            kpColor::Transparent,
            environ));

#if DEBUG_KP_TOOL_CROP
    kDebug () << "\tmoving sel to (0,0) cmd";
#endif
    kpToolSelectionMoveCommand *moveCmd =
        new kpToolSelectionMoveCommand (
            QString()/*uninteresting child of macro cmd*/,
            environ);
    moveCmd->moveTo (QPoint (0, 0), true/*move on exec, not now*/);
    moveCmd->finalize ();
    macroCmd->addCommand (moveCmd);


    mainWindow->addImageOrSelectionCommand (
        macroCmd,
        true/*add create cmd*/,
        true/*add create content cmd*/);
}

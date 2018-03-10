
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



#include <qapplication.h>
#include <qdebug.h>
#include <qlocale.h>
#include <qmessagebox.h>

#include <kpDefs.h>
#include <kpMainWindow.h>
#include <kpApplication.h>
#include <kpDocument.h>


int main (int argc, char *argv [])
{

    kpApplication app(argc, argv);
    QStringList arg = QApplication::arguments();

    QCoreApplication::setOrganizationName("AriguanaboSoft");
    QCoreApplication::setOrganizationDomain("com.cu.ariguanabosoft");
    QCoreApplication::setApplicationName("ikPaint");

    app.setAttribute(Qt::AA_DontShowIconsInMenus);

    // keep ikPaint on Dock
    // app.setQuitOnLastWindowClosed(false);

    kpMainWindow *mainWindow = 0;

    if (arg.size() >= 2) {
        for (int i = 1; i < arg.size(); i++) {
            mainWindow = new kpMainWindow (arg.at(i));
            mainWindow->show ();
        }
	}
    else {

        // I dont know why but when I call processEvent without mainWindow->show()
        // OS X shows : "No document could be open ..."
        mainWindow = new kpMainWindow ();
        mainWindow->showMinimized ();

        // process FileOpenEvent if exist
        app.processEvents();

        // check if some mainWindow was open
        bool someOtherOpen = false;
        foreach (QWidget *widget, app.topLevelWidgets()) {
            kpMainWindow *otherMainWindow = dynamic_cast<kpMainWindow *>(widget);
            if (otherMainWindow && !otherMainWindow->document()->isEmpty()) // it means some other file open
                someOtherOpen = true;
        }

        if (someOtherOpen) mainWindow->deleteLater();
        else mainWindow->showNormal();
    }

    return app.exec ();
}


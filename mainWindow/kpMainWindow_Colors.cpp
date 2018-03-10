
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


#include <kpMainWindow.h>
#include <kpMainWindowPrivate.h>

#include <QAction>
#include <QFileDialog>
#include <QMessageBox>

#include <kactioncollection.h>
#include <qdebug.h>
#include <tools.h>

#include <kpColorCells.h>
#include <kpColorCollection.h>
#include <kpColorToolBar.h>
#include <kpUrlFormatter.h>


// private
void kpMainWindow::setupColorsMenuActions ()
{
    KActionCollection *ac = actionCollection ();


    d->actionColorsDefault = ac->addAction ("colors_default");
    d->actionColorsDefault->setText (i18n ("Use ik&Paint Defaults"));
    connect (d->actionColorsDefault, SIGNAL (triggered (bool)),
        SLOT (slotColorsDefault ()));

    d->actionColorsReload = ac->addAction ("colors_reload");
    d->actionColorsReload->setText (i18n ("Reloa&d"));
    connect (d->actionColorsReload, SIGNAL (triggered (bool)),
        SLOT (slotColorsReload ()));


    d->actionColorsSave = ac->addAction ("colors_save");
    d->actionColorsSave->setText (i18n ("&Save"));
    connect (d->actionColorsSave, SIGNAL (triggered (bool)),
        SLOT (slotColorsSave ()));

    d->actionColorsAppendColumn = ac->addAction ("colors_append_row");
    d->actionColorsAppendColumn->setText (i18n ("Add Column"));
    connect (d->actionColorsAppendColumn, SIGNAL (triggered (bool)),
        SLOT (slotColorsAppendColumn ()));

    d->actionColorsDeleteColumn = ac->addAction ("colors_delete_row");
    d->actionColorsDeleteColumn->setText (i18n ("Delete Last Column"));
    connect (d->actionColorsDeleteColumn, SIGNAL (triggered (bool)),
        SLOT (slotColorsDeleteColumn ()));


    enableColorsMenuDocumentActions (false);
}

// private
void kpMainWindow::createColorBox ()
{
    d->colorToolBar = new kpColorToolBar (i18n ("Color Box"), this);

    // (needed for QMainWindow::saveState())
    d->colorToolBar->setObjectName ( QLatin1String("Color Box" ));

    connect (colorCells (), SIGNAL (columnCountChanged (int)),
        SLOT (slotUpdateColorsAppendDeleteColumnActionEnabled ()));

    addToolBar(Qt::BottomToolBarArea, d->colorToolBar);
    d->colorToolBar->setMovable(false);
}

// private
void kpMainWindow::enableColorsMenuDocumentActions (bool enable)
{
    d->actionColorsDefault->setEnabled (enable);
    d->actionColorsReload->setEnabled (enable);

    d->actionColorsSave->setEnabled (enable);

    d->actionColorsAppendColumn->setEnabled (enable);

    d->colorMenuDocumentActionsEnabled = enable;

    slotUpdateColorsAppendDeleteColumnActionEnabled ();
}

// private slot
void kpMainWindow::slotUpdateColorsAppendDeleteColumnActionEnabled ()
{
    // Currently, this is always enabled since kpColorCells guarantees that
    // there will be at least one row of cells (which might all be of the
    // invalid color).
    //
    // But this method is left here for future extensibility.
    d->actionColorsDeleteColumn->setEnabled (
        d->colorMenuDocumentActionsEnabled && colorCells ()->canDeleteColumn());

    d->actionColorsAppendColumn->setEnabled(
        d->colorMenuDocumentActionsEnabled && colorCells ()->canAppendColumn ());
}


// private
bool kpMainWindow::queryCloseColors ()
{
#if DEBUG_KP_MAIN_WINDOW
    kDebug () << "kpMainWindow::queryCloseColors() colorCells.modified="
              << colorCells ()->isModified ();
#endif

    toolEndShape ();

    if (!colorCells ()->isModified ())
        return true;  // ok to close

    int result = QMessageBox::Cancel;

	// TODO: do not ask again
	result = QMessageBox::question (this, QString(),
		i18n ("The color palette has been modified.\n"
              "Do you want to save it for the next time you open ikPaint?"),
			  QMessageBox::Yes | QMessageBox::No | QMessageBox::Cancel);

    switch (result)
    {
    case QMessageBox::Yes:
        return slotColorsSave ();  // close only if save succeeds
    case QMessageBox::No:
        return true;  // close without saving
    default:
        return false;  // don't close current doc
    }
}

// private slot
void kpMainWindow::slotColorsDefault ()
{
    // Call just in case.
    toolEndShape ();

    if (colorCells ()->isModified ())
    {
        int result = QMessageBox::Cancel;

        // TODO: do not ask again
        result = QMessageBox::question (this, QString(),
            i18n ("Loading default ikPaint color palette.\n\n"
                  "Are you sure?"),
                  QMessageBox::Yes || QMessageBox:: Cancel);

        if (result != QMessageBox::Yes)
            return;
    }

    colorCells ()->setColorCollection (
        kpColorCells::DefaultColorCollection ());
}


// private slot
void kpMainWindow::slotColorsReload ()
{
    toolEndShape ();

    if (colorCells ()->isModified ())
    {
        int result = QMessageBox::Cancel;

        // TODO: do not ask again
		result = QMessageBox::question (this, QString(),
			i18n ("The color palette has been modified.\n"
				  "Reloading will lose all changes since you last saved it.\n"
				  "Are you sure?"),
				  QMessageBox::Yes || QMessageBox:: Cancel);


    #if DEBUG_KP_MAIN_WINDOW
        kDebug () << "result=" << result
                  << "vs KMessageBox::Continue" << KMessageBox::Continue;
    #endif

        if (result != QMessageBox::Yes)
            return;
    }

    colorCells ()->openColorCollection ();
}


// private slot
bool kpMainWindow::slotColorsSave ()
{
    // Call due to dialog.
    toolEndShape ();

    return colorCells ()->saveColorCollection ();
}

// private slot
void kpMainWindow::slotColorsAppendColumn ()
{
    // Call just in case.
    toolEndShape ();

    kpColorCells *colorCells = d->colorToolBar->colorCells ();
    colorCells->appendColumn ();
}

// private slot
void kpMainWindow::slotColorsDeleteColumn ()
{
    // Call just in case.
    toolEndShape ();

    kpColorCells *colorCells = d->colorToolBar->colorCells ();
    colorCells->deleteLastColumn ();
}

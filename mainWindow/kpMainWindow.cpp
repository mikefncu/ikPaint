
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

#include <qevent.h>
#include <qtimer.h>

#include <qsettings.h>
#include <qdebug.h>
#include <qglobal.h>
#include <qlocale.h>
#include <qmenu.h>
#include <qmenubar.h>
#include <tools.h>
#include <kactioncollection.h>
#include <qfontcombobox.h>

#include <kpAbstractImageSelection.h>
#include <kpCommandEnvironment.h>
#include <kpColorCells.h>
#include <kpColorToolBar.h>
#include <kpCommandHistory.h>
#include <kpDefs.h>
#include <kpDocument.h>
#include <kpDocumentEnvironment.h>
#include <kpSelectionDrag.h>
#include <kpTool.h>
#include <kpToolToolBar.h>
#include <kpViewManager.h>
#include <kpViewScrollableContainer.h>
#include <kpWidgetMapper.h>
#include <kpZoomedView.h>

#if DEBUG_KP_MAIN_WINDOW
    #include <qdatetime.h>
#endif


//---------------------------------------------------------------------

kpMainWindow::kpMainWindow ()
    : QMainWindow (0/*parent*/)
{
    init ();
    open (QString (), true/*create an empty doc*/);

    d->isFullyConstructed = true;
}

//---------------------------------------------------------------------

kpMainWindow::kpMainWindow (const QString &url)
    : QMainWindow (0/*parent*/)
{
    init ();
    open (url, true/*create an empty doc with the same url if url !exist*/);

    d->isFullyConstructed = true;
}

//---------------------------------------------------------------------

kpMainWindow::kpMainWindow (kpDocument *newDoc)
    : QMainWindow (0/*parent*/)
{
    init ();
    setDocument (newDoc);

    d->isFullyConstructed = true;
}

//---------------------------------------------------------------------


// TODO: Move into appropriate kpMainWindow_*.cpp or another class

// private
void kpMainWindow::readGeneralSettings ()
{
#if DEBUG_KP_MAIN_WINDOW
    kDebug () << "\tkpMainWindow(" << objectName () << ")::readGeneralSettings()";
#endif

	QSettings settings;
	settings.beginGroup(kpSettingsGroupGeneral);
	if (settings.contains("geometry")) restoreGeometry(settings.value("geometry").toByteArray());
	else {
		setGeometry(100, 100, 1000, 800);
	}
	d->configFirstTime = settings.value(kpSettingFirstTime, true).toBool();

    d->configFirstTime = settings.value(kpSettingFirstTime, true).toBool();
    d->configShowGrid = settings.value(kpSettingShowGrid, false).toBool();
    d->configShowPath = settings.value(kpSettingShowPath, false).toBool();
    d->moreEffectsDialogLastEffect = settings.value(kpSettingMoreEffectsLastEffect, 0).toInt();

    if (settings.contains(kpSettingOpenImagesInSameWindow))
    {
        d->configOpenImagesInSameWindow = settings.value(kpSettingOpenImagesInSameWindow, false).toBool();
    }
    else
    {
        d->configOpenImagesInSameWindow = false;
#if DEBUG_KP_MAIN_WINDOW
        kDebug () << "\tconfigOpenImagesInSameWindow: first time"
                  << " - writing default: " << d->configOpenImagesInSameWindow
                  << endl;
#endif
        // TODO: More hidden options have to write themselves out on startup,
        //       not on use, to be discoverable (e.g. printing centered on page).
        settings.setValue(kpSettingOpenImagesInSameWindow,
                        d->configOpenImagesInSameWindow);

    }

    d->configPrintImageCenteredOnPage = settings.value(kpSettingPrintImageCenteredOnPage, true). toBool();

	settings.endGroup();

#if DEBUG_KP_MAIN_WINDOW
    kDebug () << "\t\tGeneral Settings: firstTime=" << d->configFirstTime
               << " showGrid=" << d->configShowGrid
               << " showPath=" << d->configShowPath
               << " moreEffectsDialogLastEffect=" << d->moreEffectsDialogLastEffect
               << " openImagesInSameWindow=" << d->configOpenImagesInSameWindow
               << " printImageCenteredOnPage=" << d->configPrintImageCenteredOnPage;
#endif
}

//---------------------------------------------------------------------

// private
void kpMainWindow::init ()
{
#if DEBUG_KP_MAIN_WINDOW
    kDebug () << "kpMainWindow(" << objectName () << ")::init()";
    QTime totalTime; totalTime.start ();
#endif

    d = new kpMainWindowPrivate;

    //
    // Set fields that must be set early on.
    //
    // For all other fields that don't need to be set as urgently, please
    // set them in their respective kpMainWindow_*.cpp.
    //

    setWindowIcon(QIcon(":/winicon"));

    d->isFullyConstructed = false;

    d->scrollView = 0;
    d->mainView = 0;
    d->document = 0;
    d->viewManager = 0;
    d->colorToolBar = 0;
    d->toolToolBar = 0;
    d->toolBarText = 0;
    d->commandHistory = 0;
    d->statusBarCreated = false;
    d->settingImageSelectionTransparency = 0;
    d->settingTextStyle = 0;

    d->docResizeToBeCompleted = false;

    d->documentEnvironment = 0;
    d->commandEnvironment = 0;
    d->toolSelectionEnvironment = 0;
    d->toolsActionGroup = 0;
    d->transformDialogEnvironment = 0;


    //
    // set mainwindow properties
    //

    //setMinimumSize (800, 600);
    setAcceptDrops (true);

    //
    // read config
    //

    readGeneralSettings ();

    //
    // create action collection
    //

    d->ac = new KActionCollection(this);

    //
    // create GUI
    //
    setupActions ();
    createStatusBar ();
    createToolBox ();
    createColorBox ();
    createGUI ();

    // Let the Tool Box take all the vertical space, since it can be quite
    // tall with all its tool option widgets.  This also avoids occasional
    // bugs like the Tool Box overlapping the Color Tool Bar.
    setCorner(Qt::BottomLeftCorner, Qt::LeftDockWidgetArea);
    setCorner(Qt::BottomRightCorner, Qt::RightDockWidgetArea);

    // no tabbed docks; does not make sense with only 2 dock widgets
    setDockOptions(QMainWindow::AnimatedDocks | QMainWindow::AllowNestedDocks);

    //addDockWidget(Qt::BottomDockWidgetArea, d->colorToolBar, Qt::Horizontal);

    d->scrollView = new kpViewScrollableContainer (this);
    d->scrollView->setObjectName ( QLatin1String("scrollView" ));
    connect (d->scrollView, SIGNAL (beganDocResize ()),
             this, SLOT (slotBeganDocResize ()));
    connect (d->scrollView, SIGNAL (continuedDocResize (const QSize &)),
             this, SLOT (slotContinuedDocResize (const QSize &)));
    connect (d->scrollView, SIGNAL (cancelledDocResize ()),
             this, SLOT (slotCancelledDocResize ()));
    connect (d->scrollView, SIGNAL (endedDocResize (const QSize &)),
             this, SLOT (slotEndedDocResize (const QSize &)));

    connect (d->scrollView, SIGNAL (statusMessageChanged (const QString &)),
             this, SLOT (slotDocResizeMessageChanged (const QString &)));

    connect (d->scrollView, SIGNAL(contentsMoved()),
             this, SLOT(slotScrollViewAfterScroll()));

    setCentralWidget (d->scrollView);

    //
    // set initial pos/size of GUI
    //

    // our non-XMLGUI tools-toolbar will get initially the toolButtonStyle as
    // all other toolbars, but we want to show only icons for the tools by default
    // (have to do this _after_ setAutoSaveSettings as that applies the default settings)
    if (d->configFirstTime)
    {
    	d->toolToolBar->setToolButtonStyle(Qt::ToolButtonIconOnly);

		QSettings settings;
		settings.beginGroup(kpSettingsGroupGeneral);
		settings.setValue(kpSettingFirstTime, d->configFirstTime = false);
		settings.endGroup();
    }

    setAttribute(Qt::WA_DeleteOnClose);

#if DEBUG_KP_MAIN_WINDOW
    kDebug () << "\tall done in " << totalTime.elapsed () << "msec";
#endif
}

//---------------------------------------------------------------------

// private
void kpMainWindow::createGUI () {

    QMenu *menu = menuBar()->addMenu(tr("&File"));
    menu->addAction(d->ac->action("file_new"));
	menu->addAction(d->ac->action("file_open"));
    menu->addAction(d->ac->action("file_open_recent"));
	menu->addAction(d->ac->action("file_screenshot"));

	menu->addSeparator();

	//menu->addAction(d->ac->action("file_properties"));

	//menu->addSeparator();

	menu->addAction(d->ac->action("file_save"));
	menu->addAction(d->ac->action("file_save_as"));
	menu->addAction(d->ac->action("file_export"));

	menu->addSeparator();

	menu->addAction(d->ac->action("file_revert"));

	menu->addSeparator();

	menu->addAction(d->ac->action("file_print"));
	menu->addAction(d->ac->action("file_print_preview"));

	menu->addSeparator();

	menu->addAction(d->ac->action("file_close"));

	menu->addSeparator();

	menu->addAction(d->ac->action("file_quit"));

	//
	// Edit
	//
    menu = menuBar()->addMenu(tr("&Edit"));

    //menu->addSeparator();

    menu->addAction(d->ac->action("edit_undo-menu"));
    menu->addAction(d->ac->action("edit_redo-menu"));

	menu->addSeparator();

	menu->addAction(d->ac->action("edit_cut"));
	menu->addAction(d->ac->action("edit_copy"));
	menu->addAction(d->ac->action("edit_paste"));
	menu->addAction(d->ac->action("edit_paste_in_new_window"));
	menu->addAction(d->ac->action("edit_clear"));

	menu->addSeparator();

	menu->addAction(d->ac->action("edit_select_all"));
    menu->addAction(d->ac->action("edit_deselect"));

	menu->addSeparator();

	menu->addAction(d->ac->action("edit_copy_to_file"));
	menu->addAction(d->ac->action("edit_paste_from_file"));

	//
	// View
	//
    menu = menuBar()->addMenu(tr("&View"));

	menu->addAction(d->ac->action("view_actual_size"));
	menu->addAction(d->ac->action("view_fit_to_page"));
	menu->addAction(d->ac->action("view_fit_to_width"));
	menu->addAction(d->ac->action("view_fit_to_height"));

	menu->addSeparator();

	menu->addAction(d->ac->action("view_zoom_in"));
	menu->addAction(d->ac->action("view_zoom_to"));
	menu->addAction(d->ac->action("view_zoom_out"));

	menu->addSeparator();

	menu->addAction(d->ac->action("settings_fullscreen"));

    menu->addSeparator();

    menu->addAction(d->ac->action("settings_show_path"));

	//
	// Image
	//
    menu = menuBar()->addMenu(tr("&Image"));

	menu->addAction(d->ac->action("image_crop"));
	menu->addAction(d->ac->action("image_auto_crop"));

	menu->addSeparator();

	menu->addAction(d->ac->action("image_resize_scale"));
	menu->addAction(d->ac->action("image_flip"));
	menu->addAction(d->ac->action("image_mirror"));

	menu->addSeparator();

	menu->addAction(d->ac->action("image_rotate_270deg"));
	menu->addAction(d->ac->action("image_rotate_90deg"));
	menu->addAction(d->ac->action("image_rotate"));

	menu->addSeparator();

	menu->addAction(d->ac->action("image_skew"));

	menu->addSeparator();

	menu->addAction(d->ac->action("image_convert_to_black_and_white"));
	menu->addAction(d->ac->action("image_convert_to_grayscale"));
	menu->addAction(d->ac->action("image_more_effects"));

	menu->addSeparator();

	menu->addAction(d->ac->action("image_invert_colors"));
	menu->addAction(d->ac->action("image_clear"));

	menu->addSeparator();

	menu->addAction(d->ac->action("image_draw_opaque"));
	menu->addAction(d->ac->action("image_draw_color_similarity"));

	//
	// Color
	//
    menu = menuBar()->addMenu(tr("&Color"));

	menu->addAction(d->ac->action("colors_default"));

	menu->addSeparator();

	menu->addAction(d->ac->action("colors_save"));
	menu->addAction(d->ac->action("colors_reload"));

	menu->addSeparator();

	menu->addAction(d->ac->action("colors_append_row"));
	menu->addAction(d->ac->action("colors_delete_row"));

	//
	// Help
	//
    menu = menuBar()->addMenu(tr("&Help"));
    menu->addAction(tr("&About"), this, SLOT(slotAbout()));

	// ////////////////////////////////////////////////////////////////////////
	//		ToolBar
	// ////////////////////////////////////////////////////////////////////////

	//
	// Main
	//

	QToolBar *bar = addToolBar(tr("Main Toolbar"));

	bar->setOrientation(Qt::Horizontal);
	bar->setAllowedAreas(Qt::TopToolBarArea);
	bar->setToolButtonStyle(Qt::ToolButtonTextBesideIcon);

    bar->addAction(d->ac->action("file_new"));
    bar->addAction(d->ac->action("file_open"));
    bar->addAction(d->ac->action("file_save"));

	bar->addSeparator();

	bar->addAction(d->ac->action("edit_undo"));
	bar->addAction(d->ac->action("edit_redo"));

	bar->addSeparator();

	bar->addAction(d->ac->action("view_zoom_out"));
	bar->addAction(d->ac->action("view_zoom_to"));
	bar->addAction(d->ac->action("view_zoom_in"));

	//
	// Font
	//

	bar = d->toolBarText = addToolBar(tr("Text Toolbar"));

	bar->setOrientation(Qt::Horizontal);
	bar->setAllowedAreas(Qt::TopToolBarArea);

	bar->addAction(d->ac->action("text_font_family"));
	bar->addAction(d->ac->action("text_font_size"));

	bar->addSeparator();

	bar->addAction(d->ac->action("text_bold"));
	bar->addAction(d->ac->action("text_italic"));
	bar->addAction(d->ac->action("text_underline"));
	bar->addAction(d->ac->action("text_strike_thru"));
}


//---------------------------------------------------------------------

void kpMainWindow::saveSettings ()
{
#if DEBUG_KP_MAIN_WINDOW
    kDebug () << "kpMainWindow<" << this << ">::saveProperties()";
#endif

	QSettings settings;
	settings.beginGroup(kpSettingsGroupGeneral);
	settings.setValue("geometry", saveGeometry());
	settings.endGroup();
}

//---------------------------------------------------------------------


kpMainWindow::~kpMainWindow ()
{
	d->isFullyConstructed = false;

    // Get the kpTool to finish up.  This makes sure that the kpTool destructor
    // will not need to access any other class (that might be deleted before
    // the destructor is called by the QObject child-deletion mechanism).
    if (tool ())
        tool ()->endInternal ();

    // Delete document & views.
    // Note: This will disconnects signals from the current kpTool, so kpTool
    //       must not be destructed yet.
    setDocument (0);

    delete d->commandHistory; d->commandHistory = 0;
    delete d->scrollView; d->scrollView = 0;

    delete d; d = 0;
}

//---------------------------------------------------------------------


// public
kpDocument *kpMainWindow::document () const
{
    return d->document;
}

//---------------------------------------------------------------------

// public
kpDocumentEnvironment *kpMainWindow::documentEnvironment ()
{
    if (!d->documentEnvironment)
        d->documentEnvironment = new kpDocumentEnvironment (this);

    return d->documentEnvironment;
}

//---------------------------------------------------------------------

// public
kpViewManager *kpMainWindow::viewManager () const
{
    return d->viewManager;
}

//---------------------------------------------------------------------

// public
kpColorToolBar *kpMainWindow::colorToolBar () const
{
    return d->colorToolBar;
}

//---------------------------------------------------------------------

// public
kpColorCells *kpMainWindow::colorCells () const
{
    return d->colorToolBar ? d->colorToolBar->colorCells () : 0;
}

//---------------------------------------------------------------------

// public
kpToolToolBar *kpMainWindow::toolToolBar () const
{
    return d->toolToolBar;
}

//---------------------------------------------------------------------

// public
kpCommandHistory *kpMainWindow::commandHistory () const
{
    return d->commandHistory;
}

//---------------------------------------------------------------------

kpCommandEnvironment *kpMainWindow::commandEnvironment ()
{
    if (!d->commandEnvironment)
        d->commandEnvironment = new kpCommandEnvironment (this);

    return d->commandEnvironment;
}

//---------------------------------------------------------------------

// private
void kpMainWindow::setupActions ()
{
    setupFileMenuActions ();
    setupEditMenuActions ();
    setupViewMenuActions ();
    setupImageMenuActions ();
    setupColorsMenuActions ();
    setupSettingsMenuActions ();

    setupTextToolBarActions ();
    setupToolActions ();
}

//---------------------------------------------------------------------

// private
void kpMainWindow::enableDocumentActions (bool enable)
{
    enableFileMenuDocumentActions (enable);
    enableEditMenuDocumentActions (enable);
    enableViewMenuDocumentActions (enable);
    enableImageMenuDocumentActions (enable);
    enableColorsMenuDocumentActions (enable);
    enableSettingsMenuDocumentActions (enable);
}

//---------------------------------------------------------------------

// private
void kpMainWindow::setDocument (kpDocument *newDoc)
{
#if DEBUG_KP_MAIN_WINDOW
    kDebug () << "kpMainWindow::setDocument (" << newDoc << ")";
#endif

    // is it a close operation?
    if (!newDoc)
    {
    #if DEBUG_KP_MAIN_WINDOW
        kDebug () << "\tdisabling actions";
    #endif

        // sync with the bit marked "sync" below

        // TODO: Never disable the Color Box because the user should be
        //       able to manipulate the colors, even without a currently
        //       open document.
        //
        //       We just have to make sure that signals from the Color
        //       Box aren't fired and received unexpectedly when there's
        //       no document.
        Q_ASSERT (d->colorToolBar);
        d->colorToolBar->setEnabled (false);

        enableTextToolBarActions (false);
    }

    // Always disable the tools.
    // If we decide to open a new document/mainView we want
    // kpTool::begin() to be called again e.g. in case it sets the cursor.
    // kpViewManager won't do this because we nuke it to avoid stale state.
    enableToolsDocumentActions (false);

    if (!newDoc)
    {
        enableDocumentActions (false);
    }

#if DEBUG_KP_MAIN_WINDOW
    kDebug () << "\tdestroying views";
#endif

    delete d->mainView; d->mainView = 0;

#if DEBUG_KP_MAIN_WINDOW
    kDebug () << "\tdestroying viewManager";
#endif

    // viewManager will die and so will the selection
    d->actionCopy->setEnabled (false);
    d->actionCut->setEnabled (false);
    d->actionDelete->setEnabled (false);
    d->actionDeselect->setEnabled (false);
    d->actionCopyToFile->setEnabled (false);

    delete d->viewManager; d->viewManager = 0;

#if DEBUG_KP_MAIN_WINDOW
    kDebug () << "\tdestroying document";
    kDebug () << "\t\td->document=" << d->document;
#endif
    // destroy current document
    delete d->document;
    d->document = newDoc;


    if (!d->lastCopyToURL.isEmpty ())
        d->lastCopyToURL = QString();
    d->copyToFirstTime = true;

    if (!d->lastExportURL.isEmpty ())
        d->lastExportURL = QString();
    d->exportFirstTime = true;


    // not a close operation?
    if (d->document)
    {
    #if DEBUG_KP_MAIN_WINDOW
        kDebug () << "\treparenting doc that may have been created into a"
                  << " different mainWindiow" << endl;
    #endif
        d->document->setEnviron (documentEnvironment ());

    #if DEBUG_KP_MAIN_WINDOW
        kDebug () <<"\tcreating viewManager";
    #endif
        d->viewManager = new kpViewManager (this);

    #if DEBUG_KP_MAIN_WINDOW
        kDebug () << "\tcreating views";
    #endif
        d->mainView = new kpZoomedView (d->document, d->toolToolBar, d->viewManager,
                                       0/*buddyView*/,
                                       d->scrollView,
                                       d->scrollView->viewport ());
        d->mainView->setObjectName ( QLatin1String("mainView" ));

        d->scrollView->setView (d->mainView);
        d->viewManager->registerView (d->mainView);
        d->mainView->show ();

    #if DEBUG_KP_MAIN_WINDOW
        kDebug () << "\thooking up document signals";
    #endif

        // Copy/Cut/Deselect/Delete
        connect (d->document, SIGNAL (selectionEnabled (bool)),
                 d->actionCut, SLOT (setEnabled (bool)));
        connect (d->document, SIGNAL (selectionEnabled (bool)),
                 d->actionCopy, SLOT (setEnabled (bool)));
        connect (d->document, SIGNAL (selectionEnabled (bool)),
                 d->actionDelete, SLOT (setEnabled (bool)));
        connect (d->document, SIGNAL (selectionEnabled (bool)),
                 d->actionDeselect, SLOT (setEnabled (bool)));
        connect (d->document, SIGNAL (selectionEnabled (bool)),
                 d->actionCopyToFile, SLOT (setEnabled (bool)));

        // this code won't actually enable any actions at this stage
        // (fresh document) but better safe than sorry
        d->actionCopy->setEnabled (d->document->selection ());
        d->actionCut->setEnabled (d->document->selection ());
        d->actionDeselect->setEnabled (d->document->selection ());
        d->actionDelete->setEnabled (d->document->selection ());
        d->actionCopyToFile->setEnabled (d->document->selection ());

        connect (d->document, SIGNAL (selectionEnabled (bool)),
                 this, SLOT (slotImageMenuUpdateDueToSelection ()));
        connect (d->document, SIGNAL (selectionIsTextChanged (bool)),
                 this, SLOT (slotImageMenuUpdateDueToSelection ()));

        // Status bar
        connect (d->document, SIGNAL (documentOpened ()),
                 this, SLOT (recalculateStatusBar ()));

        connect (d->document, SIGNAL (sizeChanged (const QSize &)),
                 this, SLOT (setStatusBarDocSize (const QSize &)));

        // Caption (url, modified)
        connect (d->document, SIGNAL (documentModified ()),
                 this, SLOT (slotUpdateCaption ()));
        connect (d->document, SIGNAL (documentOpened ()),
                 this, SLOT (slotUpdateCaption ()));
        connect (d->document, SIGNAL (documentSaved ()),
                 this, SLOT (slotUpdateCaption ()));

        // File/Reload action only available with non-empty URL
        connect (d->document, SIGNAL (documentSaved ()),
                 this, SLOT (slotEnableReload ()));

        connect (d->document, SIGNAL (documentSaved ()),
                 this, SLOT (slotEnableSettingsShowPath ()));

        // Command history
        Q_ASSERT (d->commandHistory);
        connect (d->commandHistory, SIGNAL (documentRestored ()),
                 this, SLOT (slotDocumentRestored ()));  // caption "!modified"
        connect (d->document, SIGNAL (documentSaved ()),
                 d->commandHistory, SLOT (documentSaved ()));

        // Sync document -> views
        connect (d->document, SIGNAL (contentsChanged (const QRect &)),
                 d->viewManager, SLOT (updateViews (const QRect &)));
        connect (d->document, SIGNAL (sizeChanged (int, int)),
                 d->viewManager, SLOT (adjustViewsToEnvironment ()));

    #if DEBUG_KP_MAIN_WINDOW
        kDebug () << "\tenabling actions";
    #endif

        // sync with the bit marked "sync" above

        Q_ASSERT (d->colorToolBar);
        d->colorToolBar->setEnabled (true);


        // Hide the text toolbar - it will be shown by kpToolText::begin()
        enableTextToolBarActions (false);

        enableToolsDocumentActions (true);

        enableDocumentActions (true);
    }

#if DEBUG_KP_MAIN_WINDOW
    kDebug () << "\tupdating mainWindow elements";
#endif

    slotImageMenuUpdateDueToSelection ();
    recalculateStatusBar ();
    slotUpdateCaption ();  // Untitled to start with
    slotEnableReload ();
    slotEnableSettingsShowPath ();

    if (d->commandHistory)
        d->commandHistory->clear ();

#if DEBUG_KP_MAIN_WINDOW
    kDebug () << "\tdocument and views ready to go!";
#endif
}

//---------------------------------------------------------------------

// private virtual [base QWidget]
void kpMainWindow::dragEnterEvent (QDragEnterEvent *e)
{
    // It's faster to test for QMimeData::hasText() first due to the
    // lazy evaluation of the '||' operator.
    e->setAccepted (e->mimeData ()->hasText () ||
                    //KUrl::List::canDecode (e->mimeData ()) ||
                    kpSelectionDrag::canDecode (e->mimeData ()));
}

//---------------------------------------------------------------------

// private virtual [base QWidget]
void kpMainWindow::dropEvent (QDropEvent *e)
{
#if DEBUG_KP_MAIN_WINDOW
    kDebug () << "kpMainWindow::dropEvent" << e->pos ();
#endif

    QList<QUrl> urls;
    //KUrl::List urls;

    kpAbstractImageSelection *sel = kpSelectionDrag::decode (e->mimeData ());
    if (sel)
    {
        // TODO: How do you actually drop a selection or ordinary images on
        //       the clipboard)?  Will this code path _ever_ execute?
        sel->setTransparency (imageSelectionTransparency ());
        // TODO: drop at point like with QTextDrag below?
        paste (*sel);
        delete sel;
    }
    else if (!(urls = e->mimeData ()->urls()).isEmpty ()) // if (!(urls = KUrl::List::fromMimeData (e->mimeData ())).isEmpty ())
    {
        // LOTODO: kpSetOverrideCursorSaver cursorSaver (Qt::waitCursor);
        //
        //         However, you would need to prefix all possible error/warning
        //         dialogs that might be called, with Qt::arrowCursor e.g. in
        //         kpDocument  and probably a lot more places.
        foreach (const QUrl &u, urls)
            if (u.isLocalFile()) open (u.toLocalFile());

    }
    else if (e->mimeData ()->hasText ())
    {
        const QString text = e->mimeData ()->text ();

        QPoint selTopLeft = KP_INVALID_POINT;
        const QPoint globalPos = QWidget::mapToGlobal (e->pos ());
    #if DEBUG_KP_MAIN_WINDOW
        kDebug () << "\tpos toGlobal=" << globalPos;
    #endif

        kpView *view = 0;

        if (d->viewManager)
        {
            view = d->viewManager->viewUnderCursor ();
        #if DEBUG_KP_MAIN_WINDOW
            kDebug () << "\t\tviewUnderCursor=" << view;
        #endif
            if (!view)
            {
                // HACK: see kpViewManager::setViewUnderCursor() to see why
                //       it's not reliable
            #if DEBUG_KP_MAIN_WINDOW
                kDebug () << "\t\tattempting to discover view";

                if (d->mainView && d->scrollView)
                {
                    kDebug () << "\t\t\tmainView->globalRect="
                            << kpWidgetMapper::toGlobal (d->mainView, d->mainView->rect ())
                            << " scrollView->globalRect="
                            << kpWidgetMapper::toGlobal (d->scrollView,
                                    QRect (0, 0,
                                            d->scrollView->viewport()->width (),
                                            d->scrollView->viewport()->height ()))
                            << endl;
                }
            #endif

                if (d->mainView &&
                         kpWidgetMapper::toGlobal (d->mainView, d->mainView->rect ())
                             .contains (globalPos) &&
                         d->scrollView &&
                         kpWidgetMapper::toGlobal (d->scrollView,
                             QRect (0, 0,
                                    d->scrollView->viewport()->width (),
                                    d->scrollView->viewport()->height ()))
                             .contains (globalPos))
                {
                    view = d->mainView;
                }
            }
        }

        if (view)
        {
            const QPoint viewPos = view->mapFromGlobal (globalPos);
            const QPoint docPoint = view->transformViewToDoc (viewPos);

            // viewUnderCursor() is hacky and can return a view when we aren't
            // over one thanks to drags.
            if (d->document && d->document->rect ().contains (docPoint))
            {
                selTopLeft = docPoint;

                // TODO: In terms of doc pixels, would be inconsistent behaviour
                //       based on zoomLevel of view.
                // selTopLeft -= QPoint (-view->selectionResizeHandleAtomicSize (),
                //                       -view->selectionResizeHandleAtomicSize ());
            }
        }

        pasteText (text, true/*force new text selection*/, selTopLeft);
    }
}

//---------------------------------------------------------------------

// private slot
void kpMainWindow::slotScrollViewAfterScroll ()
{
    // OPT: Why can't this be moved into kpViewScrollableContainer::slotDragScroll(),
    //      grouping all drag-scroll-related repaints, which would potentially avoid
    //      double repainting?
    if (tool ())
    {
        tool ()->somethingBelowTheCursorChanged ();
    }
}

//---------------------------------------------------------------------

// private virtual [base QWidget]
void kpMainWindow::moveEvent (QMoveEvent * /*e*/)
{

}

//---------------------------------------------------------------------

// private slot
void kpMainWindow::slotUpdateCaption ()
{
    if (d->document)
    {
    	if (d->document->isModified ())
    		setWindowTitle(d->configShowPath ? QString("*%1").arg(d->document->prettyUrl ())
                                     : QString("*%1").arg(d->document->prettyFilename ()));
    	else
    		setWindowTitle(d->configShowPath ? d->document->prettyUrl ()
                                     : d->document->prettyFilename ());
    }
    else
    {
    	setWindowTitle (QString());
    }
}

//---------------------------------------------------------------------

// private slot
void kpMainWindow::slotDocumentRestored ()
{
    if (d->document)
        d->document->setModified (false);
    slotUpdateCaption ();
}

//---------------------------------------------------------------------

KActionCollection *kpMainWindow::actionCollection () {

	return d->ac;
}

//---------------------------------------------------------------------


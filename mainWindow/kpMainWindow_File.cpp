
/*
   Copyright (c) 2003-2007 Clarence Dang <dang@kde.org>
   Copyright (c) 2007 Martin Koller <kollix@aon.at>
   Copyright (c) 2007 John Layt <john@layt.net>
   Copyright (c) 2011 Martin Koller <kollix@aon.at>
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

#include <qdatastream.h>
#include <QDesktopWidget>
#include <qpainter.h>
#include <qpixmap.h>
#include <qsize.h>
#include <QPrinter>
#include <QPrintDialog>
#include <qprintpreviewdialog.h>
#include <QApplication>
#include <QTimer>
#include <QLabel>
#include <QCheckBox>
#include <QVBoxLayout>

#if QT_VERSION > 0x050000
#include <QStandardPaths>
#include <qscreen.h>
#else
#endif

#include <qdialog.h>
#include <qaction.h>
#include <kactioncollection.h>
#include <qsettings.h>

#include <qdebug.h>
#include <qspinbox.h>
#include <qfiledialog.h>
#include <qglobal.h>

#include <qlocale.h>
#include <qmessagebox.h>
#include <krecentfilesaction.h>

#include <tools.h>
#include <qimagewriter.h>
#include <qimagereader.h>
#include <qevent.h>

#include <kpCommandHistory.h>
#include <kpDefs.h>
#include <kpDocument.h>
#include <kpDocumentMetaInfo.h>
#include <kpDocumentMetaInfoCommand.h>
#include <kpDocumentMetaInfoDialog.h>

#include <kpPixmapFX.h>
#include <kpPrintDialogPage.h>
#include <kpView.h>
#include <kpViewManager.h>

#include <kpAboutDialog.h>

// private
void kpMainWindow::setupFileMenuActions ()
{
#if DEBUG_KP_MAIN_WINDOW
    kDebug () << "kpMainWindow::setupFileMenuActions()";
#endif
    KActionCollection *ac = actionCollection ();

    d->actionNew = ac->addAction("file_new");
    d->actionNew->setText (i18n ("New"));
    d->actionNew->setIcon (QIcon (":/document-new"));
    d->actionNew->setShortcuts(QKeySequence::New);
    connect(d->actionNew, SIGNAL(triggered(bool) ), SLOT (slotNew ()));

    d->actionOpen = ac->addAction("file_open");
    d->actionOpen->setText (i18n ("Open"));
    d->actionOpen->setIcon (QIcon (":/document-open"));
    d->actionOpen->setShortcuts(QKeySequence::Open);
    connect(d->actionOpen, SIGNAL(triggered(bool) ), SLOT (slotOpen ()));

    QSettings cfg;
    cfg.beginGroup(kpSettingsGroupRecentFiles);
    d->actionOpenRecent = ac->add<KRecentFilesAction> ("file_open_recent");
    d->actionOpenRecent->setText (i18n ("Open &Recent..."));
    connect(d->actionOpenRecent, SIGNAL(urlSelected(const QString &) ), SLOT (slotOpenRecent (const QString &)));
    d->actionOpenRecent->loadEntries (cfg);
    cfg.endGroup();
#if DEBUG_KP_MAIN_WINDOW
    kDebug () << "\trecent URLs=" << d->actionOpenRecent->items ();
#endif


    d->actionSave = ac->addAction("file_save");
    d->actionSave->setText (i18n ("&Save"));
    d->actionSave->setIcon (QIcon (":/document-save"));
    d->actionSave->setShortcuts(QKeySequence::Save);
    connect(d->actionSave, SIGNAL(triggered(bool) ), SLOT (slotSave ()));

    d->actionSaveAs = ac->addAction("file_save_as");
    d->actionSaveAs->setText (i18n ("Save &As..."));
    d->actionSaveAs->setShortcuts(QKeySequence::SaveAs);
    connect(d->actionSaveAs, SIGNAL(triggered(bool) ), SLOT (slotSaveAs ()));

    d->actionExport = ac->addAction("file_export");
    d->actionExport->setText (i18n ("E&xport"));
    connect(d->actionExport, SIGNAL(triggered(bool) ), SLOT (slotExport ()));

    d->actionScreenshot = ac->addAction("file_screenshot");
    d->actionScreenshot->setText(i18n("Acquire Screenshot"));
    connect(d->actionScreenshot, SIGNAL(triggered(bool)), SLOT(slotScreenshot()));

    d->actionProperties = ac->addAction ("file_properties");
    d->actionProperties->setText (i18n ("Properties"));
    connect (d->actionProperties, SIGNAL (triggered (bool)), SLOT (slotProperties ()));

    d->actionReload = ac->addAction ("file_revert");
    d->actionReload->setText (i18n ("Reloa&d"));
    connect(d->actionReload, SIGNAL(triggered(bool) ), SLOT (slotReload ()));
    slotEnableReload ();

    d->actionPrint = ac->addAction("file_print");
    d->actionPrint->setText (i18n ("&Print"));
    d->actionPrint->setShortcuts(QKeySequence::Print);
    connect(d->actionPrint, SIGNAL(triggered(bool) ), SLOT (slotPrint ()));

    d->actionPrintPreview = ac->addAction("file_print_preview");
    d->actionPrintPreview->setText (i18n ("Print previe&w..."));
    connect(d->actionPrintPreview, SIGNAL(triggered(bool) ), SLOT (slotPrintPreview ()));

    d->actionClose = ac->addAction("file_close");
    d->actionClose->setText (i18n ("&Close"));
    d->actionClose->setShortcuts(QKeySequence::Close);
    connect(d->actionClose, SIGNAL(triggered(bool) ), SLOT (slotClose ()));

    d->actionQuit = ac->addAction("file_quit");
    d->actionQuit->setText (i18n ("&Quit"));
    //d->actionQuit->setIcon (QIcon ("document-quit"));
    connect(d->actionQuit, SIGNAL(triggered(bool) ), SLOT (slotQuit ()));

    enableFileMenuDocumentActions (false);
}

//---------------------------------------------------------------------

// private
void kpMainWindow::enableFileMenuDocumentActions (bool enable)
{
    // d->actionNew
    // d->actionOpen

    // d->actionOpenRecent

    d->actionSave->setEnabled (enable);
    d->actionSaveAs->setEnabled (enable);

    d->actionExport->setEnabled (enable);

    d->actionProperties->setEnabled (enable);

    // d->actionReload

    d->actionPrint->setEnabled (enable);
    d->actionPrintPreview->setEnabled (enable);

    d->actionClose->setEnabled (enable);
    // d->actionQuit->setEnabled (enable);
}

//---------------------------------------------------------------------

// private
void kpMainWindow::addRecentURL (const QString &url_)
{
    // HACK: KRecentFilesAction::loadEntries() clears the KRecentFilesAction::d->urls
    //       map.
    //
    //       So afterwards, the URL ref, our method is given, points to an
    //       element in this now-cleared map (see KRecentFilesAction::urlSelected(QAction*)).
    //       Accessing it would result in a crash.
    //
    //       To avoid the crash, make a copy of it before calling
    //       loadEntries() and use this copy, instead of the to-be-dangling
    //       ref.
    const QString url = url_;

#if DEBUG_KP_MAIN_WINDOW
    kDebug () << "kpMainWindow::addRecentURL(" << url << ")";
#endif
    if (url.isEmpty ())
        return;

#if DEBUG_KP_MAIN_WINDOW
    kDebug () << "\trecent URLs=" << d->actionOpenRecent->items ();
#endif
    // HACK: Something might have changed interprocess.
    // If we could PROPAGATE: interprocess, then this wouldn't be required.
    QSettings cfg;
    cfg.beginGroup(kpSettingsGroupRecentFiles);
    d->actionOpenRecent->loadEntries (cfg);
#if DEBUG_KP_MAIN_WINDOW
    kDebug () << "\tafter loading config=" << d->actionOpenRecent->items ();
#endif

    d->actionOpenRecent->addUrl (url);

    d->actionOpenRecent->saveEntries (cfg);

#if DEBUG_KP_MAIN_WINDOW
    kDebug () << "\tnew recent URLs=" << d->actionOpenRecent->items ();
#endif


    // TODO: PROPAGATE: interprocess
    // TODO: Is this loop safe since a KMainWindow later along in the list,
    //       could be closed as the code in the body almost certainly re-enters
    //       the event loop?  Problem for KDE 3 as well, I think.
    foreach (QObject *kmw, QMainWindow::children ())
    {
        if (dynamic_cast <kpMainWindow *> (kmw) == 0) continue;
        kpMainWindow *mw = static_cast <kpMainWindow *> (kmw);

    #if DEBUG_KP_MAIN_WINDOW
        kDebug () << "\t\tmw=" << mw;
    #endif

        if (mw != this)
        {
            // WARNING: Do not use KRecentFilesAction::setItems()
            //          - it does not work since only its superclass,
            //          KSelectAction, implements setItems() and can't
            //          update KRecentFilesAction's URL list.

            // Avoid URL memory leak in KRecentFilesAction::loadEntries().
            mw->d->actionOpenRecent->clear ();

            mw->d->actionOpenRecent->loadEntries (cfg);
        #if DEBUG_KP_MAIN_WINDOW
            kDebug () << "\t\t\tcheck recent URLs="
                        << mw->d->actionOpenRecent->items () << endl;
        #endif
        }
    }

    cfg.endGroup();
}

//---------------------------------------------------------------------


// private slot
// TODO: Disable action if
//       (d->configOpenImagesInSameWindow && d->document && d->document->isEmpty())
//       as it does nothing if this is true.
void kpMainWindow::slotNew ()
{
    toolEndShape ();

    if (d->document && !d->configOpenImagesInSameWindow)
    {
        // A document -- empty or otherwise -- is open.
        // Force open a new window.  In contrast, open() might not open
        // a new window in this case.
        kpMainWindow *win = new kpMainWindow ();
        //win->setAttribute(Qt::WA_DeleteOnClose);
        win->show ();
    }
    else
    {
        open (QString (), true/*create an empty doc*/);
    }
}

//---------------------------------------------------------------------


// private
QSize kpMainWindow::defaultDocSize () const
{
    // KConfig::readEntry() does not actually reread from disk, hence doesn't
    // realize what other processes have done e.g. Settings / Show Path
    //KGlobal::config ()->reparseConfiguration ();

	QSettings settings;
	settings.beginGroup(kpSettingsGroupGeneral);
	QSize docSize = settings.value (kpSettingLastDocSize, QSize ()).toSize();
    settings.endGroup();

    if (docSize.isEmpty ())
    {
        docSize = QSize (400, 300);
    }
    else
    {
        // Don't get too big or you'll thrash (or even lock up) the computer
        // just by opening a window
        docSize = QSize (qMin (2048, docSize.width ()),
                         qMin (2048, docSize.height ()));
    }

    return docSize;
}

//---------------------------------------------------------------------

// private
void kpMainWindow::saveDefaultDocSize (const QSize &size)
{
#if DEBUG_KP_MAIN_WINDOW
    kDebug () << "\tCONFIG: saving Last Doc Size = " << size;
#endif

	QSettings settings;
	settings.beginGroup(kpSettingsGroupGeneral);
    settings.setValue(kpSettingLastDocSize, size);
    settings.endGroup();
}

//---------------------------------------------------------------------

// private
bool kpMainWindow::shouldOpen ()
{
    if (d->configOpenImagesInSameWindow)
    {
    #if DEBUG_KP_MAIN_WINDOW
        kDebug () << "\topenImagesInSameWindow";
    #endif
        // (this brings up a dialog and might save the current doc)
        if (!queryCloseDocument ())
        {
        #if DEBUG_KP_MAIN_WINDOW
            kDebug () << "\t\tqueryCloseDocument() aborts open";
        #endif
            return false;
        }
    }

    return true;
}

//---------------------------------------------------------------------

// private
void kpMainWindow::setDocumentChoosingWindow (kpDocument *doc)
{
    // Want new window?
    if (d->document && !d->document->isEmpty () &&
        !d->configOpenImagesInSameWindow)
    {
        // Send doc to new window.
        kpMainWindow *win = new kpMainWindow (doc);
        win->show ();
    }
    else
    {
        // (sets up views, doc signals)
        setDocument (doc);
    }
}

//---------------------------------------------------------------------

// private
kpDocument *kpMainWindow::openInternal (const QString &url,
        const QSize &fallbackDocSize,
        bool newDocSameNameIfNotExist)
{
    // If using OpenImagesInSameWindow mode, ask whether to close the
    // current document.
    if (!shouldOpen ())
        return 0;

    // Create/open doc.
    kpDocument *newDoc = new kpDocument (fallbackDocSize.width (),
                                         fallbackDocSize.height (),
                                         documentEnvironment ());
    if (!newDoc->open (url, newDocSameNameIfNotExist))
    {
    #if DEBUG_KP_MAIN_WINDOW
        kDebug () << "\topen failed";
    #endif
        delete newDoc;
        return 0;
    }

#if DEBUG_KP_MAIN_WINDOW
    kDebug () << "\topen OK";
#endif
    // Send document to current or new window.
    setDocumentChoosingWindow (newDoc);

    return newDoc;
}

//---------------------------------------------------------------------

// private
bool kpMainWindow::open (const QString &url, bool newDocSameNameIfNotExist)
{
#if DEBUG_KP_MAIN_WINDOW
    kDebug () << "kpMainWindow::open(" << url
              << ",newDocSameNameIfNotExist=" << newDocSameNameIfNotExist
              << ")" << endl;
#endif

    kpDocument *newDoc = openInternal (url,
                                       defaultDocSize (),
                                       newDocSameNameIfNotExist);

    if (newDoc)
    {
        if (newDoc->isFromURL (false/*don't bother checking exists*/))
            addRecentURL (url);
        return true;
    }
    else
    {
        return false;
    }
}

//---------------------------------------------------------------------

// private
QList<QString> kpMainWindow::askForOpenURLs(const QString &caption, bool /*allowMultipleURLs*/)
{
    QList<QByteArray> mimeTypes = QImageReader::supportedImageFormats();
#if DEBUG_KP_MAIN_WINDOW
    QStringList sortedMimeTypes = mimeTypes;
    sortedMimeTypes.sort ();
    kDebug () << "kpMainWindow::askForURLs(allowMultiple="
               << allowMultipleURLs
               << ")" << endl
               << "\tmimeTypes=" << mimeTypes << endl
               << "\tsortedMimeTypes=" << sortedMimeTypes << endl;
#endif

    QString filter;
    QStringList mimes;
    QString startURL;
    Q_FOREACH(const QByteArray &mime, mimeTypes) {
    	mimes << tr("*.%1 ").arg(QString(mime));
    }
    filter = mimes.join(" ");
    filter.prepend(tr("All Image format ("));
    filter.append(")");

    if (document()) startURL = document()->url();
    if (startURL.isEmpty()) startURL = QDir::homePath();

    QStringList files = QFileDialog::getOpenFileNames(this,
		caption, startURL, filter, 0);


    return files;
}

//---------------------------------------------------------------------

// private slot
void kpMainWindow::slotOpen ()
{
    toolEndShape ();

    const QList<QString> urls = askForOpenURLs(i18n("Open Image"));

    for (QList<QString>::const_iterator it = urls.begin ();
         it != urls.end ();
         ++it)
    {
        open (*it);
    }
}

//---------------------------------------------------------------------

// private slot
void kpMainWindow::slotOpenRecent (const QString &url)
{
#if DEBUG_KP_MAIN_WINDOW
    kDebug () << "kpMainWindow::slotOpenRecent(" << url << ")";
    kDebug () << "\titems=" << d->actionOpenRecent->items ();
#endif

    toolEndShape ();

    open (url);

    // If the open is successful, addRecentURL() would have bubbled up the
    // URL in the File / Open Recent action.  As a side effect, the URL is
    // deselected.
    //
    // If the open fails, we should deselect the URL:
    //
    // 1. for consistency
    //
    // 2. because it has not been opened.
    //
    //d->actionOpenRecent->setCurrentItem (-1);
}

//---------------------------------------------------------------------

void kpMainWindow::slotScreenshot()
{
  toolEndShape();

  KDialog *dialog = new KDialog(this);
  dialog->setButtons(QDialogButtonBox::Ok | QDialogButtonBox::Cancel);

  QLabel *label = new QLabel(i18n("Snapshot Delay"));
  QSpinBox *seconds = new QSpinBox;
  seconds->setRange(0, 99);
  seconds->setSuffix(i18n(" seconds"));
  seconds->setSpecialValueText(i18n("No delay"));

  QCheckBox *hideWindow = new QCheckBox(i18n("Hide Main Window"));
  hideWindow->setChecked(true);

  QVBoxLayout *vbox = new QVBoxLayout(dialog->mainWidget());
  vbox->addWidget(label);
  vbox->addWidget(seconds);
  vbox->addWidget(hideWindow);

  if ( dialog->exec() == KDialog::Rejected )
  {
    delete dialog;
    return;
  }

  if ( hideWindow->isChecked() )
    hide();

  // at least 1 seconds to make sure the window is hidden and the hide effect already stopped
  QTimer::singleShot((seconds->value() + 1) * 1000, this, SLOT(slotMakeScreenshot()));

  delete dialog;
}

//---------------------------------------------------------------------

void kpMainWindow::slotMakeScreenshot()
{
  QCoreApplication::processEvents();
  #if QT_VERSION > 0x050000
  QPixmap pixmap = qApp->primaryScreen()->grabWindow(QApplication::desktop()->winId());
  #else
  QPixmap pixmap = QPixmap::grabWindow(QApplication::desktop()->winId());
  #endif

  kpDocument *doc = new kpDocument(pixmap.width(), pixmap.height(),
                                   documentEnvironment());
  doc->setImage(pixmap.toImage());

  // Send document to current or new window.
  setDocumentChoosingWindow(doc);

  show();  // in case we hid the mainwindow, show it again
}

//---------------------------------------------------------------------

// private slot
void kpMainWindow::slotProperties ()
{
    toolEndShape ();

    kpDocumentMetaInfoDialog dialog (document ()->metaInfo (), this);

    if (dialog.exec () && !dialog.isNoOp ())
    {
        commandHistory ()->addCommand (
            new kpDocumentMetaInfoCommand (
                i18n ("Document Properties"),
                dialog.metaInfo ()/*new*/, *document ()->metaInfo ()/*old*/,
                commandEnvironment ()));
    }
}

//---------------------------------------------------------------------

// private slot
bool kpMainWindow::save (bool localOnly)
{
    if (d->document->url ().isEmpty () )
    {
        return saveAs (localOnly);
    }
    else
    {
        if (d->document->save (false/*no overwrite prompt*/,
                              !d->document->savedAtLeastOnceBefore ()/*lossy prompt*/))
        {
            addRecentURL (d->document->url ());
            return true;
        }
        else
            return false;
    }
}

//---------------------------------------------------------------------

// private slot
bool kpMainWindow::slotSave ()
{
    toolEndShape ();

    return save ();
}

//---------------------------------------------------------------------

// private
QString kpMainWindow::askForSaveURL (const QString &caption,
                                  const QString &startURL,
                                  const kpImage &,
                                  const QString &startSaveOptions,
                                  const kpDocumentMetaInfo &,
                                  const QString &forcedSaveOptionsGroup,
                                  bool , //localOnly,
                                  QString *chosenSaveOptions,
                                  bool isSavingForFirstTime,
                                  bool *allowOverwritePrompt,
                                  bool *allowLossyPrompt)
{
#if DEBUG_KP_MAIN_WINDOW
    kDebug () << "kpMainWindow::askForURL() startURL=" << startURL;
    startSaveOptions.printDebug ("\tstartSaveOptions");
#endif


	QSettings cfg;
	cfg.beginGroup(forcedSaveOptionsGroup);

    if (chosenSaveOptions) *chosenSaveOptions = QString ();

    if (allowOverwritePrompt) *allowOverwritePrompt = true;  // play it safe for now

    if (allowLossyPrompt) *allowLossyPrompt = false; //true;  // play it safe for now

    QString fdSaveOptions = startSaveOptions;

    QList<QByteArray> mimeTypes = QImageWriter::supportedImageFormats(); //KImageIO::mimeTypes (KImageIO::Writing);
#if DEBUG_KP_MAIN_WINDOW
    QStringList sortedMimeTypes = mimeTypes;
    sortedMimeTypes.sort ();
    kDebug () << "\tmimeTypes=" << mimeTypes
               << "\tsortedMimeTypes=" << sortedMimeTypes << endl;
#endif
    if (mimeTypes.isEmpty ())
    {
        qCritical () << "No QImage output mimetypes!" << endl;
        return QString ();
    }

    if (fdSaveOptions.isNull() || !mimeTypes.contains (fdSaveOptions.toLatin1()))
    {
    #if DEBUG_KP_MAIN_WINDOW
        kDebug () << "\tmimeType=" << fdSaveOptions.mimeType ()
                   << " not valid, get default" << endl;
    #endif

        fdSaveOptions = cfg.value (kpSettingForcedMimeType, QString::fromLatin1 ("png")).toString();

        if (!mimeTypes.contains (fdSaveOptions.toLatin1()))
        {
        #if DEBUG_KP_MAIN_WINDOW
            kDebug () << "\tmimeType=" << fdSaveOptions.mimeType ()
                       << " not valid, get hardcoded" << endl;
        #endif
            if (mimeTypes.contains ("png")) fdSaveOptions = "png";
            else if (mimeTypes.contains ("bmp")) fdSaveOptions = "bmp";
            else fdSaveOptions = mimeTypes.first ();
        }
    }


#if DEBUG_KP_MAIN_WINDOW
    fdSaveOptions.printDebug ("\tcorrected saveOptions passed to fileDialog");
#endif

    QStringList mimes;
    QString filter;
    int mimeIndex = -1;
    int i = 0;
    Q_FOREACH(const QByteArray &mime, mimeTypes) {
    	mimes << tr("%1 Image (*.%2)").arg(QString(mime).toUpper()).arg(QString(mime));
    	filter.append(tr("*.%1 ").arg(QString(mime)));
    	if (fdSaveOptions == QString(mime)) mimeIndex = i;
    	++i;
    }
    filter.remove(filter.size() - 1, 1);
    filter.prepend(tr("All Image format ("));
    filter.append(")");
    mimes << filter;
    if (mimeIndex >= 0) filter = mimes.at(mimeIndex); // else filter = filter :)
    mimes.swap(0, mimes.size() - 1);

    QString startDir;
    if (startURL.isEmpty()) {
        QStringList urls = d->actionOpenRecent->urls();
        if (urls.size()) startDir = urls.first().left(urls.first().lastIndexOf("/")); // QDir::fromNativeSeparators
        else {
            #if QT_VERSION > 0x050000
            urls = QStandardPaths::standardLocations(QStandardPaths::DocumentsLocation);
            if (urls.size()) startDir = urls.first();
            //else startDir = QDir::homePath();
            //#else
            //startDir = QDir::homePath(); // "~/Documents"
            #endif
        }
        if (startDir.isEmpty()) startDir = QDir::homePath();
    }
    else startDir = startURL;

    QString fileName = QFileDialog::getSaveFileName(this,
        caption, startDir, mimes.join(";;"), &filter, QFileDialog::DontConfirmOverwrite);

    if ( !fileName.isEmpty() )
    {
    	QString newSaveOptions = fileName.mid(fileName.indexOf('.') + 1);
    #if DEBUG_KP_MAIN_WINDOW
        newSaveOptions.printDebug ("\tnewSaveOptions");
    #endif

        // Save options user forced - probably want to use them in future
        if (fdSaveOptions != newSaveOptions) cfg.setValue(kpSettingForcedMimeType, newSaveOptions);

        if (chosenSaveOptions)
            *chosenSaveOptions = newSaveOptions;

        bool shouldAllowOverwritePrompt =
                (fileName != startURL || newSaveOptions != startSaveOptions);

        if (allowOverwritePrompt)
        {
            *allowOverwritePrompt = shouldAllowOverwritePrompt;
        #if DEBUG_KP_MAIN_WINDOW
            kDebug () << "\tallowOverwritePrompt=" << *allowOverwritePrompt;
        #endif
        }

        if (allowLossyPrompt)
        {
            // SYNC: kpDocumentSaveOptions elements - everything except quality
            //       (one quality setting is "just as lossy" as another so no
            //        need to continually warn due to quality change)
            *allowLossyPrompt =
                (isSavingForFirstTime || shouldAllowOverwritePrompt);
        #if DEBUG_KP_MAIN_WINDOW
            kDebug () << "\tallowLossyPrompt=" << *allowLossyPrompt;
        #endif
        }

    #if DEBUG_KP_MAIN_WINDOW
        kDebug () << "\tselectedUrl=" << fd.selectedUrl ();
    #endif
        return fileName;
    }
    else
        return QString ();
}

//---------------------------------------------------------------------

// private slot
bool kpMainWindow::saveAs (bool localOnly)
{
	QString chosenSaveOptions;
    bool allowOverwritePrompt, allowLossyPrompt;
    QString chosenURL = askForSaveURL (i18n ("Save Image As"),
                                    d->document->url(),
                                    d->document->imageWithSelection (),
                                    *d->document->saveOptions (),
                                    *d->document->metaInfo (),
                                    kpSettingsGroupFileSaveAs,
                                    localOnly,
                                    &chosenSaveOptions,
                                    !d->document->savedAtLeastOnceBefore (),
                                    &allowOverwritePrompt,
                                    &allowLossyPrompt);


    if (chosenURL.isEmpty ())
        return false;


    if (!d->document->saveAs (chosenURL, chosenSaveOptions,
                             allowOverwritePrompt,
                             allowLossyPrompt))
    {
        return false;
    }


    addRecentURL (chosenURL);

    return true;
}

//---------------------------------------------------------------------

// private slot
bool kpMainWindow::slotSaveAs ()
{
    toolEndShape ();

    return saveAs ();
}

//---------------------------------------------------------------------

// private slot
bool kpMainWindow::slotExport ()
{
    toolEndShape ();

    QString chosenSaveOptions;
    bool allowOverwritePrompt, allowLossyPrompt;
    QString chosenURL = askForSaveURL (i18n ("Export"),
                                    d->lastExportURL,
                                    d->document->imageWithSelection (),
                                    d->lastExportSaveOptions,
                                    *d->document->metaInfo (),
                                    kpSettingsGroupFileExport,
                                    false/*allow remote files*/,
                                    &chosenSaveOptions,
                                    d->exportFirstTime,
                                    &allowOverwritePrompt,
                                    &allowLossyPrompt);


    if (chosenURL.isEmpty ())
        return false;

    if (!kpDocument::savePixmapToFile (d->document->imageWithSelection (),
                                       chosenURL,
                                       chosenSaveOptions, *d->document->metaInfo (),
                                       allowOverwritePrompt,
                                       allowLossyPrompt,
                                       this))
    {
        return false;
    }


    addRecentURL (chosenURL);

    d->lastExportURL = chosenURL;
    d->lastExportSaveOptions = chosenSaveOptions;

    d->exportFirstTime = false;

    return true;
}

//---------------------------------------------------------------------

// private slot
void kpMainWindow::slotEnableReload ()
{
    d->actionReload->setEnabled ((d->document) && (!d->document->url().isEmpty())); // iPaint modified!!
}

//---------------------------------------------------------------------

// private slot
bool kpMainWindow::slotReload ()
{
    toolEndShape ();

    Q_ASSERT (d->document);


    QString oldURL = d->document->url ();


    if (d->document->isModified ())
    {
        int result = QMessageBox::Cancel;

        if (d->document->isFromURL (false/*don't bother checking exists*/) && !oldURL.isEmpty ())
        {
            result = QMessageBox::warning (this, tr("Reload"),
                         i18n ("The document \n\"%1\"\nhas been modified,"
                               "reloading will lose all changes since you last saved it.\n\n"
                        	   "Are you sure?\n",
                               d->document->prettyFilename ()),
                               tr("&Reload"), tr("&Cancel"), QString(), 1, 1);
        }
        else
        {
        	result = QMessageBox::warning (this, tr("Reload"),
                         i18n ("The document \n\"%1\"\nhas been modified,"
                               "reloading will lose all changes.\n\n"
                        	   "Are you sure?\n",
                               d->document->prettyFilename ()),
                               tr("&Reload"), tr("&Cancel"), QString(), 1, 1);
        }

        if (result == 1) return false;
    }


    kpDocument *doc = 0;

    // If it's _supposed to_ come from a URL or it exists
    if (d->document->isFromURL (false/*don't bother checking exists*/) ||
        (!oldURL.isEmpty() && QFile::exists (oldURL)))
    {
    #if DEBUG_KP_MAIN_WINDOW
        kDebug () << "kpMainWindow::slotReload() reloading from disk!";
    #endif

        doc = new kpDocument (1, 1, documentEnvironment ());
        if (!doc->open (oldURL))
        {
            delete doc; doc = 0;
            return false;
        }

        addRecentURL (oldURL);
    }
    else
    {
    #if DEBUG_KP_MAIN_WINDOW
        kDebug () << "kpMainWindow::slotReload() create doc";
    #endif

        doc = new kpDocument (d->document->constructorWidth (),
                              d->document->constructorHeight (),
                              documentEnvironment ());
        doc->setURL (oldURL, false/*not from URL*/);
    }


    setDocument (doc);

    return true;
}


// private
void kpMainWindow::sendDocumentNameToPrinter (QPrinter *printer)
{
    QString url = d->document->url ();
    if (!url.isEmpty ())
    {
        int dot;

        QString fileName = url;
        dot = fileName.lastIndexOf ('.');

        // file.ext but not .hidden-file?
        if (dot > 0)
            fileName.truncate (dot);

    #if DEBUG_KP_MAIN_WINDOW
        kDebug () << "kpMainWindow::sendDocumentNameToPrinter() fileName="
                   << fileName
                   << " dir="
                   << url.directory ()
                   << endl;
    #endif
        printer->setDocName (fileName);
    }
}


// private
void kpMainWindow::sendImageToPrinter (QPrinter *printer,
        bool showPrinterSetupDialog)
{
    // Get image to be printed.
    kpImage image = d->document->imageWithSelection ();


    // Get image DPI.
    double imageDotsPerMeterX =
        double (d->document->metaInfo ()->dotsPerMeterX ());
    double imageDotsPerMeterY =
        double (d->document->metaInfo ()->dotsPerMeterY ());
#if DEBUG_KP_MAIN_WINDOW
    kDebug () << "kpMainWindow::sendImageToPrinter() image:"
               << " width=" << image.width ()
               << " height=" << image.height ()
               << " dotsPerMeterX=" << imageDotsPerMeterX
               << " dotsPerMeterY=" << imageDotsPerMeterY
               << endl;
#endif

    // Image DPI invalid (e.g. new image, could not read from file
    // or Qt3 doesn't implement DPI for JPEG)?
    if (imageDotsPerMeterX <= 0 || imageDotsPerMeterY <= 0)
    {
        // Even if just one DPI dimension is invalid, mutate both DPI
        // dimensions as we have no information about the intended
        // aspect ratio anyway (and other dimension likely to be invalid).

        // When rendering text onto a document, the fonts are rasterised
        // according to the screen's DPI.
        // TODO: I think we should use the image's DPI.  Technically
        //       possible?
        //
        //       So no matter what computer you draw text on, you get
        //       the same pixels.
        //
        // So we must print at the screen's DPI to get the right text size.
        //
        // Unfortunately, this means that moving to a different screen DPI
        // affects printing.  If you edited the image at a different screen
        // DPI than when you print, you get incorrect results.  Furthermore,
        // this is bogus if you don't have text in your image.  Worse still,
        // what if you have multiple screens connected to the same computer
        // with different DPIs?
        // TODO: mysteriously, someone else is setting this to 96dpi always.
        QPixmap arbitraryScreenElement(1, 1);
        const QPaintDevice *screenDevice = &arbitraryScreenElement;
        const int dpiX = screenDevice->logicalDpiX (),
            dpiY = screenDevice->logicalDpiY ();
    #if DEBUG_KP_MAIN_WINDOW
        kDebug () << "\tusing screen dpi: x=" << dpiX << " y=" << dpiY;
    #endif

        imageDotsPerMeterX = dpiX * KP_INCHES_PER_METER;
        imageDotsPerMeterY = dpiY * KP_INCHES_PER_METER;
    }


    // Get page size (excluding margins).
    // Coordinate (0,0) is the X here:
    //     mmmmm
    //     mX  m
    //     m   m       m = margin
    //     m   m
    //     mmmmm
    const int printerWidthMM = printer->widthMM ();
    const int printerHeightMM = printer->heightMM ();
#if DEBUG_KP_MAIN_WINDOW
    kDebug () << "\tprinter: widthMM=" << printerWidthMM
               << " heightMM=" << printerHeightMM
               << endl;
#endif


    double dpiX = imageDotsPerMeterX / KP_INCHES_PER_METER;
    double dpiY = imageDotsPerMeterY / KP_INCHES_PER_METER;
#if DEBUG_KP_MAIN_WINDOW
    kDebug () << "\timage: dpiX=" << dpiX << " dpiY=" << dpiY;
#endif


    //
    // If image doesn't fit on page at intended DPI, change the DPI.
    //

    const double scaleDpiX =
        (image.width () / (printerWidthMM / KP_MILLIMETERS_PER_INCH))
            / dpiX;
    const double scaleDpiY =
        (image.height () / (printerHeightMM / KP_MILLIMETERS_PER_INCH))
            / dpiY;
    const double scaleDpi = qMax (scaleDpiX, scaleDpiY);
#if DEBUG_KP_MAIN_WINDOW
    kDebug () << "\t\tscaleDpi: x=" << scaleDpiX << " y=" << scaleDpiY
               << " --> scale at " << scaleDpi << " to fit?"
               << endl;
#endif

    // Need to increase resolution to fit page?
    if (scaleDpi > 1.0)
    {
        dpiX *= scaleDpi;
        dpiY *= scaleDpi;
    #if DEBUG_KP_MAIN_WINDOW
        kDebug () << "\t\t\tto fit page, scaled to:"
                   << " dpiX=" << dpiX << " dpiY=" << dpiY << endl;
    #endif
    }


    // Make sure DPIs are equal as that's all QPrinter::setResolution()
    // supports.  We do this in such a way that we only ever stretch an
    // image, to avoid losing information.  Don't antialias as the printer
    // will do that to translate our DPI to its physical resolution and
    // double-antialiasing looks bad.
    if (dpiX > dpiY)
    {
    #if DEBUG_KP_MAIN_WINDOW
        kDebug () << "\tdpiX > dpiY; stretching image height to equalise DPIs to dpiX="
                   << dpiX << endl;
    #endif
        kpPixmapFX::scale (&image,
             image.width (),
             qMax (1, qRound (image.height () * dpiX / dpiY)),
             false/*don't antialias*/);

        dpiY = dpiX;
    }
    else if (dpiY > dpiX)
    {
    #if DEBUG_KP_MAIN_WINDOW
        kDebug () << "\tdpiY > dpiX; stretching image width to equalise DPIs to dpiY="
                   << dpiY << endl;
    #endif
        kpPixmapFX::scale (&image,
             qMax (1, qRound (image.width () * dpiY / dpiX)),
             image.height (),
             false/*don't antialias*/);

        dpiX = dpiY;
    }

    Q_ASSERT (dpiX == dpiY);


    // QPrinter::setResolution() has to be called before QPrinter::setup().
    printer->setResolution (qMax (1, qRound (dpiX)));


    sendDocumentNameToPrinter (printer);


    if (showPrinterSetupDialog)
    {
        kpPrintDialogPage *optionsPage = new kpPrintDialogPage (this);
        optionsPage->setPrintImageCenteredOnPage (d->configPrintImageCenteredOnPage);

        QPrintDialog *printDialog = new QPrintDialog(printer, this);
        printDialog->setWindowTitle (i18n ("Print Image"));
        printDialog->setOptionTabs(QList <QWidget *> () << optionsPage);

        // Display dialog.
        const bool wantToPrint = printDialog->exec ();

        if (optionsPage->printImageCenteredOnPage () !=
            d->configPrintImageCenteredOnPage)
        {
            // Save config option even if the dialog was cancelled.
            d->configPrintImageCenteredOnPage = optionsPage->printImageCenteredOnPage ();

        	QSettings settings;
        	settings.beginGroup(kpSettingsGroupGeneral);
        	settings.setValue(kpSettingPrintImageCenteredOnPage, d->configPrintImageCenteredOnPage);
        	settings.endGroup();
        }

        delete printDialog;

        if (!wantToPrint)
            return;
    }


    double originX = 0, originY = 0;

    // Center image on page?
    if (d->configPrintImageCenteredOnPage)
    {
        originX =
            (printerWidthMM * dpiX / KP_MILLIMETERS_PER_INCH - image.width ())
                / 2;
        originY =
            (printerHeightMM * dpiY / KP_MILLIMETERS_PER_INCH - image.height ())
                / 2;
    }

#if DEBUG_KP_MAIN_WINDOW
    kDebug () << "\torigin: x=" << originX << " y=" << originY;
#endif


    // Send image to printer.
    QPainter painter;
    painter.begin (printer);
    painter.drawImage (qRound (originX), qRound (originY), image);
    painter.end ();
}

//---------------------------------------------------------------------

// private slot
void kpMainWindow::slotPrintPreviewRequest(QPrinter * printer) {

	sendImageToPrinter(printer, false);
}

//---------------------------------------------------------------------

// private slot
void kpMainWindow::slotPrint ()
{
    toolEndShape ();

    QPrinter printer;

    sendImageToPrinter (&printer, true/*showPrinterSetupDialog*/);
}

//---------------------------------------------------------------------

// private slot
void kpMainWindow::slotPrintPreview ()
{
    toolEndShape ();

    QPrinter printer;

    QPrintPreviewDialog printPreview (&printer);

    connect(&printPreview, SIGNAL(paintRequested(QPrinter * )), this, SLOT(slotPrintPreviewRequest(QPrinter * )));

    //sendImageToPrinter (&printer, false/*don't showPrinterSetupDialog*/);

    printPreview.exec ();
}

//---------------------------------------------------------------------

// private
bool kpMainWindow::queryCloseDocument ()
{
    toolEndShape ();

    if (!d->document || !d->document->isModified ())
        return true;  // ok to close current doc

    int result = QMessageBox::question (this, QString(),
                     i18n ("The document \"%1\" has been modified.\n\n"
                           "Do you want to save it?",
                           d->document->prettyFilename ()),
                     QMessageBox::Yes | QMessageBox::No | QMessageBox::Cancel);

    switch (result)
    {
    case QMessageBox::Yes:
        return slotSave ();  // close only if save succeeds
    case QMessageBox::No:
        return true;  // close without saving
    default:
        return false;  // don't close current doc
    }
}

//---------------------------------------------------------------------

// private virtual [base QWidget]

void kpMainWindow::closeEvent(QCloseEvent *event) {

    if (queryClose()) {
    	saveSettings ();
        event->accept();
    } else {
        event->ignore();
    }
}

bool kpMainWindow::queryClose ()
{
#if DEBUG_KP_MAIN_WINDOW
    kDebug () << "kpMainWindow::queryClose()";
#endif
    toolEndShape ();

    if (!queryCloseDocument ())
        return false;

    if (!queryCloseColors ())
        return false;

    return true;
}

//---------------------------------------------------------------------

// private slot
void kpMainWindow::slotClose ()
{
    toolEndShape ();

    if (!queryCloseDocument ())
        return;

    setDocument (0);
}

//---------------------------------------------------------------------

// private slot
void kpMainWindow::slotQuit ()
{
    toolEndShape ();

    close ();  // will call queryClose()
}

//---------------------------------------------------------------------

// private slot
void kpMainWindow::slotAbout ()
{
    toolEndShape ();

    kpAboutDialog dialog(this);

    dialog.exec();
}

//---------------------------------------------------------------------

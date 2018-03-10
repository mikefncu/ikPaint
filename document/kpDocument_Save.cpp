
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


#define DEBUG_KP_DOCUMENT 0


#include <kpDocument.h>
#include <kpDocumentPrivate.h>

#include <math.h>

#include <qcolor.h>
#include <qbitmap.h>
#include <qbrush.h>
#include <qfile.h>
#include <qimage.h>
#include <qlist.h>
#include <QImage>
#include <qpainter.h>
#include <qrect.h>
#include <qsize.h>
#include <qmatrix.h>

#include <qdebug.h>
#include <qglobal.h>

#include <qlocale.h>
#include <qmessagebox.h>

#include <QFile>
#include <qtemporaryfile.h>
#include <tools.h>

#include <kpColor.h>
#include <kpColorToolBar.h>
#include <kpDefs.h>
#include <kpDocumentEnvironment.h>
#include <kpDocumentMetaInfo.h>
#include <kpEffectReduceColors.h>
#include <kpPixmapFX.h>
#include <kpTool.h>
#include <kpToolToolBar.h>
#include <kpUrlFormatter.h>
#include <kpViewManager.h>


bool kpDocument::save (bool overwritePrompt, bool lossyPrompt)
{
#if DEBUG_KP_DOCUMENT
    kDebug () << "kpDocument::save("
               << "overwritePrompt=" << overwritePrompt
               << ",lossyPrompt=" << lossyPrompt
               << ") url=" << m_url
               << " savedAtLeastOnceBefore=" << savedAtLeastOnceBefore ()
               << endl;
#endif

    // TODO: check feels weak
    if (m_url.isEmpty () || m_saveExt->isEmpty ())
    {
        QMessageBox::critical (d->environ->dialogParent (),
            i18n ("Could not save image - insufficient information."),
            i18n ("URL: %1\n"
                  "Mimetype: %2",
                  prettyUrl (),
                  m_saveExt->isEmpty () ?
                          i18n ("<empty>") :
                          *m_saveExt),
            i18n ("Internal Error"));
        return false;
    }

    return saveAs (m_url, *m_saveExt,
                   overwritePrompt,
                   lossyPrompt);
}

//---------------------------------------------------------------------

// public static
bool kpDocument::savePixmapToDevice (const QImage &image,
                                     QIODevice *device,
                                     const QString &saveExt,
                                     const kpDocumentMetaInfo &metaInfo,
                                     bool ,
                                     QWidget *,
                                     bool *userCancelled)
{
    if (userCancelled)
        *userCancelled = false;

    QStringList types = QStringList() << saveExt; //saveOptions(); //"png"; typeForMime
#if DEBUG_KP_DOCUMENT
    kDebug () << "\ttypes=" << types;
#endif
    if (types.isEmpty ())
        return false;

    // It's safe to arbitrarily choose the 0th type as any type in the list
    // should invoke the same KImageIO image loader.
    const QString type = types [0];

#if DEBUG_KP_DOCUMENT
    kDebug () << "\tmimeType=" << saveOptions.mimeType ()
               << " type=" << type << endl;
#endif


    //
    // Reduce colors if required
    //

#if DEBUG_KP_DOCUMENT
    kDebug () << "\tuseSaveOptionsColorDepth=" << useSaveOptionsColorDepth
              << "current image depth=" << image.depth ()
              << "save options depth=" << saveOptions.colorDepth ();
#endif
    QImage imageToSave(image);


    //
    // Write Meta Info
    //

    imageToSave.setDotsPerMeterX (metaInfo.dotsPerMeterX ());
    imageToSave.setDotsPerMeterY (metaInfo.dotsPerMeterY ());
    imageToSave.setOffset (metaInfo.offset ());

    QList <QString> keyList = metaInfo.textKeys ();
    for (QList <QString>::const_iterator it = keyList.constBegin ();
         it != keyList.constEnd ();
         ++it)
    {
        imageToSave.setText (*it, metaInfo.text (*it));
    }

    //
    // Save at required quality
    //

    int quality = -1;  // default


#if DEBUG_KP_DOCUMENT
    kDebug () << "\tsaving";
#endif
    if (!imageToSave.save (device, type.toLatin1 (), quality))
    {
    #if DEBUG_KP_DOCUMENT
        kDebug () << "\tQImage::save() returned false";
    #endif
        return false;
    }


#if DEBUG_KP_DOCUMENT
    kDebug () << "\tsave OK";
#endif
    return true;
}

//---------------------------------------------------------------------

static void CouldNotCreateTemporaryFileDialog (QWidget *parent)
{
    QMessageBox::critical (parent, "Error",
                        i18n ("Could not save image - unable to create temporary file."));
}

//---------------------------------------------------------------------

static void CouldNotSaveDialog (const QString &url, QWidget *parent)
{
    // TODO: use file.errorString()
    QMessageBox::critical (parent, "Error",
                        i18n ("Could not save as \"%1\".",
                              kpUrlFormatter::PrettyFilename (url)));
}

//---------------------------------------------------------------------

// public static
bool kpDocument::savePixmapToFile (const QImage &pixmap,
                                   const QString &url,
                                   const QString &saveExt,
                                   const kpDocumentMetaInfo &metaInfo,
                                   bool overwritePrompt,
                                   bool ,
                                   QWidget *parent)
{
    // TODO: Use KIO::NetAccess:mostLocalURL() for accessing home:/ (and other
    //       such local URLs) for efficiency and because only local writes
    //       are atomic.
#if DEBUG_KP_DOCUMENT
    kDebug () << "kpDocument::savePixmapToFile ("
               << url
               << ",overwritePrompt=" << overwritePrompt
               << ",lossyPrompt=" << lossyPrompt
               << ")" << endl;
    saveOptions.printDebug (QLatin1String ("\tsaveOptions"));
    metaInfo.printDebug (QLatin1String ("\tmetaInfo"));
#endif

    if (overwritePrompt && QFile::exists (url))
    {
        int result = QMessageBox::question (parent,
			i18n ("Overwrite"),
            i18n ("A document called:\n\n \"%1\" already exists.\n\n"
                  "Do you want to overwrite it?",
                  kpUrlFormatter::PrettyFilename (url)),
            QMessageBox::Save | QMessageBox::Cancel
            );

        if (result != QMessageBox::Save)
        {
        #if DEBUG_KP_DOCUMENT
            kDebug () << "\tuser doesn't want to overwrite";
        #endif

            return false;
        }
    }

	const QString & filename = url;

	QFile atomicFileWriter (filename);
	{
		if (!atomicFileWriter.open (QIODevice::WriteOnly))
		{

		#if DEBUG_KP_DOCUMENT
			kDebug () << "\treturning false because could not open KSaveFile"
					  << " error=" << atomicFileWriter.error () << endl;
		#endif
			::CouldNotCreateTemporaryFileDialog (parent);
			return false;
		}

		// Write to local temporary file.
		if (!savePixmapToDevice (pixmap, &atomicFileWriter,
								 saveExt, metaInfo,
								 false/*no lossy prompt*/,
								 parent))
		{
			atomicFileWriter.close();

		#if DEBUG_KP_DOCUMENT
			kDebug () << "\treturning false because could not save pixmap to device"
					  << endl;
		#endif
			::CouldNotSaveDialog (url, parent);
			return false;
		}

		atomicFileWriter.close();

	}

    return true;
}

//---------------------------------------------------------------------

bool kpDocument::saveAs (const QString &url,
						 const QString &saveExt,
                         bool overwritePrompt,
                         bool lossyPrompt)
{
#if DEBUG_KP_DOCUMENT
    kDebug () << "kpDocument::saveAs (" << url << ","
               << saveOptions.mimeType () << ")" << endl;
#endif

    if (kpDocument::savePixmapToFile (imageWithSelection (),
                                      url,
                                      saveExt, *metaInfo (),
                                      overwritePrompt,
                                      lossyPrompt,
                                      d->environ->dialogParent ()))
    {
        setURL (url, true/*is from url*/);
        *m_saveExt = saveExt;
        m_modified = false;

        m_savedAtLeastOnceBefore = true;

        emit documentSaved ();
        return true;
    }
    else
    {
        return false;
    }
}

//---------------------------------------------------------------------

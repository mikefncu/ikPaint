
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
#include <qimage.h>

#include <qdebug.h>
#include <qglobal.h>

#include <qimagereader.h>
#include <qlocale.h>
#include <qmessagebox.h>

#include <tools.h>

#include <kpColor.h>
#include <kpColorToolBar.h>
#include <kpDefs.h>
#include <kpDocumentEnvironment.h>

#include <kpDocumentMetaInfo.h>
#include <kpEffectReduceColors.h>
#include <kpPixmapFX.h>
#include <kpTool.h>
#include <kpUrlFormatter.h>
#include <kpViewManager.h>

//---------------------------------------------------------------------

void kpDocument::getDataFromImage(const QImage &image,
                                  kpDocumentMetaInfo &metaInfo)
{
  metaInfo.setDotsPerMeterX(image.dotsPerMeterX());
  metaInfo.setDotsPerMeterY(image.dotsPerMeterY());
  metaInfo.setOffset(image.offset());

  QStringList keys = image.textKeys();
  for (int i = 0; i < keys.count(); i++)
    metaInfo.setText(keys[i], image.text(keys[i]));
}

//---------------------------------------------------------------------

// public static
QImage kpDocument::getPixmapFromFile(const QString &url, bool suppressDoesntExistDialog,
                                     QWidget *parent,
                                     QString *saveOptions,
                                     kpDocumentMetaInfo *metaInfo)
{
#if DEBUG_KP_DOCUMENT
    kDebug () << "kpDocument::getPixmapFromFile(" << url << "," << parent << ")";
#endif

    if (saveOptions)
        *saveOptions = QString ();

    if (metaInfo)
        *metaInfo = kpDocumentMetaInfo ();

    if (url.isEmpty ())
    {
        if (!suppressDoesntExistDialog)
        {
            // TODO: Use "Cannot" instead of "Could not" in all dialogs in KolourPaint.
            //       Or at least choose one consistently.
            //
            // TODO: Have captions for all dialogs in KolourPaint.
            QMessageBox::critical (parent, "Sorry",
                                i18n ("Could not open \"%1\".", kpUrlFormatter::PrettyFilename (url)));
        }

        return QImage ();
    }

    QString detectedMimeType = QString(QImageReader::imageFormat (url));

    QImage image;
    if (!detectedMimeType.isEmpty())
    {

    	if (saveOptions)
            *saveOptions = detectedMimeType;

    #if DEBUG_KP_DOCUMENT
        kDebug () << "\ttempFile=" << tempFile;
        kDebug () << "\tmimetype=" << detectedMimeType;
        kDebug () << "\tsrc=" << url.path ();
    #endif

        image = QImage (url);
    }
    else {
        // TODO: <detectedMimeType> might be different.
        //       Should we feed it into QImage to solve this problem?
        //
        //       If so, should we have used KMimeType::findByContent()
        //       instead?  Are some image types not detectable by findByContent()
        //       (e.g. image types that are only detected by extension)?
        //
        //       Currently, opening a PNG with a ".jpg" extension does not
        //       work -- QImage and findByUrl() both think it's a JPG based
        //       on the extension, but findByContent() correctly detects
        //       it as a PNG.
        QMessageBox::critical (parent, "Sorry",
                            i18n ("Could not open \"%1\" - unknown mimetype.",
                                kpUrlFormatter::PrettyFilename (url)));

        return QImage ();
    }

    if (image.isNull ())
    {
        QMessageBox::critical (parent, "Sorry",
                            i18n ("Could not open \"%1\" - unsupported image format.\n"
                                  "The file may be corrupt.",
                                  kpUrlFormatter::PrettyFilename (url)));
        return QImage ();
    }

#if DEBUG_KP_DOCUMENT
    kDebug () << "\tpixmap: depth=" << image.depth ()
                << " hasAlphaChannel=" << image.hasAlphaChannel ()
                << endl;
#endif

    if (metaInfo )
      getDataFromImage(image, *metaInfo);

    // make sure we always have Format_ARGB32_Premultiplied as this is the fastest to draw on
    // and Qt can not draw onto Format_Indexed8 (Qt-4.7)
    if ( image.format() != QImage::Format_ARGB32_Premultiplied )
      image = image.convertToFormat(QImage::Format_ARGB32_Premultiplied);

    return image;
}

//---------------------------------------------------------------------

void kpDocument::openNew (const QString &url)
{
#if DEBUG_KP_DOCUMENT
    kDebug () << "kpDocument::openNew (" << url << ")";
#endif

    m_image->fill(QColor(Qt::white).rgb());

    setURL (url, false/*not from url*/);
    // TODO: Maybe we should guess the mimetype from "url"'s filename
    //       extension.
    //
    //       That way "kolourpaint doesnotexist.bmp" would automatically
    //       select the BMP file format when the save dialog comes up for
    //       the first time.
    *m_saveExt = QString ();
    *m_metaInfo = kpDocumentMetaInfo ();
    m_modified = false;

    emit documentOpened ();
}

//---------------------------------------------------------------------

bool kpDocument::open (const QString &url, bool newDocSameNameIfNotExist)
{
#if DEBUG_KP_DOCUMENT
    kDebug () << "kpDocument::open (" << url << ")";
#endif

    QString newSaveExt;
    kpDocumentMetaInfo newMetaInfo;
    QImage newPixmap = kpDocument::getPixmapFromFile (url,
        newDocSameNameIfNotExist/*suppress "doesn't exist" dialog*/,
        d->environ->dialogParent (),
        &newSaveExt,
        &newMetaInfo);

    if (!newPixmap.isNull ())
    {
        delete m_image;
        m_image = new kpImage (newPixmap);

        setURL (url, true/*is from url*/);
        *m_saveExt = newSaveExt;
        *m_metaInfo = newMetaInfo;
        m_modified = false;

        emit documentOpened ();
        return true;
    }

    if (newDocSameNameIfNotExist)
    {
        if (!url.isEmpty ()) //&&
            // not just a permission error?
            //!KIO::NetAccess::exists (url, KIO::NetAccess::SourceSide/*open*/, d->environ->dialogParent ()))
        {
            openNew (url);
        }
        else
        {
            openNew (QString ());
        }

        return true;
    }
    else
    {
        return false;
    }
}

//---------------------------------------------------------------------

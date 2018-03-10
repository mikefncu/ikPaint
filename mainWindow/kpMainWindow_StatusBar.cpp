/*
   Copyright (c) 2003-2007 Clarence Dang <dang@kde.org>
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

#define DEBUG_STATUS_BAR (DEBUG_KP_MAIN_WINDOW && 0)


#include <kpMainWindow.h>
#include <kpMainWindowPrivate.h>

#include <qlabel.h>
#include <qstring.h>

#include <qdebug.h>
#include <qlocale.h>
#include <qstatusbar.h>
#include <qfontmetrics.h>
#include <tools.h>

#include <kpDefs.h>
#include <kpDocument.h>
#include <kpTool.h>
#include <kpViewManager.h>
#include <kpViewScrollableContainer.h>
#include <kpZoomedView.h>
#include <kpStatusBarLabels.h>

//---------------------------------------------------------------------

// private
void kpMainWindow::createStatusBar ()
{
    QStatusBar *sb = statusBar ();

    d->statusBarMessageLabel = new QLabel(sb);
    // this is done to have the same height as the other labels in status bar; done like in kstatusbar.cpp
    d->statusBarMessageLabel->setFixedHeight(d->statusBarMessageLabel->fontMetrics().height() + 2);
    sb->addWidget(d->statusBarMessageLabel, 1/*stretch*/);

    d->statusLabels = new kpStatusBarLabels(sb);

    QFontMetrics fm(font());
    QString numSample;

    numSample = QString(" #FFFFFFFF ");
    d->statusLabels->addLabel (StatusBarItemColor, fm.width(numSample));

    numSample = QString(" 8888,8888 - 8888,8888 ");
    d->statusLabels->addLabel (StatusBarItemShapePoints, fm.width(numSample));

    numSample = QString(" -8888 x -8888 ");
    d->statusLabels->addLabel (StatusBarItemShapeSize, fm.width(numSample));

    numSample = i18n(" %1 x %2 ", QString::number(5000), QString::number(5000));  // localized string; can e.g. be "5 000"
    d->statusLabels->addLabel(StatusBarItemDocSize, fm.width(numSample));

    numSample = QString(" XXbpp ");
    d->statusLabels->addLabel(StatusBarItemDocDepth, fm.width(numSample));

    numSample = QString(" 1600% ");
    d->statusLabels->addLabel (StatusBarItemZoom,fm.width(numSample));

    sb->addPermanentWidget(d->statusLabels);

    d->statusBarShapeLastPointsInitialised = false;
    d->statusBarShapeLastSizeInitialised = false;
    d->statusBarCreated = true;
}

//---------------------------------------------------------------------

// private slot
void kpMainWindow::setStatusBarMessage (const QString &message)
{
#if DEBUG_STATUS_BAR && 1
    kDebug () << "kpMainWindow::setStatusBarMessage("
               << message
               << ") ok=" << d->statusBarCreated
               << endl;
#endif

    if (!d->statusBarCreated)
        return;

    d->statusBarMessageLabel->setText (message);
}

//---------------------------------------------------------------------

// private slot
void kpMainWindow::setStatusBarShapePoints (const QPoint &startPoint,
                                            const QPoint &endPoint)
{
#if DEBUG_STATUS_BAR && 0
    kDebug () << "kpMainWindow::setStatusBarShapePoints("
               << startPoint << "," << endPoint
               << ") ok=" << d->statusBarCreated
               << endl;
#endif

    if (!d->statusBarCreated)
        return;

    if (d->statusBarShapeLastPointsInitialised &&
        startPoint == d->statusBarShapeLastStartPoint &&
        endPoint == d->statusBarShapeLastEndPoint)
    {
    #if DEBUG_STATUS_BAR && 0
        kDebug () << "\tNOP";
    #endif
        return;
    }


    if (startPoint == KP_INVALID_POINT)
    {
        d->statusLabels->setText(StatusBarItemShapePoints, QString());
    }
    else if (endPoint == KP_INVALID_POINT)
    {
        d->statusLabels->setText(StatusBarItemShapePoints, i18n ("%1,%2",
                            QString::number(startPoint.x ()),
                            QString::number(startPoint.y ())));
    }
    else
    {
        d->statusLabels->setText(StatusBarItemShapePoints, tr  ("%1,%2 - %3,%4")
                            .arg(startPoint.x ())
                            .arg(startPoint.y ())
                            .arg(endPoint.x ())
                            .arg(endPoint.y ()));
    }


    d->statusBarShapeLastStartPoint = startPoint;
    d->statusBarShapeLastEndPoint = endPoint;
    d->statusBarShapeLastPointsInitialised = true;
}

//---------------------------------------------------------------------

// private slot
void kpMainWindow::setStatusBarShapeSize (const QSize &size)
{
#if DEBUG_STATUS_BAR && 0
    kDebug () << "kpMainWindow::setStatusBarShapeSize("
               << size
               << ") ok=" << d->statusBarCreated
               << endl;
#endif

    if (!d->statusBarCreated)
        return;

    if (d->statusBarShapeLastSizeInitialised &&
        size == d->statusBarShapeLastSize)
    {
    #if DEBUG_STATUS_BAR && 0
        kDebug () << "\tNOP";
    #endif
        return;
    }

    if (size == KP_INVALID_SIZE)
    {
        d->statusLabels->setText(StatusBarItemShapeSize, QString());
    }
    else
    {
        d->statusLabels->setText(StatusBarItemShapeSize, tr ("%1x%2")
                                 .arg(size.width ())
                                 .arg(size.height ()));
    }

    d->statusBarShapeLastSize = size;
    d->statusBarShapeLastSizeInitialised = true;
}

//---------------------------------------------------------------------

// private slot
void kpMainWindow::setStatusBarDocSize (const QSize &size)
{
#if DEBUG_STATUS_BAR && 0
    kDebug () << "kpMainWindow::setStatusBarDocSize("
               << size
               << ") ok=" << d->statusBarCreated
               << endl;
#endif

    if (!d->statusBarCreated)
        return;

    if (size == KP_INVALID_SIZE)
    {
        d->statusLabels->setText(StatusBarItemDocSize, QString());
    }
    else
    {
        d->statusLabels->setText(StatusBarItemDocSize, tr ("%1 x %2")
                                 .arg(size.width ())
                                 .arg(size.height ()));
    }
}

//---------------------------------------------------------------------

// private slot
void kpMainWindow::setStatusBarDocDepth (int depth)
{
#if DEBUG_STATUS_BAR && 0
    kDebug () << "kpMainWindow::setStatusBarDocDepth("
               << depth
               << ") ok=" << d->statusBarCreated
               << endl;
#endif

    if (!d->statusBarCreated)
        return;

    if (depth <= 0)
    {
        d->statusLabels->setText(StatusBarItemDocDepth, QString());
    }
    else
    {
        d->statusLabels->setText(StatusBarItemDocDepth, tr ("%1bpp").arg(depth));
    }
}

//---------------------------------------------------------------------

// private slot
void kpMainWindow::setStatusBarZoom (int zoom)
{
#if DEBUG_STATUS_BAR && 0
    kDebug () << "kpMainWindow::setStatusBarZoom("
               << zoom
               << ") ok=" << d->statusBarCreated
               << endl;
#endif

    if (!d->statusBarCreated)
        return;

    if (zoom <= 0)
    {
        d->statusLabels->setText(StatusBarItemZoom, QString());
    }
    else
    {
        d->statusLabels->setText(StatusBarItemZoom, tr ("%1%").arg(zoom));
    }
}

//---------------------------------------------------------------------

void kpMainWindow::setStatusBarPixelColor (const QColor &color) {

    if (!d->statusBarCreated)
        return;

    if (color.isValid()) {
        #if QT_VERSION > 0x050400
        d->statusLabels->setText(StatusBarItemColor, color.name(QColor::HexArgb).toUpper());
        #else
        d->statusLabels->setText(StatusBarItemColor, color.name() + QString("#%1").arg(QString::number(color.alpha(), 16).toUpper() ));
        #endif
    }
    else d->statusLabels->setText(StatusBarItemColor, QString());
}

//---------------------------------------------------------------------

void kpMainWindow::recalculateStatusBarMessage ()
{
#if DEBUG_STATUS_BAR && 1
    kDebug () << "kpMainWindow::recalculateStatusBarMessage()";
#endif
    QString scrollViewMessage = d->scrollView->statusMessage ();
#if DEBUG_STATUS_BAR && 1
    kDebug () << "\tscrollViewMessage=" << scrollViewMessage;
    kDebug () << "\tresizing doc? " << !d->scrollView->newDocSize ().isEmpty ()
               << endl;
    kDebug () << "\tviewUnderCursor? "
               << (d->viewManager && d->viewManager->viewUnderCursor ())
               << endl;
#endif

    // HACK: To work around kpViewScrollableContainer's unreliable
    //       status messages (which in turn is due to Qt not updating
    //       QWidget::underMouse() on drags and we needing to hack around it)
    if (!scrollViewMessage.isEmpty () &&
        d->scrollView->newDocSize ().isEmpty () &&
        d->viewManager && d->viewManager->viewUnderCursor ())
    {
    #if DEBUG_STATUS_BAR && 1
        kDebug () << "\t\tnot resizing & viewUnderCursor - message is wrong - clearing"
                   << endl;
    #endif
        d->scrollView->blockSignals (true);
        d->scrollView->clearStatusMessage ();
        d->scrollView->blockSignals (false);

        scrollViewMessage.clear ();
    #if DEBUG_STATUS_BAR && 1
        kDebug () << "\t\t\tdone";
    #endif
    }

    if (!scrollViewMessage.isEmpty ())
    {
        setStatusBarMessage (scrollViewMessage);
    }
    else
    {
        const kpTool *t = tool ();
        if (t)
        {
            setStatusBarMessage (t->userMessage ());
        }
        else
        {
            setStatusBarMessage ();
        }
    }
}

//---------------------------------------------------------------------

// private slot
void kpMainWindow::recalculateStatusBarShape ()
{
#if DEBUG_STATUS_BAR && 0
    kDebug () << "kpMainWindow::recalculateStatusBarShape()";
#endif

    QSize docResizeTo = d->scrollView->newDocSize ();
#if DEBUG_STATUS_BAR && 0
    kDebug () << "\tdocResizeTo=" << docResizeTo;
#endif
    if (docResizeTo.isValid ())
    {
        const QPoint startPoint (d->document->width (), d->document->height ());
    #if DEBUG_STATUS_BAR && 0
        kDebug () << "\thavedMovedFromOrgSize="
                   << d->scrollView->haveMovedFromOriginalDocSize () << endl;
    #endif
        if (!d->scrollView->haveMovedFromOriginalDocSize ())
        {
            setStatusBarShapePoints (startPoint);
            setStatusBarShapeSize ();
        }
        else
        {
            const int newWidth = docResizeTo.width ();
            const int newHeight = docResizeTo.height ();

            setStatusBarShapePoints (startPoint, QPoint (newWidth, newHeight));
            const QPoint sizeAsPoint (QPoint (newWidth, newHeight) - startPoint);
            setStatusBarShapeSize (QSize (sizeAsPoint.x (), sizeAsPoint.y ()));
        }
    }
    else
    {
        const kpTool *t = tool ();
    #if DEBUG_STATUS_BAR && 0
        kDebug () << "\ttool=" << t;
    #endif
        if (t)
        {
            setStatusBarShapePoints (t->userShapeStartPoint (),
                                     t->userShapeEndPoint ());
            setStatusBarShapeSize (t->userShapeSize ());

            QRgb c;
            QPoint p = t->userShapeEndPoint ();
            if (p == KP_INVALID_POINT) p = t->userShapeStartPoint ();

            if (p != KP_INVALID_POINT && document()->imagePointer()->rect().contains(p)) {
                c = document()->imagePointer()->pixel(p);
                setStatusBarPixelColor(QColor(qRed(c), qGreen(c), qBlue(c), qAlpha(c)));
            }
            else setStatusBarPixelColor();

        }
        else
        {
            setStatusBarShapePoints ();
            setStatusBarShapeSize ();
            setStatusBarPixelColor();
        }
    }
}

//---------------------------------------------------------------------

// private slot
void kpMainWindow::recalculateStatusBar ()
{
#if DEBUG_STATUS_BAR && 1
    kDebug () << "kpMainWindow::recalculateStatusBar() ok="
               << d->statusBarCreated
               << endl;
#endif

    if (!d->statusBarCreated)
        return;

    recalculateStatusBarMessage ();
    recalculateStatusBarShape ();

    if (d->document)
    {
        setStatusBarDocSize (QSize (d->document->width (), d->document->height ()));
        setStatusBarDocDepth (d->document->image ().depth ());
    }
    else
    {
        setStatusBarDocSize ();
        setStatusBarDocDepth ();
    }

    if (d->mainView)
    {
        setStatusBarZoom (d->mainView->zoomLevelX ());
    }
    else
    {
        setStatusBarZoom ();
    }
}

//---------------------------------------------------------------------

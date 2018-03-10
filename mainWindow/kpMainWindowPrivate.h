
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


#ifndef kpMainWindowPrivate_H
#define kpMainWindowPrivate_H


#define DEBUG_KP_MAIN_WINDOW 0


class QAction;
class QActionGroup;
class QLabel;

class KSelectAction;
class KRecentFilesAction;

class kpCommandEnvironment;
class kpDocumentEnvironment;
class kpToolSelectionEnvironment;
class kpTransformDialogEnvironment;
class kpStatusBarLabels;

struct kpMainWindowPrivate
{
    kpMainWindowPrivate ()
    {
    }

    bool isFullyConstructed;

    kpViewScrollableContainer *scrollView;
    kpZoomedView *mainView;
    kpDocument *document;
    kpViewManager *viewManager;
    kpColorToolBar *colorToolBar;
    kpToolToolBar *toolToolBar;
    kpCommandHistory *commandHistory;

    bool configFirstTime;
    bool configShowGrid;
    bool configShowPath;

    bool configThumbnailShown;
    QRect configThumbnailGeometry;
    bool configZoomedThumbnail;

    kpDocumentEnvironment *documentEnvironment;
    kpCommandEnvironment *commandEnvironment;

    //
    // ActionCollection
    //
    KActionCollection * ac;

    //
    // Tools
    //

    kpToolSelectionEnvironment *toolSelectionEnvironment;
    QActionGroup *toolsActionGroup;

    kpTool *toolSpraycan, *toolBrush,
           *toolColorEraser, *toolColorPicker,
           *toolCurve, *toolEllipse,
           *toolEllipticalSelection, *toolEraser,
           *toolFloodFill, *toolFreeFormSelection,
           *toolLine, *toolPen, *toolPolygon,
           *toolPolyline, *toolRectangle, *toolRectSelection,
           *toolRoundedRectangle, *toolZoom;
    kpToolText *toolText;

    QList <kpTool *> tools;
    int lastToolNumber;

    bool toolActionsEnabled;
    QAction *actionPrevToolOptionGroup1,
            *actionNextToolOptionGroup1,
            *actionPrevToolOptionGroup2,
            *actionNextToolOptionGroup2;

    int settingImageSelectionTransparency;

    int docResizeWidth, docResizeHeight;
    bool docResizeToBeCompleted;


    //
    // File Menu
    //

    bool configOpenImagesInSameWindow, configPrintImageCenteredOnPage;

    QAction *actionNew, *actionOpen;
    KRecentFilesAction *actionOpenRecent;
    QAction *actionScreenshot, *actionProperties,
            *actionSave, *actionSaveAs, *actionExport,
            *actionReload,
            *actionPrint, *actionPrintPreview,
            *actionClose, *actionQuit;

    QString lastExportURL;
    QString lastExportSaveOptions;
    bool exportFirstTime;


    //
    // Edit Menu
    //

    bool editMenuDocumentActionsEnabled;

    QAction *actionUndo, *actionRedo,
            *actionCut, *actionCopy,
            *actionPaste, *actionPasteInNewWindow,
            *actionDelete,
            *actionSelectAll, *actionDeselect,
            *actionCopyToFile, *actionPasteFromFile;

    QString lastCopyToURL;
    QString lastCopyToSaveOptions;
    bool copyToFirstTime;


    //
    // View Menu
    //

    bool viewMenuDocumentActionsEnabled;

    QAction *actionActualSize,
            *actionFitToPage, *actionFitToWidth, *actionFitToHeight,
            *actionZoomIn, *actionZoomOut;

    KSelectAction *actionZoom;

    QList <int> zoomList;

    QTimer *thumbnailSaveConfigTimer;

    //
    // Image Menu
    //

    kpTransformDialogEnvironment *transformDialogEnvironment;

    bool imageMenuDocumentActionsEnabled;

    QAction *actionResizeScale,
            *actionCrop, *actionAutoCrop,
            *actionFlip, *actionMirror,
            *actionRotate, *actionRotateLeft, *actionRotateRight,
            *actionSkew,
            *actionConvertToBlackAndWhite, *actionConvertToGrayscale,
            *actionMoreEffects,
            *actionInvertColors, *actionClear;

    // Implemented in kpMainWindow_Tools.cpp, not kpImageWindow_Image.cpp
    // since they're really setting tool options.
    QAction *actionDrawOpaque;
    QAction *actionDrawColorSimilarity;

    int moreEffectsDialogLastEffect;

    //
    // Colors Menu
    //

    bool colorMenuDocumentActionsEnabled;

    QAction *actionColorsDefault;

    QAction *actionColorsReload;
    QAction *actionColorsSave;

    QAction *actionColorsAppendColumn;
    QAction *actionColorsDeleteColumn;


    //
    // Settings Menu
    //

    QAction *actionFullScreen, *actionShowPath;

    //
    // Status Bar
    //

    bool statusBarCreated;
    QLabel *statusBarMessageLabel;
    kpStatusBarLabels *statusLabels;

    bool statusBarShapeLastPointsInitialised;
    QPoint statusBarShapeLastStartPoint, statusBarShapeLastEndPoint;
    bool statusBarShapeLastSizeInitialised;
    QSize statusBarShapeLastSize;


    //
    // Text ToolBar
    //

    QToolBar *toolBarText;
    KFontAction *actionTextFontFamily;
    KFontSizeAction *actionTextFontSize;

    QAction *actionTextBold, *actionTextItalic, *actionTextUnderline, *actionTextStrikeThru;

    int settingTextStyle;
    QString textOldFontFamily;
    int textOldFontSize;
};


#endif  // kpMainWindowPrivate_H

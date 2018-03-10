
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


#define DEBUG_KP_EFFECTS_DIALOG 0


#include <kpEffectsDialog.h>

#include <qgroupbox.h>
#include <qlabel.h>
#include <qlayout.h>
#include <qtimer.h>
#include <qimage.h>

#include <qapplication.h>
#include <qcombobox.h>
#include <qconfig.h>
#include <qdebug.h>
#include <qboxlayout.h>
#include <qlocale.h>
#include <tools.h>

#include <kpDefs.h>
#include <kpDocument.h>
#include <kpEffectBalanceWidget.h>
#include <kpEffectBlurSharpenWidget.h>
#include <kpEffectEmbossWidget.h>
#include <kpEffectFlattenWidget.h>
#include <kpEffectHSVWidget.h>
#include <kpEffectInvertWidget.h>
#include <kpEffectReduceColorsWidget.h>
#include <kpEffectToneEnhanceWidget.h>
#include <kpEffectColorToAlphaWidget.h>
#include <kpPixmapFX.h>
#include <kpTransformDialogEnvironment.h>


// protected static
int kpEffectsDialog::s_lastWidth = 640;
int kpEffectsDialog::s_lastHeight = 620;


kpEffectsDialog::kpEffectsDialog (bool actOnSelection,
                                  kpTransformDialogEnvironment *_env,
                                  QWidget *parent,
                                  int defaultSelectedEffect)
    : kpTransformPreviewDialog (kpTransformPreviewDialog::Preview,
                           true/*reserve top row*/,
                           QString()/*caption*/,
                           QString()/*afterActionText (no Dimensions Group Box)*/,
                           actOnSelection,
                           _env,
                           parent),
      m_delayedUpdateTimer (new QTimer (this)),
      m_effectsComboBox (0),
      m_settingsGroupBox (0),
      m_settingsLayout (0),
      m_effectWidget (0)
{
#if DEBUG_KP_EFFECTS_DIALOG
    kDebug () << "kpEffectsDialog::kpEffectsDialog()";
#endif
    const bool e = updatesEnabled ();
    setUpdatesEnabled (false);


    if (actOnSelection)
        setWindowTitle (i18n ("More Image Effects (Selection)"));
    else
        setWindowTitle (i18n ("More Image Effects"));


    m_delayedUpdateTimer->setSingleShot (true);
    connect (m_delayedUpdateTimer, SIGNAL (timeout ()),
             this, SLOT (slotUpdateWithWaitCursor ()));

    QHBoxLayout *effectContainer = new QHBoxLayout();
    effectContainer->setSpacing (KDialog::spacingHint () * 4
                                 /*need more space for QGroupBox titles*/);
    effectContainer->setMargin (0);

    QLabel *label = new QLabel (i18n ("&Effect:"));
    effectContainer->addWidget(label);

    m_effectsComboBox = new QComboBox ();
    // Keep in alphabetical order.
    // TODO: What about translations?
    // sync: order in selectEffect().

    // iPaint: require blitz (qimageblitz.h)
    m_effectsComboBox->addItem (i18n ("Color to Alpha"), 8);
    m_effectsComboBox->addItem (i18n ("Balance"), 0);
    //m_effectsComboBox->addItem (i18n ("Emboss"), 1);
    //m_effectsComboBox->addItem (i18n ("Flatten"), 2);
    m_effectsComboBox->addItem (i18n ("Histogram Equalizer"), 3);
    m_effectsComboBox->addItem (i18n ("Hue, Saturation, Value"), 4);
    m_effectsComboBox->addItem (i18n ("Invert"), 5);
    m_effectsComboBox->addItem (i18n ("Reduce Colors"), 6);
    //m_effectsComboBox->addItem (i18n ("Soften & Sharpen"), 7);

    label->setBuddy (m_effectsComboBox);
    effectContainer->addWidget(m_effectsComboBox, 1);

    QWidget * wid = new QWidget();//mainWidget());
    wid->setLayout(effectContainer);
    addCustomWidgetToFront (wid);

    m_settingsGroupBox = new QGroupBox (mainWidget ());
    m_settingsLayout = new QVBoxLayout ( m_settingsGroupBox );
    m_settingsLayout->setMargin( KDialog::marginHint () * 2 );
    m_settingsLayout->setSpacing( KDialog::spacingHint () );
    addCustomWidgetToBack (m_settingsGroupBox);


    connect (m_effectsComboBox, SIGNAL (activated (int)),
             this, SLOT (selectEffect (int)));
    selectEffect (defaultSelectedEffect);


    resize (s_lastWidth, s_lastHeight);


#if DEBUG_KP_EFFECTS_DIALOG
    kDebug () << "about to setUpdatesEnabled()";
#endif
    // OPT: The preview pixmap gets recalculated here and then possibly
    //      again when QResizeEvent fires, when the dialog is shown.
    setUpdatesEnabled (e);
#if DEBUG_KP_EFFECTS_DIALOG
    kDebug () << endl
              << endl
              << endl;
#endif
}

kpEffectsDialog::~kpEffectsDialog ()
{
    s_lastWidth = width ();
    s_lastHeight = height ();
}


// public virtual [base kpTransformPreviewDialog]
bool kpEffectsDialog::isNoOp () const
{
    if (!m_effectWidget)
        return true;

    return m_effectWidget->isNoOp ();
}

// public
kpEffectCommandBase *kpEffectsDialog::createCommand () const
{
    if (!m_effectWidget)
        return 0;

    return m_effectWidget->createCommand (m_environ->commandEnvironment ());
}


// protected virtual [base kpTransformPreviewDialog]
QSize kpEffectsDialog::newDimensions () const
{
    kpDocument *doc = document ();
    if (!doc)
        return QSize ();

    return QSize (doc->width (m_actOnSelection),
                  doc->height (m_actOnSelection));
}

// protected virtual [base kpTransformPreviewDialog]
QImage kpEffectsDialog::transformPixmap (const QImage &pixmap,
                                         int targetWidth, int targetHeight) const
{
    QImage pixmapWithEffect;

    if (m_effectWidget && !m_effectWidget->isNoOp ())
        pixmapWithEffect = m_effectWidget->applyEffect (pixmap);
    else
        pixmapWithEffect = pixmap;

    return kpPixmapFX::scale (pixmapWithEffect, targetWidth, targetHeight);
}


// public
int kpEffectsDialog::selectedEffect () const
{
    return m_effectsComboBox->currentIndex ();
}

// public slot
void kpEffectsDialog::selectEffect (int which)
{
#if DEBUG_KP_EFFECTS_DIALOG
    kDebug () << "kpEffectsDialog::selectEffect(" << which << ")";
#endif

    if (which < 0 ||
        which >= m_effectsComboBox->count ())
    {
        return;
    }

    if (which != m_effectsComboBox->currentIndex ())
        m_effectsComboBox->setCurrentIndex (which);


    delete m_effectWidget;
    m_effectWidget = 0;


    m_settingsGroupBox->setWindowTitle(QString());

#define CREATE_EFFECT_WIDGET(name)  \
    m_effectWidget = new name (m_actOnSelection, m_settingsGroupBox)
    // sync: order in constructor.

    int whichselector = m_effectsComboBox->itemData(which).toInt();
    switch (whichselector)
    {
    case 0:
        CREATE_EFFECT_WIDGET (kpEffectBalanceWidget);
        break;

    case 1:
        CREATE_EFFECT_WIDGET (kpEffectEmbossWidget);
        break;

    case 2:
        CREATE_EFFECT_WIDGET (kpEffectFlattenWidget);
        break;
    
    case 3:
        CREATE_EFFECT_WIDGET (kpEffectToneEnhanceWidget);
        break;

    case 4:
        CREATE_EFFECT_WIDGET (kpEffectHSVWidget);
        break;

    case 5:
        CREATE_EFFECT_WIDGET (kpEffectInvertWidget);
        break;

    case 6:
        CREATE_EFFECT_WIDGET (kpEffectReduceColorsWidget);
        break;

    case 7:
        CREATE_EFFECT_WIDGET (kpEffectBlurSharpenWidget);
        break;

    case 8:
        CREATE_EFFECT_WIDGET (kpEffectColorToAlphaWidget);
        break;
    }
#undef CREATE_EFFECT_WIDGET


    if (m_effectWidget)
    {
        const bool e = updatesEnabled ();
        setUpdatesEnabled (false);

    #if DEBUG_KP_EFFECTS_DIALOG
        kDebug () << "widget exists for effect #";
    #endif
        m_settingsGroupBox->setTitle (m_effectWidget->caption ());

        // Show widget.
        //
        // Don't resize the whole dialog when doing this.
        // This seems to work magically without any extra code with Qt4.
    #if DEBUG_KP_EFFECTS_DIALOG
        kDebug () << "addWidget";
    #endif
        m_settingsLayout->addWidget (m_effectWidget);
    #if DEBUG_KP_EFFECTS_DIALOG
        kDebug () << "show widget";
    #endif
        m_effectWidget->show ();

        connect (m_effectWidget, SIGNAL (settingsChangedNoWaitCursor ()),
                 this, SLOT (slotUpdate ()));
        connect (m_effectWidget, SIGNAL (settingsChanged ()),
                 this, SLOT (slotUpdateWithWaitCursor ()));
        connect (m_effectWidget, SIGNAL (settingsChangedDelayed ()),
                 this, SLOT (slotDelayedUpdate ()));

    #if DEBUG_KP_EFFECTS_DIALOG
        kDebug () << "about to setUpdatesEnabled()";
    #endif
        setUpdatesEnabled (e);
    }


#if DEBUG_KP_EFFECTS_DIALOG
    kDebug () << "done"
              << endl
              << endl
              << endl;
#endif
}


// protected slot virtual [base kpTransformPreviewDialog]
void kpEffectsDialog::slotUpdate ()
{
#if DEBUG_KP_EFFECTS_DIALOG
    kDebug () << "kpEffectsDialog::slotUpdate()"
               << " timerActive=" << m_delayedUpdateTimer->isActive ()
               << endl;
#endif

    m_delayedUpdateTimer->stop ();

    kpTransformPreviewDialog::slotUpdate ();
}

// protected slot virtual [base kpTransformPreviewDialog]
void kpEffectsDialog::slotUpdateWithWaitCursor ()
{
#if DEBUG_KP_EFFECTS_DIALOG
    kDebug () << "kpEffectsDialog::slotUpdateWithWaitCursor()"
               << " timerActive=" << m_delayedUpdateTimer->isActive ()
               << endl;
#endif

    m_delayedUpdateTimer->stop ();

    kpTransformPreviewDialog::slotUpdateWithWaitCursor ();
}


// protected slot
void kpEffectsDialog::slotDelayedUpdate ()
{
#if DEBUG_KP_EFFECTS_DIALOG
    kDebug () << "kpEffectsDialog::slotDelayedUpdate()"
               << " timerActive=" << m_delayedUpdateTimer->isActive ()
               << endl;
#endif
    m_delayedUpdateTimer->stop ();

    // (single shot)
    m_delayedUpdateTimer->start (400/*ms*/);
}


/* This file is part of the KDE libraries
    Copyright (C) 1999 Reginald Stadlbauer <reggie@kde.org>
              (C) 1999 Simon Hausmann <hausmann@kde.org>
              (C) 2000 Nicolas Hadacek <haadcek@kde.org>
              (C) 2000 Kurt Granroth <granroth@kde.org>
              (C) 2000 Michael Koch <koch@kde.org>
              (C) 2001 Holger Freyther <freyther@kde.org>
              (C) 2002 Ellis Whitehead <ellis@kde.org>
              (C) 2005-2006 Hamish Rodda <rodda@kde.org>

    Modified by Maikel Diaz <ariguanabosoft@gmail.com>
    Copyright (c) 2015-2018

    This library is free software; you can redistribute it and/or
    modify it under the terms of the GNU Library General Public
    License version 2 as published by the Free Software Foundation.

    This library is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
    Library General Public License for more details.

    You should have received a copy of the GNU Library General Public License
    along with this library; see the file COPYING.LIB.  If not, write to
    the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
    Boston, MA 02110-1301, USA.
*/

#ifndef KACTION_H
#define KACTION_H

#include <QWidgetAction>

class QIcon;

//TODO Reduce the word count. This is not very focused and takes too long to read.
//Keep in mind that QAction also has documentation that we don't need to repeat here.
/**
 * @short Class to encapsulate user-driven action or event
 *
 * The KAction class (and derived and super classes) extends QAction,
 * which provides a way to easily encapsulate a "real" user-selected
 * action or event in your program.
 *
 * For instance, a user may want to @p paste the contents of
 * the clipboard, @p scroll @p down a document, or @p quit the
 * application.  These are all \b actions -- events that the
 * user causes to happen.  The KAction class allows the developer to
 * deal with these actions in an easy and intuitive manner, and conforms
 * to KDE's extended functionality requirements - including supporting
 * multiple user-configurable shortcuts, and KDE named icons.  Actions
 * also improve accessibility.
 *
 * Specifically, QAction (and thus KAction) encapsulates the various attributes
 * of an event/action.  For instance, an action might have an icon()
 * that provides a visual representation (a clipboard for a "paste" action or
 * scissors for a "cut" action).  The action should also be described by some text().
 * It will certainly be connected to a method that actually @p executes the action!
 * All these attributes are contained within the action object.
 *
 * The advantage of dealing with actions is that you can manipulate
 * the Action without regard to the GUI representation of it.  For
 * instance, in the "normal" way of dealing with actions like "cut",
 * you would manually insert a item for Cut into a menu and a button
 * into a toolbar.  If you want to disable the cut action for a moment
 * (maybe nothing is selected), you would have to hunt down the pointer
 * to the menu item and the toolbar button and disable both
 * individually.  Setting the menu item and toolbar item up uses very
 * similar code - but has to be done twice!
 *
 * With the action concept, you simply add the action to whatever
 * GUI element you want.  The KAction class will then take care of
 * correctly defining the menu item (with icons, accelerators, text,
 * etc), toolbar button, or other.  From then on, if you
 * manipulate the action at all, the effect will propagate through all
 * GUI representations of it.  Back to the "cut" example: if you want
 * to disable the Cut Action, you would simply call
 * 'cutAction->setEnabled(false)' and both the menuitem and button would
 * instantly be disabled!
 *
 * This is the biggest advantage to the action concept -- there is a
 * one-to-one relationship between the "real" action and @p all
 * GUI representations of it.
 *
 * KAction emits the hovered() signal on mouseover, and the triggered(bool checked)
 * signal on activation of a corresponding GUI element ( menu item, toolbar button, etc. )
 *
 * If you are in the situation of wanting to map the triggered()
 * signal of multiple action objects to one slot, with a special
 * argument bound to each action, you have several options:
 *
 * Using QActionGroup:
 * \li Create a QActionGroup and assign it to each of the actions with setActionGroup(), then
 * \li Connect the QActionGroup::triggered(QAction*) signal to your slot.
 *
 * Using QSignalMapper:
 * \code
 * QSignalMapper *desktopNumberMapper = new QSignalMapper( this );
 * connect( desktopNumberMapper, SIGNAL( mapped( int ) ),
 *          this, SLOT( moveWindowToDesktop( int ) ) );
 *
 * for ( uint i = 0; i < numberOfDesktops; ++i ) {
 *     KAction *desktopAction = new KAction( i18n( "Move Window to Desktop %i" ).arg( i ), ... );
 *     connect( desktopAction, SIGNAL( triggered(bool) ), desktopNumberMapper, SLOT( map() ) );
 *     desktopNumberMapper->setMapping( desktopAction, i );
 * }
 * \endcode
 *
 * \section kaction_general General Usage
 *
 * The steps to using actions are roughly as follows:
 *
 * @li Decide which attributes you want to associate with a given
 *     action (icons, text, keyboard shortcut, etc)
 * @li Create the action using KAction (or derived or super class).
 * @li Add the action into whatever GUI element you want.  Typically,
 *      this will be a menu or toolbar.
 *
 * \section kaction_general The kinds of shortcuts
 *
 * Local shortcuts are active if their context has the focus, global shortcus
 * are active even if the program does not have the focus. If a global
 * shortcut and a local shortcut are ambiguous the global shortcut wins.
 *
 * @li Active shortcuts trigger a KAction if activated.
 * @li Default shortcuts are what the active shortcuts revert to if the user chooses
 * to reset shortcuts to default.
 *
 * \section kaction_example Detailed Example
 *
 * Here is an example of enabling a "New [document]" action
 * \code
 * KAction *newAct = actionCollection()->addAction(
 *              KStandardAction::New,   //< see KStandardAction
 *              this,                   //< Receiver
 *              SLOT(fileNew())  );     //< SLOT
 * \endcode
 *
 * This section creates our action. Text, Icon and Shortcut will be set from
 * KStandardAction. KStandardAction ensures your application complies to the
 * platform standards. When triggered the \c fileNew() slot will be called.
 *
 * @see KStandardAction for more information. 
 *
 * If you want to create your own actions use
 * \code
 * KAction *newAct = actionCollection()->addAction("quick-connect");
 * newAct->setText(i18n("Quick Connect"))
 * newAct->setIcon(KIcon("quick-connect"));
 * newAct->setShortcut(Qt::Key_F6);
 * connect(newAct, SIGNAL(triggered()), this, SLOT(quickConnect()));
 * \endcode
 *
 * This section creates our action. It displays the text "Quick Connect",
 * uses the Icon "quick-connect" and pressing \c F6 will trigger it. When
 * invoked, the slot quickConnect() is called.
 *
 * \code
 * QMenu *file = new QMenu;
 * file->addAction(newAct);
 * \endcode
 * That just inserted the action into the File menu.  The point is, it's not
 * important in which menu it is: all manipulation of the item is
 * done through the newAct object.
 *
 * \code
 * toolBar()->addAction(newAct);
 * \endcode
 * And this added the action into the main toolbar as a button.
 *
 * That's it!
 *
 * If you want to disable that action sometime later, you can do so
 * with
 * \code
 * newAct->setEnabled(false)
 * \endcode
 * and both the menuitem in File and the toolbar button will instantly
 * be disabled.
 *
 * Unlike with previous versions of KDE, the action can simply be deleted
 * when you have finished with it - the destructor takes care of all
 * of the cleanup.
 *
 * \warning calling QAction::setShortcut() on a KAction may lead to unexpected
 * behavior. There is nothing we can do about it because QAction::setShortcut()
 * is not virtual.
 *
 * \note if you are using a "standard" action like "new", "paste",
 * "quit", or any other action described in the KDE UI Standards,
 * please use the methods in the KStandardAction class rather than
 * defining your own.
 *
 * \section Using QActions
 *
 * Mixing QActions and KActions in an application is not a
 * good idea. KShortcutsEditor doesn't handle QActions at all.
 *
 * \section kaction_xmlgui Usage Within the XML Framework
 *
 * If you are using KAction within the context of the XML menu and
 * toolbar building framework, you do not ever
 * have to add your actions to containers manually.  The framework
 * does that for you.
 *
 * @see KStandardAction
 */

#include <kaction_p.h>

class KAction : public QWidgetAction
{
  Q_OBJECT

public:

    /**
     * Constructs an action.
     */
    explicit KAction(QObject *parent);

    /**
     * Constructs an action with the specified parent and visible text.
     *
     * @param text The visible text for this action.
     * @param parent The parent for this action.
     */
    KAction(const QString& text, QObject *parent);

    /**
     * Constructs an action with text and icon; a shortcut may be specified by
     * the ampersand character (e.g. \"&Option\" creates a shortcut with key \e O )
     *
     * This is the other common KAction constructor used.  Use it when you
     * \e do have a corresponding icon.
     *
     * @param icon The icon to display.
     * @param text The text that will be displayed.
     * @param parent The parent for this action.
     */
    KAction(const QIcon& icon, const QString& text, QObject *parent);

    /**
     * Standard destructor
     */
    virtual ~KAction();

    /**
     * @reimp
     */
    bool event(QEvent*);


Q_SIGNALS:

    /**
     * Emitted when the action is triggered. Also provides the state of the
     * keyboard modifiers and mouse buttons at the time.
     */
    void triggered(Qt::MouseButtons buttons, Qt::KeyboardModifiers modifiers);

private:

    friend class KActionCollectionPrivate; // Needs access to the component

    Q_PRIVATE_SLOT(d, void slotTriggered())
    class KActionPrivate* const d;
    friend class KActionPrivate;

};

#endif

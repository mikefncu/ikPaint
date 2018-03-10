/* This file was modified from the original file: kselectaction.cpp
 * from KDE project:

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

#ifndef KSELECTACTION_H
#define KSELECTACTION_H

#include <qtoolbutton.h>
#include <qcombobox.h>
#include <qmenu.h>

#include <kaction.h>

class KSelectActionPrivate;
/**
 *  @short Action for selecting one of several items
 *
 *  Action for selecting one of several items.
 *
 *  This action shows up a submenu with a list of items.
 *  One of them can be checked. If the user clicks on an item
 *  this item will automatically be checked,
 *  the formerly checked item becomes unchecked.
 *  There can be only one item checked at a time.
 */
class KSelectAction : public KAction
{
    Q_OBJECT
    Q_ENUMS( ToolbarMode )
    Q_DECLARE_PRIVATE(KSelectAction)

public:
    /**
     * Constructs a selection action with the specified parent.
     *
     * @param parent The action's parent object.
     */
    explicit KSelectAction(QObject *parent);

    /**
     * Constructs a selection action with text; a shortcut may be specified by
     * the ampersand character (e.g. "&Option" creates a shortcut with key \e O )
     *
     * This is the most common KSelectAction used when you do not have a
     * corresponding icon (note that it won't appear in the current version
     * of the "Edit ToolBar" dialog, because an action needs an icon to be
     * plugged in a toolbar...).
     *
     * @param text The text that will be displayed.
     * @param parent The action's parent object.
     */
    KSelectAction(const QString& text, QObject *parent);

    /**
     * Constructs a selection action with text and an icon; a shortcut may be specified by
     * the ampersand character (e.g. "&Option" creates a shortcut with key \e O )
     *
     * This is the other common KSelectAction used.  Use it when you
     * \e do have a corresponding icon.
     *
     * @param icon The icon to display.
     * @param text The text that will be displayed.
     * @param parent The action's parent object.
     */
    KSelectAction(const QIcon& icon, const QString& text, QObject *parent);

    /**
     * Destructor
     */
    virtual ~KSelectAction();

    enum ToolBarMode {
      /// Creates a button which pops up a menu when interacted with, as defined by toolButtonPopupMode().
      MenuMode,
      /// Creates a combo box which contains the actions.
      /// This is the default.
      ComboBoxMode
    };

    /**
     * Returns which type of widget (combo box or button with drop-down menu) will be inserted
     * in a toolbar.
     */
    ToolBarMode toolBarMode() const;

    /**
     * Set the type of widget to be inserted in a toolbar to \a mode.
     */
    void setToolBarMode(ToolBarMode mode);

    /**
     * Returns the style for the list of actions, when this action is plugged
     * into a KToolBar. The default value is QToolButton::InstantPopup
     *
     * \sa QToolButton::setPopupMode()
     */
    QToolButton::ToolButtonPopupMode toolButtonPopupMode() const;

    /**
     * Set how this list of actions should behave when in popup mode and plugged into a toolbar.
     */
    void setToolButtonPopupMode(QToolButton::ToolButtonPopupMode mode);

    /**
     * The action group used to create exclusivity between the actions associated with this action.
     */
    QActionGroup* selectableActionGroup() const;

    /**
     * Returns the current QAction.
     * @see setCurrentAction
     */
    QAction* currentAction() const;

    /**
     * Returns the index of the current item.
     *
     * @sa currentItem and currentAction
     */
    int currentItem() const;

    /**
     * Returns the text of the currently selected item.
     *
     * @sa currentItem and currentAction
     */
    QString currentText() const;

    /**
     * Returns the list of selectable actions
     */
    QList<QAction*> actions() const;

    /**
     * Returns the action at \a index, if one exists.
     */
    QAction* action(int index) const;

    /**
     * Searches for an action with the specified \a text, using a search whose
     * case sensitivity is defined by \a cs.
     */
    QAction* action(const QString& text, Qt::CaseSensitivity cs = Qt::CaseSensitive) const;

    /**
     * Sets the currently checked item.
     *
     * @param action the QAction to become the currently checked item.
     *
     * \return \e true if a corresponding action was found and successfully checked.
     */
    bool setCurrentAction(QAction* action);

    /**
     * \overload setCurrentAction(QAction*)
     *
     * Convenience function to set the currently checked action to be the action
     * at index \p index.
     *
     * If there is no action at that index, the currently checked action (if any) will
     * be deselected.
     *
     * \return \e true if a corresponding action was found and thus set to the current action, otherwise \e false
     */
    bool setCurrentItem(int index);

    /**
     * \overload setCurrentAction(QAction*)
     *
     * Convenience function to set the currently checked action to be the action
     * which has \p text as its text().
     *
     * If there is no action at that index, the currently checked action (if any) will
     * be deselected.
     *
     * \return \e true if a corresponding action was found, otherwise \e false
     */
    bool setCurrentAction(const QString& text, Qt::CaseSensitivity cs = Qt::CaseSensitive);

    /**
     * Add \a action to the list of selectable actions.
     */
    virtual void addAction(QAction* action);

    /**
     * \overload addAction(QAction* action)
     *
     * Convenience function which creates an action from \a text and inserts it into
     * the list of selectable actions.
     *
     * The newly created action is checkable and not user configurable.
     */
    KAction* addAction(const QString& text);

    /**
     * \overload addAction(QAction* action)
     *
     * Convenience function which creates an action from \a text and \a icon and inserts it into
     * the list of selectable actions.
     *
     * The newly created action is checkable and not user configurable.
     */
    KAction* addAction(const QIcon& icon, const QString& text);

    /**
     * Remove the specified \a action from this action selector.
     *
     * You take ownership here, so save or delete it in order to not leak the action.
     */
    virtual QAction* removeAction(QAction* action);

    /**
     * Convenience function to create the list of selectable items.
     * Any previously existing items will be cleared.
     */
    void setItems( const QStringList &lst );

    /**
     * Convenience function which returns the items that can be selected with this action.
     * It is the same as iterating selectableActionGroup()->actions() and looking at each
     * action's text().
     */
    QStringList items() const;

    /**
     * When this action is plugged into a toolbar, it creates a combobox.
     * @return true if the combo editable.
     */
    bool isEditable() const;

    /**
     * When this action is plugged into a toolbar, it creates a combobox.
     * This makes the combo editable or read-only.
     */
    void setEditable( bool );

    /**
     * When this action is plugged into a toolbar, it creates a combobox.
     * This returns the maximum width set by setComboWidth
     */
    int comboWidth() const;

    /**
     * When this action is plugged into a toolbar, it creates a combobox.
     * This gives a _maximum_ size to the combobox.
     * The minimum size is automatically given by the contents (the items).
     */
    void setComboWidth( int width );

    /**
     * Sets the maximum items that are visible at once if the action
     * is a combobox, that is the number of items in the combobox's viewport
     */
    void setMaxComboViewCount( int n );

    /**
     * Clears up all the items in this action.
     * \warning The actions will be deleted for backwards compatibility with KDE3.
     *          If you just want to remove all actions, use removeAllActions()
     */
    void clear();

    void removeAllActions();

    /**
     * Sets whether any occurrence of the ampersand character ( & ) in items
     * should be interpreted as keyboard accelerator for items displayed in a
     * menu or not.  Only applies to (overloaded) methods dealing with QStrings,
     * not those dealing with QActions.
     *
     * Defaults to true.
     *
     * \param b true if ampersands indicate a keyboard accelerator, otherwise false.
     */
    void setMenuAccelsEnabled( bool b );

    /**
     * Returns whether ampersands passed to methods using QStrings are interpreted
     * as keyboard accelerator indicators or as literal ampersands.
     */
    bool menuAccelsEnabled() const;

    /**
     * Changes the text of item @param index to @param text .
     */
    void changeItem( int index, const QString& text );

Q_SIGNALS:
    /**
     * This signal is emitted when an item is selected; @param action
     * indicates the item selected.
     */
    void triggered( QAction* action );

    /**
     * This signal is emitted when an item is selected; @param index indicates
     * the item selected.
     */
    void triggered( int index );

    /**
     * This signal is emitted when an item is selected; @param text indicates
     * the item selected.
     */
    void triggered( const QString& text );

protected Q_SLOTS:
    /**
     * This function is called whenever an action from the selections is triggered.
     */
    virtual void actionTriggered(QAction* action);

    /**
     * For structured menu building. Deselects all items if the action was unchecked by the top menu
     */
    void slotToggled(bool);

protected:
    /**
     * Reimplemented from @see QWidgetAction.
     */
    virtual QWidget *createWidget(QWidget *parent);

    /**
     * Reimplemented from @see QWidgetAction.
     */
    virtual void deleteWidget(QWidget *widget);

    virtual bool eventFilter (QObject *watched, QEvent *event);

    /**
     * @internal
     * Creates a new KSelectAction object.
     *
     * @param dd the private d member
     * @param parent The action's parent object.
     */
    KSelectAction(KSelectActionPrivate &dd, QObject *parent);

    KSelectActionPrivate *d_ptr;

private:
    Q_PRIVATE_SLOT( d_func(), void _k_comboBoxDeleted(QObject*) )
    Q_PRIVATE_SLOT( d_func(), void _k_comboBoxCurrentIndexChanged(int) )
};

class KSelectActionPrivate
{
    Q_DECLARE_PUBLIC(KSelectAction)

public:
  KSelectActionPrivate()
    : q_ptr(0)
  {
    m_edit = false;
    m_menuAccelsEnabled = true;
    m_comboWidth = -1;
    m_maxComboViewCount = -1;

    m_toolBarMode = KSelectAction::ComboBoxMode;
    m_toolButtonPopupMode = QToolButton::InstantPopup; //InstantPopup by default because there is no default action

    m_actionGroup = new QActionGroup(0L);
  }

  virtual ~KSelectActionPrivate()
  {
    // unhook the event filter, as the deletion of the actiongroup
    // will trigger it
    Q_FOREACH( QComboBox* box, m_comboBoxes )
        box->removeEventFilter( q_ptr );
    Q_FOREACH( QToolButton* button, m_buttons )
        button->removeEventFilter( q_ptr );
    delete m_actionGroup;
  }

  void _k_comboBoxDeleted(QObject*);
  void _k_comboBoxCurrentIndexChanged(int);

  void init(KSelectAction*);

  bool m_edit : 1;
  bool m_menuAccelsEnabled : 1;
  int m_comboWidth;
  int m_maxComboViewCount;

  KSelectAction::ToolBarMode m_toolBarMode;
  QToolButton::ToolButtonPopupMode m_toolButtonPopupMode;

  QActionGroup* m_actionGroup;

  QList<QToolButton*> m_buttons;
  QList<QComboBox*> m_comboBoxes;

  QString makeMenuText( const QString &_text )
  {
      if ( m_menuAccelsEnabled )
        return _text;
      QString text = _text;
      int i = 0;
      while ( i < text.length() ) {
          if ( text[ i ] == '&' ) {
              text.insert( i, '&' );
              i += 2;
          }
          else
              ++i;
      }
      return text;
  }

  KSelectAction *q_ptr;
};

#endif /* KSELECTACTION_H_ */

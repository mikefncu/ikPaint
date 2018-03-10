/* This file was modified from the original file: kactioncollection.cpp
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

#ifndef KACTIONCOLLECTION_H_
#define KACTIONCOLLECTION_H_

#include <qobject.h>
#include <qaction.h>

/**
 * \short A container for a set of QAction objects.
 *
 * KActionCollection manages a set of QAction objects.  It
 * allows them to be grouped for organized presentation of configuration to the user,
 * saving + loading of configuration, and optionally for automatic plugging into
 * specified widget(s).
 *
 * Additionally, KActionCollection provides several convenience functions for locating
 * named actions, and actions grouped by QActionGroup.
 *
 * \note If you create your own action collection and need to assign shortcuts
 * to the actions within, you have to call associateWidget() or
 * addAssociatedWidget() to have them working.
 */

class KActionCollectionPrivate;

class KActionCollection : public QObject
{
    Q_OBJECT

public:
    /**
     * Constructor.  Allows specification of a component name other than the default
     * application name, where needed (remember to call setComponentDisplayName() too).
     */
    explicit KActionCollection(QObject *parent);

    /**
     * Destructor.
     */
    virtual ~KActionCollection();

    /**
     * Access the list of all action collections in existence for this app
     */
    static const QList<KActionCollection *> &allCollections();

    /**
     * Clears the entire action collection, deleting all actions.
     */
    void clear();

    /**
     * Associate all actions in this collection to the given \a widget.
     * Unlike addAssociatedWidget, this method only adds all current actions
     * in the collection to the given widget. Any action added after this call
     * will not be added to the given widget automatically.
     * So this is just a shortcut for a foreach loop and a widget->addAction call.
     */
    void associateWidget(QWidget *widget) const;

    /**
     * Associate all actions in this collection to the given \a widget, including any actions
     * added after this association is made.
     *
     * This does not change the action's shortcut context, so if you need to have the actions only
     * trigger when the widget has focus, you'll need to set the shortcut context on each action
     * to Qt::WidgetShortcut (or better still, Qt::WidgetWithChildrenShortcut with Qt 4.4+)
     */
    void addAssociatedWidget(QWidget *widget);

    /**
     * Remove an association between all actions in this collection and the given \a widget, i.e.
     * remove those actions from the widget, and stop associating newly added actions as well.
     */
    void removeAssociatedWidget(QWidget *widget);

    /**
     * Return a list of all associated widgets.
     */
    QList<QWidget *> associatedWidgets() const;

    /**
     * Clear all associated widgets and remove the actions from those widgets.
     */
    void clearAssociatedWidgets();

    /**
     * Returns the number of actions in the collection.
     *
     * This is equivalent to actions().count().
     */
    int count() const;

    /**
     * Returns whether the action collection is empty or not.
     */
    bool isEmpty() const;

    /**
     * Return the QAction* at position "index" in the action collection.
     *
     * This is equivalent to actions().value(index);
     */
    QAction *action(int index) const;

    /**
     * Get the action with the given \a name from the action collection.
     *
     * @param name Name of the QAction
     * @return A pointer to the QAction in the collection which matches the parameters or
     * null if nothing matches.
     */
    QAction *action(const QString &name) const;

    /**
     * Returns the list of QActions which belong to this action collection.
     *
     * The list is guaranteed to be in the same order the action were put into
     * the collection.
     */
    QList<QAction *> actions() const;

    /**
     * Returns the list of QActions without an QAction::actionGroup() which belong to this action collection.
     */
    const QList<QAction *> actionsWithoutGroup() const;

    /**
     * Returns the list of all QActionGroups associated with actions in this action collection.
     */
    const QList<QActionGroup *> actionGroups() const;

    /**
     * Set the \a componentName associated with this action collection.
     *
     * \warning Don't call this method on a KActionCollection that contains
     * actions. This is not supported.
     *
     * \param componentData the name which is to be associated with this action collection,
     * or QString() to indicate the app name. This is used to load/save settings into XML files.
     * KXmlGuiClient::setComponentName takes care of calling this.
     */
    void setComponentName(const QString &componentName);

    /** The component name with which this class is associated. */
    QString componentName() const;

    /**
     * Set the component display name associated with this action collection.
     * (e.g. for the toolbar editor)
     * KXmlGuiClient::setComponentName takes care of calling this.
     */
    void setComponentDisplayName(const QString &displayName);

    /** The display name for the associated component. */
    QString componentDisplayName() const;

Q_SIGNALS:
    /**
     * Indicates that \a action was inserted into this action collection.
     */
    void inserted(QAction *action);

    /**
     * Indicates that \a action was removed from this action collection.
     * @deprecated
     */
    QT_MOC_COMPAT void removed(QAction *action);

    /**
     * Indicates that \a action was highlighted (hovered over).
     * @deprecated Replaced by actionHovered(QAction* action);
     */
    QT_MOC_COMPAT void actionHighlighted(QAction *action);

    /**
     * Indicates that \a action was hovered.
     */
    void actionHovered(QAction *action);

    /**
     * Indicates that \a action was triggered
     */
    void actionTriggered(QAction *action);

protected:
    /// Overridden to perform connections when someone wants to know whether an action was highlighted or triggered
    // Not declarated, removing...
    // void connectNotify(const QMetaMethod &signal);

protected Q_SLOTS:
    virtual void slotActionTriggered();

    /**
     * @internal
     * @deprecated Replaced by slotActionHovered();
     */
    QT_MOC_COMPAT virtual void slotActionHighlighted();

private Q_SLOTS:
    void slotActionHovered();

public:
    /**
     * Add an action under the given name to the collection.
     *
     * Inserting an action that was previously inserted under a different name will replace the
     * old entry, i.e. the action will not be available under the old name anymore but only under
     * the new one.
     *
     * Inserting an action under a name that is already used for another action will replace
     * the other action in the collection (but will not delete it).
     *
     * If KAuthorized::authorizeKAction() reports that the action is not
     * authorized, it will be disabled and hidden.
     *
     * @param name The name by which the action be retrieved again from the collection.
     * @param action The action to add.
     * @return the same as the action given as parameter. This is just for convenience
     * (chaining calls) and consistency with the other addAction methods, you can also
     * simply ignore the return value.
     */
    Q_INVOKABLE QAction *addAction(const QString &name, QAction *action);

    /**
     * Adds a list of actions to the collection.
     *
     * The objectName of the actions is used as their internal name in the collection.
     *
     * Uses addAction(QString, QAction*).
     *
     * @param actions the list of the actions to add.
     *
     * @see addAction()
     * @since 5.0
     */
    void addActions(const QList<QAction *> &actions);

    /**
     * Removes an action from the collection and deletes it.
     * @param action The action to remove.
     */
    void removeAction(QAction *action);

    /**
     * Removes an action from the collection.
     * @param action the action to remove.
     */
    QAction *takeAction(QAction *action);

    /**
     * Creates a new action under the given name to the collection and connects
     * the action's triggered(bool) signal to the specified receiver/member. The
     * newly created action is returned.
     *
     * NOTE: KDE prior to 4.2 used the triggered() signal instead of the triggered(bool)
     * signal.
     *
     * Inserting an action that was previously inserted under a different name will replace the
     * old entry, i.e. the action will not be available under the old name anymore but only under
     * the new one.
     *
     * Inserting an action under a name that is already used for another action will replace
     * the other action in the collection.
     *
     * @param name The name by which the action be retrieved again from the collection.
     * @param receiver The QObject to connect the triggered(bool) signal to.  Leave 0 if no
     *                 connection is desired.
     * @param member The SLOT to connect the triggered(bool) signal to.  Leave 0 if no
     *               connection is desired.
     * @return new action of the given type ActionType.
     */
    QAction *addAction(const QString &name, const QObject *receiver = 0, const char *member = 0);

    /**
     * Creates a new action under the given name, adds it to the collection and connects the action's triggered(bool)
     * signal to the specified receiver/member. The receiver slot may accept either a bool or no
     * parameters at all (i.e. slotTriggered(bool) or slotTriggered() ).
     * The type of the action is specified by the template parameter ActionType.
     *
     * NOTE: KDE prior to 4.2 connected the triggered() signal instead of the triggered(bool)
     * signal.
     *
     * @param name The internal name of the action (e.g. "file-open").
     * @param receiver The QObject to connect the triggered(bool) signal to.  Leave 0 if no
     *                 connection is desired.
     * @param member The SLOT to connect the triggered(bool) signal to.  Leave 0 if no
     *               connection is desired.
     * @return new action of the given type ActionType.
     *
     * @see addAction()
     */
    template<class ActionType>
    ActionType *add(const QString &name, const QObject *receiver = 0, const char *member = 0)
    {
        ActionType *a = new ActionType(this);
        if (receiver && member) {
            connect(a, SIGNAL(triggered(bool)), receiver, member);
        }
        addAction(name, a);
        return a;
    }

    /**
     * Returns true if the given action's shortcuts may be configured by the user.
     *
     * @param action the action for the hint should be verified.
     * @since 5.0
     */
    bool isShortcutsConfigurable(QAction *action) const;

    /**
     * Indicate whether the user may configure the action's shortcuts.
     *
     * @param action the action for the hint should be verified.
     * @param configurable set to true if the shortcuts of the given action may be configured by the user, otherwise false.
     * @since 5.0
     */
    void setShortcutsConfigurable(QAction *action, bool configurable);

private:
    Q_PRIVATE_SLOT(d, void _k_actionDestroyed(QObject *))
    Q_PRIVATE_SLOT(d, void _k_associatedWidgetDestroyed(QObject *))

    friend class KActionCollectionPrivate;
    KActionCollectionPrivate *const d;
};

#endif /* KACTIONCOLLECTION_H_ */

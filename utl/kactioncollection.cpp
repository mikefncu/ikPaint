/* This file was modified from the original file: kactioncollection.cpp
 * from KDE project:

   Modified by Maikel Diaz
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

#include "kactioncollection.h"

#include <qdebug.h>

#include <QtXml/QDomDocument>
#include <QtCore/QSet>
#include <QApplication>
#include <QtCore/QMap>
#include <QtCore/QList>
#include <QAction>
#include <QMetaMethod>

#include <stdio.h>

class KActionCollectionPrivate
{
public:
    KActionCollectionPrivate()
        : connectTriggered(false),
          connectHovered(false),
          q(0)

    {
    }

    static QList<KActionCollection *> s_allCollections;

    void _k_associatedWidgetDestroyed(QObject *obj);
    void _k_actionDestroyed(QObject *obj);

    bool writeKXMLGUIConfigFile();

    QString m_componentName;
    QString m_componentDisplayName;

    QMap<QString, QAction *> actionByName;
    QList<QAction *> actions;

    bool connectTriggered : 1;
    bool connectHovered : 1;

    KActionCollection *q;

    QList<QWidget *> associatedWidgets;
};

QList<KActionCollection *> KActionCollectionPrivate::s_allCollections;

KActionCollection::KActionCollection(QObject *parent)
    : QObject(parent)
    , d(new KActionCollectionPrivate)
{
    d->q = this;
    KActionCollectionPrivate::s_allCollections.append(this);
}


KActionCollection::~KActionCollection()
{
    KActionCollectionPrivate::s_allCollections.removeAll(this);

    delete d;
}

void KActionCollection::clear()
{
    d->actionByName.clear();
    qDeleteAll(d->actions);
    d->actions.clear();
}

QAction *KActionCollection::action(const QString &name) const
{
    QAction *action = 0L;

    if (!name.isEmpty()) {
        action = d->actionByName.value(name);
    }

    return action;
}

QAction *KActionCollection::action(int index) const
{
    // ### investigate if any apps use this at all
    return actions().value(index);
}

int KActionCollection::count() const
{
    return d->actions.count();
}

bool KActionCollection::isEmpty() const
{
    return count() == 0;
}

void KActionCollection::setComponentName(const QString &cName)
{
    if (count() > 0) {
        // Its component name is part of an action's signature in the context of
        // global shortcuts and the semantics of changing an existing action's
        // signature are, as it seems, impossible to get right.
        // As of now this only matters for global shortcuts. We could
        // thus relax the requirement and only refuse to change the component data
        // if we have actions with global shortcuts in this collection.
        qWarning() << "this does not work on a KActionCollection containing actions!";
    }

    if (!cName.isEmpty()) {
        d->m_componentName = cName;
    } else {
        d->m_componentName = QCoreApplication::applicationName();
    }
}

QString KActionCollection::componentName() const
{
    return d->m_componentName;
}

void KActionCollection::setComponentDisplayName(const QString &displayName)
{
    d->m_componentDisplayName = displayName;
}

QString KActionCollection::componentDisplayName() const
{
    if (!d->m_componentDisplayName.isEmpty()) {
        return d->m_componentDisplayName;
    }
    if (!QApplication::applicationName().isEmpty()) {
        return QApplication::applicationName();
    }
    return QCoreApplication::applicationName();
}

QList<QAction *> KActionCollection::actions() const
{
    return d->actions;
}

const QList< QAction * > KActionCollection::actionsWithoutGroup() const
{
    QList<QAction *> ret;
    Q_FOREACH (QAction *action, d->actions)
        if (!action->actionGroup()) {
            ret.append(action);
        }
    return ret;
}

const QList< QActionGroup * > KActionCollection::actionGroups() const
{
    QSet<QActionGroup *> set;
    Q_FOREACH (QAction *action, d->actions)
        if (action->actionGroup()) {
            set.insert(action->actionGroup());
        }
    return set.toList();
}

QAction *KActionCollection::addAction(const QString &name, QAction *action)
{
    if (!action) {
        return action;
    }

    const QString objectName = action->objectName();
    QString indexName = name;

    if (indexName.isEmpty()) {
        // No name provided. Use the objectName.
        indexName = objectName;

    } else {

        // A name was provided. Check against objectName.
        if ((!objectName.isEmpty()) && (objectName != indexName)) {
            // The user specified a new name and the action already has a
            // different one. The objectName is used for saving shortcut
            // settings to disk. Both for local and global shortcuts.
            qDebug() << "Registering action " << objectName << " under new name " << indexName;
        }

        // Set the new name
        action->setObjectName(indexName);
    }

    // No name provided and the action had no name. Make one up. This will not
    // work when trying to save shortcuts. Both local and global shortcuts.
    if (indexName.isEmpty()) {
        indexName = indexName.sprintf("unnamed-%p", (void *)action);
        action->setObjectName(indexName);
    }

    // From now on the objectName has to have a value. Else we cannot safely
    // remove actions.
    Q_ASSERT(!action->objectName().isEmpty());

    // look if we already have THIS action under THIS name ;)
    if (d->actionByName.value(indexName, 0) == action) {
        // This is not a multi map!
        Q_ASSERT(d->actionByName.count(indexName) == 1);
        return action;
    }

    // Check if we have another action under this name
    if (QAction *oldAction = d->actionByName.value(indexName)) {
        takeAction(oldAction);
    }

    // Check if we have this action under a different name.
    // Not using takeAction because we don't want to remove it from categories,
    // and because it has the new name already.
    const int oldIndex = d->actions.indexOf(action);
    if (oldIndex != -1) {
        d->actionByName.remove(d->actionByName.key(action));
        d->actions.removeAt(oldIndex);
    }

    // Add action to our lists.
    d->actionByName.insert(indexName, action);
    d->actions.append(action);

    Q_FOREACH (QWidget *widget, d->associatedWidgets) {
        widget->addAction(action);
    }

    connect(action, SIGNAL(destroyed(QObject*)), SLOT(_k_actionDestroyed(QObject*)));

    if (d->connectHovered) {
        connect(action, SIGNAL(hovered()), SLOT(slotActionHovered()));
    }

    if (d->connectTriggered) {
        connect(action, SIGNAL(triggered(bool)), SLOT(slotActionTriggered()));
    }

    emit inserted(action);
    return action;
}

void KActionCollection::addActions(const QList<QAction *> &actions)
{
    Q_FOREACH (QAction *action, actions) {
        addAction(action->objectName(), action);
    }
}

void KActionCollection::removeAction(QAction *action)
{
    delete takeAction(action);
}

QAction *KActionCollection::takeAction(QAction *action)
{
    // Remove the action from all widgets
    Q_FOREACH (QWidget *widget, d->associatedWidgets) {
        widget->removeAction(action);
    }

    action->disconnect(this);

    emit removed(action);   //deprecated
    return action;
}


QAction *KActionCollection::addAction(const QString &name, const QObject *receiver, const char *member)
{
    QAction *a = new QAction(this);
    if (receiver && member) {
        connect(a, SIGNAL(triggered(bool)), receiver, member);
    }
    return addAction(name, a);
}

bool KActionCollection::isShortcutsConfigurable(QAction *action) const
{
    // Considered as true by default
    const QVariant value = action->property("isShortcutConfigurable");
    return value.isValid() ? value.toBool() : true;
}

void KActionCollection::setShortcutsConfigurable(QAction *action, bool configurable)
{
    action->setProperty("isShortcutConfigurable", configurable);
}

void KActionCollection::slotActionTriggered()
{
    QAction *action = qobject_cast<QAction *>(sender());
    if (action) {
        emit actionTriggered(action);
    }
}

void KActionCollection::slotActionHighlighted()
{
    slotActionHovered();
}

void KActionCollection::slotActionHovered()
{
    QAction *action = qobject_cast<QAction *>(sender());
    if (action) {
        emit actionHighlighted(action);
        emit actionHovered(action);
    }
}

void KActionCollectionPrivate::_k_actionDestroyed(QObject *obj)
{
    // obj isn't really a QAction anymore. So make sure we don't do fancy stuff
    // with it.
    QAction *action = static_cast<QAction *>(obj);

    //HACK the object we emit is partly destroyed
    emit q->removed(action); //deprecated. remove in KDE5
}

const QList< KActionCollection * > &KActionCollection::allCollections()
{
    return KActionCollectionPrivate::s_allCollections;
}

void KActionCollection::associateWidget(QWidget *widget) const
{
    Q_FOREACH (QAction *action, actions()) {
        if (!widget->actions().contains(action)) {
            widget->addAction(action);
        }
    }
}

void KActionCollection::addAssociatedWidget(QWidget *widget)
{
    if (!d->associatedWidgets.contains(widget)) {
        widget->addActions(actions());

        d->associatedWidgets.append(widget);
        connect(widget, SIGNAL(destroyed(QObject*)), this, SLOT(_k_associatedWidgetDestroyed(QObject*)));
    }
}

void KActionCollection::removeAssociatedWidget(QWidget *widget)
{
    Q_FOREACH (QAction *action, actions()) {
        widget->removeAction(action);
    }

    d->associatedWidgets.removeAll(widget);
    disconnect(widget, SIGNAL(destroyed(QObject*)), this, SLOT(_k_associatedWidgetDestroyed(QObject*)));
}

QList< QWidget * > KActionCollection::associatedWidgets() const
{
    return d->associatedWidgets;
}

void KActionCollection::clearAssociatedWidgets()
{
    Q_FOREACH (QWidget *widget, d->associatedWidgets)
        Q_FOREACH (QAction *action, actions()) {
            widget->removeAction(action);
        }

    d->associatedWidgets.clear();
}

void KActionCollectionPrivate::_k_associatedWidgetDestroyed(QObject *obj)
{
    associatedWidgets.removeAll(static_cast<QWidget *>(obj));
}

#include "moc_kactioncollection.cpp"


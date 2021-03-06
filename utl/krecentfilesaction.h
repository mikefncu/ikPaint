/* This file is part of the KDE libraries
    Copyright (C) 1999 Reginald Stadlbauer <reggie@kde.org>
              (C) 1999 Simon Hausmann <hausmann@kde.org>
              (C) 2000 Nicolas Hadacek <haadcek@kde.org>
              (C) 2000 Kurt Granroth <granroth@kde.org>
              (C) 2000 Michael Koch <koch@kde.org>
              (C) 2001 Holger Freyther <freyther@kde.org>
              (C) 2002 Ellis Whitehead <ellis@kde.org>
              (C) 2003 Andras Mantia <amantia@kde.org>
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

#ifndef KRECENTFILESACTION_H
#define KRECENTFILESACTION_H

#include <kselectaction.h>
#include <qpointer.h>

class QSettings;
class KRecentFilesActionPrivate;

/**
 *  @short Recent files action
 *
 *  This class is an action to handle a recent files submenu.
 *  The best way to create the action is to use KStandardAction::openRecent.
 *  Then you simply need to call loadEntries on startup, saveEntries
 *  on shutdown, addURL when your application loads/saves a file.
 *
 *  @author Michael Koch
 */
class KRecentFilesAction : public KSelectAction
{
  Q_OBJECT
  Q_DECLARE_PRIVATE(KRecentFilesAction)

public:
    /**
     * Constructs an action with the specified parent.
     *
     * @param parent The parent of this action.
     */
    explicit KRecentFilesAction(QObject *parent);

    /**
     * Constructs an action with text; a shortcut may be specified by
     * the ampersand character (e.g. "&Option" creates a shortcut with key \e O )
     *
     * This is the most common KAction used when you do not have a
     * corresponding icon (note that it won't appear in the current version
     * of the "Edit ToolBar" dialog, because an action needs an icon to be
     * plugged in a toolbar...).
     *
     * @param text The text that will be displayed.
     * @param parent The parent of this action.
     */
    KRecentFilesAction(const QString &text, QObject *parent);

    /**
     * Constructs an action with text and an icon; a shortcut may be specified by
     * the ampersand character (e.g. "&Option" creates a shortcut with key \e O )
     *
     * This is the other common KAction used.  Use it when you
     * \e do have a corresponding icon.
     *
     * @param icon The icon to display.
     * @param text The text that will be displayed.
     * @param parent The parent of this action.
     */
    KRecentFilesAction(const QIcon &icon, const QString &text, QObject *parent);

  /**
   *  Destructor.
   */
  virtual ~KRecentFilesAction();

  /**
   * Adds \a action to the list of URLs, with \a url and title \a name.
   *
   * Do not use addAction(QAction*), as no url will be associated, and
   * consequently urlSelected() will not be emitted when \a action is selected.
   */
  void addAction(QAction* action, const QString& url, const QString& name);

  /**
   * Reimplemented for internal reasons.
   */
  virtual QAction* removeAction(QAction* action);

  /**
   * Reimplemented for internal reasons.
   */
  virtual void clear();

  /**
   *  Returns the maximum of items in the recent files list.
   */
  int maxItems() const;

  /**
   *  Sets the maximum of items in the recent files list.
   *  The default for this value is 10 set in the constructor.
   *
   *  If this value is lesser than the number of items currently
   *  in the recent files list the last items are deleted until
   *  the number of items are equal to the new maximum.
   */
  void setMaxItems( int maxItems );

  /**
   *  Loads the recent files entries from a given KConfigGroup object.
   *  You can provide the name of the group used to load the entries.
   *  If the groupname is empty, entries are load from a group called 'RecentFiles'
   *
   */
  void loadEntries( const QSettings &config );

  /**
   *  Saves the current recent files entries to a given KConfigGroup object.
   *  You can provide the name of the group used to load the entries.
   *  If the groupname is empty, entries are saved to a group called 'RecentFiles'
   *
   */
  void saveEntries(QSettings &config );

  /**
   *  Add URL to recent files list.
   *
   *  @param url The URL of the file
   *  @param name The user visible pretty name that appears before the URL
   */
  void addUrl( const QString& url, const QString& name = QString() );

  /**
   *  Remove an URL from the recent files list.
   *
   *  @param url The URL of the file
   */
  void removeUrl( const QString& url );

  /**
   *  Retrieve a list of all URLs in the recent files list.
   */
  QList<QString> urls() const;

Q_SIGNALS:
  /**
   *  This signal gets emitted when the user selects an URL.
   *
   *  @param url The URL thats the user selected.
   */
  void urlSelected( const QString& url );

private:

    // Don't warn about the virtual overload. As the comment of the other
    // addAction() says, addAction( QAction* ) should not be used.
    using KSelectAction::addAction;

    Q_PRIVATE_SLOT( d_func(), void _k_urlSelected(QAction*) )
};

class KRecentFilesActionPrivate : public KSelectActionPrivate
{
    Q_DECLARE_PUBLIC(KRecentFilesAction)

public:
  KRecentFilesActionPrivate()
  {
    m_maxItems = 10;
    m_noEntriesAction=0;
  }

  virtual ~KRecentFilesActionPrivate()
  {
  }

  void init();

  void _k_urlSelected(QAction*);

  int m_maxItems;
  QMap<QAction*, QString> m_shortNames;
  QMap<QAction*, QString> m_urls;
  QPointer<QAction> m_noEntriesAction;
};

#endif

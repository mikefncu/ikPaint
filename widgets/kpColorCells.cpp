
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


#define DEBUG_KP_COLOR_CELLS 0


#include <kpColorCells.h>

#include <QContextMenuEvent>
#include <QMouseEvent>
#include <QScrollBar>

#include <QColorDialog>
#include <qdebug.h>
#include <qlocale.h>
#include <tools.h>

#include <kpColor.h>
#include <kpColorCollection.h>
#include <kpDefaultColorCollection.h>

//---------------------------------------------------------------------
//
// Table Geometry
//

const int TableMimimumNumCells = 10;

const int TableDefaultNumRows = 2;

const int TableDefaultCellWidth = 26;

const int TableDefaultHeight = 52;

const int TableMimimumNumColumns = 5;

const int TableMaximumNumColumns = 20;

static int TableNumRows (const kpColorCollection &colorCol)
{
    if (colorCol.count () == 0)
        return 0;

    return ::TableDefaultNumRows;
}

static int TableNumColumns (const kpColorCollection &colorCol)
{
    const int rows = ::TableNumRows (colorCol);
    if (rows == 0)
        return 0;

    return (colorCol.count () + (rows - 1)) / rows;
}

static int TableCellWidth (const kpColorCollection &colorCol)
{
    Q_UNUSED (colorCol);

    return ::TableDefaultCellWidth;
}

static int TableCellHeight (const kpColorCollection &colorCol)
{

    return ::TableDefaultHeight / ::TableNumRows (colorCol);
}


//
// kpColorCells
//


struct kpColorCellsPrivate
{
    Qt::Orientation orientation;

    // REFACTOR: This is data duplication with kpColorCellsBase::color[].
    //           We've probably forgotten to synchronize them in some points.
    //
    // Calls to kpColorCellsBase::setColor() (which also come from
    // kpColorCellsBase itself) will automatically update both
    // kpColorCellsBase::d->color[] and the table cells.  setColor() emits
    // colorChanged(), which is caught by our slotColorChanged(),
    // which synchronizes this color collection and updates the modified flag.
    //
    // Avoid calling our grandparent's, QTableWidget's, mutating methods as we
    // don't override enough of them, to fire signals that we can catch to update
    // this color collection.
    //
    // If you modify this color collection directly (e.g. in setColorCollection(),
    // openColorCollection(), appendRow(), deleteLastRow(), ...), you must work
    // the other way and call makeCellsMatchColorCollection() to synchronize
    // kpColorCellsBase::d->color[] and the table cells.  You still need to update
    // the modified flag.
    kpColorCollection colorCol;

    bool isModified;

    bool blockColorChangedSig;
};

//---------------------------------------------------------------------

kpColorCells::kpColorCells (QWidget *parent,
                            Qt::Orientation o)
    : kpColorCellsBase (parent, 0/*rows for now*/, 0/*cols for now*/),
      d (new kpColorCellsPrivate ())
{
    d->orientation = o;
    d->isModified = false;
    d->blockColorChangedSig = false;


    // When a text box is active, clicking to change the background color
    // should not move the keyboard focus away from the text box.
    setFocusPolicy (Qt::TabFocus);

    setShading (false);  // no 3D look

    setAcceptDrops (true);
    setAcceptDrags (true);


    setCellsResizable (false);

    if (o == Qt::Horizontal)
    {
        // Reserve enough room for the default color collection's cells _and_
        // a vertical scrollbar, which only appears when it's required.
        // This ensures that if the vertical scrollbar appears, it does not obscure
        // any cells or require the addition of a horizontal scrollbar, which would
        // look ugly and take even more precious room.
        //
        // We do not dynamically reserve room based on the actual number of rows
        // of cells, as that would make our containing widgets too big.
        /*setMinimumSize (::TableDefaultWidth + frameWidth () * 2 +
                         verticalScrollBar()->sizeHint().width(),
                        ::TableDefaultHeight + frameWidth () * 2);*/

        setMaximumHeight(::TableDefaultHeight + frameWidth () * 2); // horizontalScrollBar()->sizeHint().width()
    }
    else
    {
        Q_ASSERT (!"implemented");
    }

    setHorizontalScrollBarPolicy (Qt::ScrollBarAsNeeded);

    // The default QTableWidget policy of QSizePolicy::Expanding forces our
    // containing widgets to get too big.  Override it.
    setSizePolicy (QSizePolicy::Minimum, QSizePolicy::Minimum);


    connect (this, SIGNAL (colorSelected (int, const QColor &, Qt::MouseButton)),
             SLOT (slotColorSelected (int, const QColor &, Qt::MouseButton)));
    connect (this, SIGNAL (colorDoubleClicked (int, const QColor &)),
             SLOT (slotColorDoubleClicked (int, const QColor &)));
    connect (this, SIGNAL (colorChanged (int, const QColor &)),
             SLOT (slotColorChanged (int, const QColor &)));

    if (!openColorCollection())
        setColorCollection (DefaultColorCollection ());

    setWhatsThis (
        i18n (
            "<qt>"

            "<p>To select the foreground color that tools use to draw,"
            " left-click on a filled-in color cell."
            " To select the background color, right-click instead.</p>"

            "<p>To change the color of a color cell itself, double-click on it.</p>"

            "<p>You can also swap the color of a filled-in cell with any other"
            " cell using drag and drop."
            " Also, if you hold down the <b>Ctrl</b> key, the destination"
            " cell's color will be"
            " overwritten, instead of being swapped with the color of the source cell.</p>"

            "</qt>"));
}

//---------------------------------------------------------------------

kpColorCells::~kpColorCells ()
{
    delete d;
}

//---------------------------------------------------------------------


// public static
kpColorCollection kpColorCells::DefaultColorCollection ()
{
    return kpDefaultColorCollection ();
}

//---------------------------------------------------------------------


// public
Qt::Orientation kpColorCells::orientation () const
{
    return d->orientation;
}

//---------------------------------------------------------------------

// public
void kpColorCells::setOrientation (Qt::Orientation o)
{
    d->orientation = o;

    makeCellsMatchColorCollection ();
}

//---------------------------------------------------------------------


// protected
// OPT: Find out why this is being called multiple times on startup.
void kpColorCells::makeCellsMatchColorCollection ()
{
    int c, r;

    if (orientation () == Qt::Horizontal)
    {
        c = ::TableNumColumns (d->colorCol);
        r = ::TableNumRows (d->colorCol);
    }
    else
    {
        c = ::TableNumRows (d->colorCol);
        r = ::TableNumColumns (d->colorCol);;
    }

#if DEBUG_KP_COLOR_CELLS
    kDebug () << "kpColorCells::makeCellsMatchColorCollection():"
              << "r=" << r << "c=" << c;
    kDebug () << "verticalScrollBar=" << verticalScrollBar ()
              << " sizeHint="
              << (verticalScrollBar () ?
                    verticalScrollBar ()->sizeHint () :
                    QSize (-12, -34));
#endif

    // Delete all cell widgets.  This ensures that there will be no left-over
    // cell widgets, for the colors in the new color collection that are
    // actually invalid (which should not have cell widgets).
    clearContents ();

    setRowCount (r);
    setColumnCount (c);

    setMinimumWidth(c * (::TableDefaultCellWidth + frameWidth () * 2));

    int CellWidth = ::TableCellWidth (d->colorCol),
        CellHeight = ::TableCellHeight (d->colorCol);

    // TODO: Take a screenshot of KolourPaint, magnify it and you'll find the
    //       cells don't have exactly the sizes requested here.  e.g. the
    //       top row of cells is 1 pixel shorter than the bottom row.  There
    //       are probably other glitches.
    for (int y = 0; y < r; y++)
        setRowHeight (y, CellHeight);
    for (int x = 0; x < c; x++)
        setColumnWidth (x, CellWidth);


    const bool oldBlockColorChangedSig = d->blockColorChangedSig;
    d->blockColorChangedSig = true;
    // The last "(rowCount() * columnCount()) - d->colorCol.count()" cells
    // will be empty because we did not initialize them.
    for (int i = 0; i < d->colorCol.count (); i++)
    {
        int y, x;
        int pos;

        if (orientation () == Qt::Horizontal)
        {
            y = i / c;
            x = i % c;
            pos = i;
        }
        else
        {
            y = i % r;
            x = i / r;
            // int x = c - 1 - i / r;
            pos = y * c + x;
        }
    #if DEBUG_KP_COLOR_CELLS && 0
        kDebug () << "\tSetting cell " << i << ": y=" << y << " x=" << x
                  << " pos=" << pos << endl;
        kDebug () << "\t\tcolor=" << (int *) d->colorCol.color (i).rgba()
                  << "isValid=" << d->colorCol.color (i).isValid ();
    #endif

        // (color may be invalid resulting in a hole in the middle of the table)
        setColor (pos, d->colorCol.color (i), d->colorCol.name (i));
        //this->setToolTip( cellGeometry (y, x), colors [i].name ());
    }
    d->blockColorChangedSig = oldBlockColorChangedSig;
}

//---------------------------------------------------------------------


bool kpColorCells::isModified () const
{
    return d->isModified;
}

//---------------------------------------------------------------------

void kpColorCells::setModified (bool yes)
{
#if DEBUG_KP_COLOR_CELLS
    kDebug () << "kpColorCells::setModified(" << yes << ")";
#endif

    if (yes == d->isModified)
        return;

    d->isModified = yes;

    emit isModifiedChanged (yes);
}

//---------------------------------------------------------------------

void kpColorCells::setModified ()
{
    setModified (true);
}

//---------------------------------------------------------------------

QString kpColorCells::name () const
{
    return d->colorCol.name ();
}

//---------------------------------------------------------------------


const kpColorCollection *kpColorCells::colorCollection () const
{
    return &d->colorCol;
}

//---------------------------------------------------------------------


void kpColorCells::ensureHaveAtLeastOneRow ()
{
    if (d->colorCol.count () == 0)
        d->colorCol.resize (::TableMimimumNumCells);
}

//---------------------------------------------------------------------

void kpColorCells::setColorCollection (const kpColorCollection &colorCol, const QString &)
{
    d->colorCol = colorCol;
    ensureHaveAtLeastOneRow ();

    setModified (false);

    makeCellsMatchColorCollection ();

    emit columnCountChanged (columnCount ());
    emit nameChanged (name ());
}

//---------------------------------------------------------------------


bool kpColorCells::openColorCollection ()
{
    // (this will pop up an error dialog on failure)
    if (d->colorCol.open ())
    {
        ensureHaveAtLeastOneRow ();

        setModified (false);

        makeCellsMatchColorCollection ();

        emit columnCountChanged (columnCount ());
        emit nameChanged (name ());

        return true;
    }

    return false;
}

//---------------------------------------------------------------------

bool kpColorCells::saveColorCollection ()
{
    // (this will pop up an error dialog on failure)
    if (d->colorCol.save())
    {
        setModified (false);
        return true;
    }

    return false;
}

//---------------------------------------------------------------------


void kpColorCells::appendColumn ()
{
    // This is the easiest implementation: change the color collection
    // and then synchronize the table cells.  The other way is to call
    // setRowCount() and then, synchronize the color collection.

    const int targetNumCells = (columnCount () + 1) * ::TableDefaultNumRows;
    d->colorCol.resize (targetNumCells);

    setModified (true);

    makeCellsMatchColorCollection ();

    emit columnCountChanged (columnCount ());
}

//---------------------------------------------------------------------

void kpColorCells::deleteLastColumn ()
{
    // This is the easiest implementation: change the color collection
    // and then synchronize the table cells.  The other way is to call
    // setRowCount() and then, synchronize the color collection.

    const int targetNumCells =
        qMax (0, (columnCount () - 1) * ::TableDefaultNumRows);
    d->colorCol.resize (targetNumCells);

    // If there was only one row of colors to start with, the effect of this
    // line (after the above resize()) is to change that row to a row of
    // invalid colors.
    ensureHaveAtLeastOneRow ();

    setModified (true);

    makeCellsMatchColorCollection ();

    emit columnCountChanged (columnCount ());
}

//---------------------------------------------------------------------

bool kpColorCells::canAppendColumn () {

    return (columnCount () < TableMaximumNumColumns);
}

//---------------------------------------------------------------------

bool kpColorCells::canDeleteColumn () {

    return (columnCount () > TableMimimumNumColumns);
}

//---------------------------------------------------------------------


// protected virtual [base QWidget]
void kpColorCells::contextMenuEvent (QContextMenuEvent *e)
{
    // Eat right-mouse press to prevent it from getting to the toolbar.
    e->accept ();
}

//---------------------------------------------------------------------

// protected slot
void kpColorCells::slotColorSelected (int cell, const QColor &color,
        Qt::MouseButton button)
{
#if DEBUG_KP_COLOR_CELLS
    kDebug () << "kpColorCells::slotColorSelected(cell=" << cell
               << ") mouseButton = " << button
               << " rgb=" << (int *) color.rgba()
               << endl;
#else
    Q_UNUSED (cell);
#endif

    if (button == Qt::LeftButton)
    {
        emit foregroundColorChanged (kpColor (color.rgba()));
    }
    else if (button == Qt::RightButton)
    {
        emit backgroundColorChanged (kpColor (color.rgba()));
    }

    // REFACTOR: Make selectedness configurable inside kpColorCellsBase?
    //
    // Deselect the selected cell (selected by above kpColorCellsBase::mouseReleaseEvent()).
    // KolourPaint's palette has no concept of a current cell/color: you can
    // pick a color but you can't mark a cell as selected.  In any case, a
    // selected cell would be rendered as violet, which would ruin the cell.
    //
    // setSelectionMode (kpColorCellsBase::NoSelection); does not work so we
    // clearSelection().  I think setSelectionMode() concerns when the user
    // directly selects a cell - not when kpColorCellsBase::mouseReleaseEvent()
    // selects a cell programmatically.
    clearSelection ();
}

//---------------------------------------------------------------------

// protected slot
void kpColorCells::slotColorDoubleClicked (int cell, const QColor &)
{
    QColorDialog dialog(this);
    dialog.setCurrentColor(kpColorCellsBase::color(cell));
    dialog.setOption(QColorDialog::ShowAlphaChannel, true);

#if QT_VERSION >= 0x050200
    if ( dialog.exec() == QDialog::Accepted )
        setColor (cell, dialog.currentColor(), dialog.currentColor().name(QColor::HexArgb).toUpper());
#else
    if ( dialog.exec() == QDialog::Accepted )
        setColor (cell, dialog.currentColor(), dialog.currentColor().name());
#endif

}

//---------------------------------------------------------------------

// protected slot
void kpColorCells::slotColorChanged (int cell, const QColor &color)
{
#if DEBUG_KP_COLOR_CELLS
    kDebug () << "cell=" << cell << "color=" << (const int *) color.rgba()
              << "d->colorCol.count()=" << d->colorCol.count ();
#endif

    if (d->blockColorChangedSig)
        return;

    // Cater for adding new colors to the end.
    if (cell >= d->colorCol.count ())
        d->colorCol.resize (cell + 1);

    // TODO: We lose color names on a color swap (during drag-and-drop).
#if QT_VERSION >= 0x050200
    const int ret = d->colorCol.changeColor (cell, color, color.name(QColor::HexArgb).toUpper());
#else
    const int ret = d->colorCol.changeColor (cell, color, QString ()/*color name*/);
#endif

    Q_ASSERT (ret == cell);

    setModified (true);
}

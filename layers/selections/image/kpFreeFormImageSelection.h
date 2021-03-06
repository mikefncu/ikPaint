
/*
   Copyright (c) 2003-2007 Clarence Dang <dang@kde.org>
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


#ifndef kpFreeFormImageSelection_H
#define kpFreeFormImageSelection_H


#include <kpAbstractImageSelection.h>


class kpFreeFormImageSelection : public kpAbstractImageSelection
{
Q_OBJECT

public:
    kpFreeFormImageSelection (const kpImageSelectionTransparency &transparency =
        kpImageSelectionTransparency ());

    kpFreeFormImageSelection (const QPolygon &points,
        const kpImage &baseImage = kpImage (),
        const kpImageSelectionTransparency &transparency =
            kpImageSelectionTransparency ());

    kpFreeFormImageSelection (const QPolygon &points,
        const kpImageSelectionTransparency &transparency =
            kpImageSelectionTransparency ());

    kpFreeFormImageSelection (const kpFreeFormImageSelection &rhs);

    kpFreeFormImageSelection &operator= (const kpFreeFormImageSelection &rhs);

    virtual kpFreeFormImageSelection *clone () const;

    virtual ~kpFreeFormImageSelection ();


//
// Marshalling
//

public:
    static const int SerialID = 2;
    virtual int serialID () const;

    virtual bool readFromStream (QDataStream &stream);

    virtual void writeToStream (QDataStream &stream) const;


//
// General Queries
//

public:
    virtual kpCommandSize::SizeType size () const;

    virtual bool isRectangular () const;

    // (as passed to the constructor)
    QPolygon originalPoints () const;


//
// Cardinally Adjacent Points
//

protected:
    void recalculateCardinallyAdjacentPoints ();

public:
    // Returns the originalPoints() interpolated to be cardinally adjacent.
    QPolygon cardinallyAdjacentPoints () const;

    // Returns cardinallyAdjacentPoints() but with extra points interpolated
    // from the last point to the first point (the original points are
    // thought of as a polygon where the first and last points are connected,
    // rather than as a string of points).
    //
    // As used by the shape mask methods.
    QPolygon cardinallyAdjacentPointsLoop () const;


//
// Position & Dimensions
//

public:
    // Implements kpAbstractSelection interface - same as
    // cardinallyAdjacentPointsLoop ().
    // This implementation is fast.
    virtual QPolygon calculatePoints () const;


//
// Shape Mask
//

public:
    virtual QRegion shapeRegion () const;


//
// Point Testing
//

public:
    virtual bool contains (const QPoint &point) const;


//
// Mutation
//

public:
    virtual void moveBy (int dx, int dy);

    virtual void flip (bool horiz, bool vert);


//
// Rendering
//

public:
    virtual void paintBorder (QImage *destPixmap, const QRect &docRect,
        bool selectionFinished) const;


private:
    struct kpFreeFormImageSelectionPrivate * const d;
};


#endif  // kpFreeFormImageSelection_H


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


#include <kpDefaultColorCollection.h>

#include <kpColor.h>


kpDefaultColorCollection::kpDefaultColorCollection ()
{
    kpColor colors [] =
    {
        kpColor::Black,
        kpColor::White,

        kpColor::Gray,
        kpColor::LightGray,

        kpColor::Red,
        kpColor::DarkRed,

        kpColor::Orange,
        kpColor::DarkOrange,

        kpColor::Yellow,
        kpColor::DarkYellow,

        kpColor::Green,
        kpColor::DarkGreen,

        kpColor::Aqua,
        kpColor::DarkAqua,

        kpColor::Blue,
        kpColor::DarkBlue,

        kpColor::Purple,
        kpColor::DarkPurple,

        kpColor::Pink,
        kpColor::LightBlue,

        kpColor::LightGreen,
        kpColor::Tan
    };

    for (int i = 0; i < (int) (sizeof (colors) / sizeof (colors [0])); i++)
    {
#if QT_VERSION >= 0x050200
        addColor (colors [i].toQColor (), colors [i].toQColor ().name(QColor::HexArgb).toUpper());
#else
        addColor (colors [i].toQColor (), colors [i].toQColor ().name());
#endif
    }
}

kpDefaultColorCollection::~kpDefaultColorCollection ()
{
}

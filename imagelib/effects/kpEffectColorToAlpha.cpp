/* This file takes code from from GIMP program

   Maikel Diaz <ariguanabosoft@gmail.com>
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

#include "kpEffectColorToAlpha.h"

#include <math.h>

#include <qimage.h>
#include <qcolor.h>

#include <qdebug.h>

void kpEffectColorToAlpha::colortoalpha (qreal *a1,
                                         qreal *a2,
                                         qreal *a3,
                                         qreal *a4,
                                         qreal c1,
                                         qreal c2,
                                         qreal c3)
{
    qreal alpha1, alpha2, alpha3, alpha4;

    alpha4 = *a4;

    if ( *a1 > c1 )
        alpha1 = (*a1 - c1)/(255.0-c1);
    else if ( *a1 < c1 )
        alpha1 = (c1 - *a1)/(c1);
    else alpha1 = 0.0;

    if ( *a2 > c2 )
        alpha2 = (*a2 - c2)/(255.0-c2);
    else if ( *a2 < c2 )
        alpha2 = (c2 - *a2)/(c2);
    else alpha2 = 0.0;

    if ( *a3 > c3 )
        alpha3 = (*a3 - c3)/(255.0-c3);
    else if ( *a3 < c3 )
        alpha3 = (c3 - *a3)/(c3);
    else alpha3 = 0.0;

    if ( alpha1 > alpha2 )
    if ( alpha1 > alpha3 )
      {
        *a4 = alpha1;
      }
    else
      {
        *a4 = alpha3;
      }
    else
    if ( alpha2 > alpha3 )
      {
        *a4 = alpha2;
      }
    else
      {
        *a4 = alpha3;
      }

    *a4 *= 255.0;

    if ( *a4 < 1.0 )
        return;
    *a1 = 255.0 * (*a1-c1)/ *a4 + c1;
    *a2 = 255.0 * (*a2-c2)/ *a4 + c2;
    *a3 = 255.0 * (*a3-c3)/ *a4 + c3;

    *a4 *= alpha4/255.0;
}

// public static
kpImage kpEffectColorToAlpha::applyEffect (const kpImage &image,
                                           QRgb argb)
{

    QImage qimage = image;

    const qreal ared = (qreal) qRed (argb);
    const qreal agreen = (qreal) qGreen(argb);
    const qreal ablue = (qreal) qBlue (argb);

    if (qimage.depth () > 8)
    {
        for (int y = 0; y < qimage.height (); y++)
        {
            for (int x = 0; x < qimage.width (); x++)
            {
                // Assuming QImage::Format_ARGB32_Premultiplied
                const QRgb rgb = qUnpremultiply(qimage.pixel (x, y)); // Qt 5.3 : qUnpremultiply

                qreal red = (qreal) qRed (rgb);
                qreal green = (qreal) qGreen (rgb);
                qreal blue = (qreal) qBlue (rgb);
                qreal alpha = (qreal) qAlpha (rgb);

                colortoalpha (&red,
                              &green,
                              &blue,
                              &alpha,
                              ared,
                              agreen,
                              ablue);

                qimage.setPixel (x, y, qPremultiply(qRgba(red, green, blue, alpha)));   // Qt 5.3 : qPremultiply
            }
        }
    }
    else
    {
        for (int i = 0; i < qimage.colorCount (); i++)
        {
            const QRgb rgb = qimage.color (i);

            qreal red = (qreal) qRed (rgb);
            qreal green = (qreal) qGreen (rgb);
            qreal blue = (qreal) qBlue (rgb);
            qreal alpha = (qreal) qAlpha (rgb);

            colortoalpha (&red,
                          &green,
                          &blue,
                          &alpha,
                          ared,
                          agreen,
                          ablue);

            qimage.setColor (i, qRgba(red, green, blue, alpha));
        }

    }

    return qimage;
}


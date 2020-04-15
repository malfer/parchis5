/*
 * ctxrot.c --- rotate context
 * 
 * Copyright (c) 2020 Mariano Alvarez Fernandez (malfer@telefonica.net)
 *
 * This file is part of Parchís5, a popular spanish game
 *
 * Parchís5 is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * Parchís5 is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with Parchís5. If not, see <http://www.gnu.org/licenses/>.
 *
 */

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <math.h>
#include <mgrx.h>

GrContext * CtxRotate(GrContext *src, int angle, GrColor bg)
{
    GrContext *dst, graux;
    int w, h;
    float rad, angsin, angcos;
    float xc, yc, w2, h2, precx1, precx2;
    int x, y, xs, ys;
    GrColor pixel;

    angle = angle % 360;
    if (angle < 0) angle = 360 + angle;
    
    w = src->gc_xmax + 1;
    h = src->gc_ymax + 1;

    dst = GrCreateContext(w, h, NULL, NULL);
    if (dst == NULL) return NULL;

    GrSaveContext(&graux);
    GrSetContext(dst);

    if (angle == 0) {
        GrBitBlt(NULL, 0, 0, src, 0, 0, w-1, h-1, GrWRITE);
    } else if (angle == 90) {
        for (x=0; x<w; x++) {
            for (y=0; y<h; y++) {
                xs = y;
                ys = (h - 1) - x;
                if (xs >= 0 && xs < w && ys >= 0 && ys < h) {
                    pixel = GrPixelC(src, xs, ys);
                    GrPlot(x, y, pixel);
                } else {
                    GrPlot(x, y, bg);
                }
            }
        }
    } else if (angle == 180) {
        for (x=0; x<w; x++) {
            xs = (w - 1) - x;
            for (y=0; y<h; y++) {
                ys = (h - 1) - y;
                if (xs >= 0 && xs < w && ys >= 0 && ys < h) {
                    pixel = GrPixelC(src, xs, ys);
                    GrPlot(x, y, pixel);
                } else {
                    GrPlot(x, y, bg);
                }
            }
        }
    } else if (angle == 270) {
        for (x=0; x<w; x++) {
            for (y=0; y<h; y++) {
                xs = (w - 1) - y;
                ys = x;
                if (xs >= 0 && xs < w && ys >= 0 && ys < h) {
                    pixel = GrPixelC(src, xs, ys);
                    GrPlot(x, y, pixel);
                } else {
                    GrPlot(x, y, bg);
                }
            }
        }
    } else {
        w2 = (w - 1) / 2.0;
        h2 = (h - 1) / 2.0;
        rad = angle * 0.0174532925199;
        angsin = sinf(-rad);
        angcos = cosf(-rad);
        for (x=0; x<w; x++) {
            xc = x - w2;
            precx1 = angcos * xc + w2;
            precx2 = angsin * xc + h2;
            for (y=0; y<h; y++) {
                yc = y - h2;
                xs = roundf(precx1 - angsin * yc);
                ys = roundf(precx2 + angcos * yc);
                if (xs >= 0 && xs < w && ys >= 0 && ys < h) {
                    pixel = GrPixelC(src, xs, ys);
                    GrPlot(x, y, pixel);
                } else {
                    GrPlot(x, y, bg);
                }
            }
        }
    }

    GrSetContext(&graux);

    return dst;
}

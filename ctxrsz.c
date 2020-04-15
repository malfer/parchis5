/*
 * ctxrsz.c --- resize context
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
#include <mgrx.h>

#define STB_IMAGE_RESIZE_IMPLEMENTATION
#include "stbimgrs.h"

void StbStretchBltT(GrContext *dst,int dx1,int dy1,int dx2,int dy2,
                    GrContext *src,int x1,int y1,int x2,int y2, GrColor trans)
{
    GrContext grcaux;
    GrColor *dstaux, *srcaux;
    int sw, sh, dw, dh;
    int ret;
    GrColor const *pix;
    int i, j;
    int need_recolor = 0;
    long auxc;

    sw = x2 - x1 + 1;
    sh = y2 - y1 + 1;
    dw = dx2 - dx1 + 1;
    dh = dy2 - dy1 + 1;

    if (sw == dw && sh == dh) {
        GrBitBlt(dst, dx1, dy1, src, x1, y1, x2, y2, GrWRITE);
        return;
    }
    
    if (src->gc_driver->bits_per_pixel < 8) {
        fprintf(stderr, "StbStretchBltT: sorry, only >= 8 bpp supported\n");
        return;
    }

    if (src->gc_driver->bits_per_pixel < 24)
        need_recolor = 1;

    srcaux = malloc(sw*sh*sizeof(GrColor));
    if (srcaux == NULL) {
        fprintf(stderr, "StbStretchBltT: error creating srcaux\n");
        return;
    }

    dstaux = malloc(dw*dh*sizeof(GrColor));
    if (dstaux == NULL) {
        fprintf(stderr, "StbStretchBltT: error creating dstaux\n");
        free(srcaux);
        return;
    }

    for (i=0; i< sh; i++) {
        pix = GrGetScanlineC(src, x1, x2, i+y1);
        for (j=0; j<sw; j++) {
            if (pix[j] == trans)
                srcaux[i*sw+j] = 0x00000000;
            else if (!need_recolor)
                srcaux[i*sw+j] = 0xFF000000 | pix[j];
            else {
                GrQueryColor2(pix[j], &auxc);
                srcaux[i*sw+j] = 0xFF000000 | auxc;
            }
        }
    }
        
    //ret = stbir_resize_uint8((unsigned char *)srcaux, sw, sh, 0,
    //                         (unsigned char *)dstaux, dw, dh, 0, 4);
    ret = stbir_resize_uint8_srgb((unsigned char *)srcaux, sw, sh, 0,
                                  (unsigned char *)dstaux, dw, dh, 0,
                                  4, 3, 0);

    if (ret != 1) {
        fprintf(stderr, "StbStretchBltT returned %d\n", ret);
        goto end;
    }

    GrSaveContext(&grcaux);
    GrSetContext(dst);

    for (i=0; i< dh; i++) {
        for (j=0; j<dw; j++) {
            if ((dstaux[i*dw+j] & 0xFF000000) < 0x70000000)
                dstaux[i*dw+j] = trans;
            else if (!need_recolor)
                dstaux[i*dw+j] = 0x00FFFFFF & dstaux[i*dw+j];
            else
                dstaux[i*dw+j] = 0x00FFFFFF & GrAllocColor2(dstaux[i*dw+j]);
        }
        GrPutScanline(dx1, dx2, i+dy1, &(dstaux[i*dw]), GrWRITE);
    }

    GrSetContext(&grcaux);

end:
    free(srcaux);
    free(dstaux);
}

void StbStretchBlt(GrContext *dst,int dx1,int dy1,int dx2,int dy2,
                   GrContext *src,int x1,int y1,int x2,int y2)
{
    GrContext *dstaux, *srcaux;
    int sw, sh, dw, dh;
    int ret, channels;

    sw = x2 - x1 + 1;
    sh = y2 - y1 + 1;
    dw = dx2 - dx1 + 1;
    dh = dy2 - dy1 + 1;

    if (sw == dw && sh == dh) {
        GrBitBlt(dst, dx1, dy1, src, x1, y1, x2, y2, GrWRITE);
        return;
    }

    switch (src->gc_driver->bits_per_pixel) {
        case 32:
            channels = 4;
            break;
        case 24:
            channels = 3;
            break;
        case 16:
        case 15:
        case 8:
            StbStretchBltT(dst, dx1, dy1, dx2, dy2,
                           src, x1, y1, x2, y2, GrNOCOLOR);
            return;
        default:
            fprintf(stderr, "StbStretchBlt: sorry, only >=8 bpp supported\n");
            return;
    }

    srcaux = GrCreateContext(sw, sh, NULL, NULL);
    if (srcaux == NULL) {
        fprintf(stderr, "StbStretchBlt: error creating srcaux\n");
        return;
    }

    dstaux = GrCreateContext(dw, dh, NULL, NULL);
    if (dstaux == NULL) {
        fprintf(stderr, "StbStretchBlt: error creating dstaux\n");
        free(srcaux);
        return;
    }

    GrBitBlt(srcaux, 0, 0, src, x1, y1, x2, y2, GrWRITE);

    ret = stbir_resize_uint8((unsigned char *)srcaux->gc_baseaddr[0],
                             sw, sh, srcaux->gc_lineoffset,
                             (unsigned char *)dstaux->gc_baseaddr[0],
                             dw, dh, dstaux->gc_lineoffset,
                             channels);
    if (ret == 1) {
        GrBitBlt(dst, dx1, dy1, dstaux, 0, 0, dw-1, dh-1, GrWRITE);
    } else {
        fprintf(stderr, "StbStretchBlt returned %d\n", ret);
    }

    free(srcaux);
    free(dstaux);
}

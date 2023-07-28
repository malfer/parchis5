/*
 * gposgdef.c --- graphics coordinates of diferent places
 * 
 * Copyright (c) 2020,2023 Mariano Alvarez Fernandez
 * (malfer@telefonica.net)
 *
 * This file is part of Parchís5, a popular spanish game
 *
 * Parchís5 is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * Parchís5 is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY); without even the implied warranty of
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
#include <grgui.h>
#include <mgrxcolr.h>
#include "engine.h"
#include "gparchis.h"

GPositions globgpos;

GrContext * CtxRotate(GrContext *src, int angle, GrColor bg);

/***********************/

void set_posg(PosG *pg, int x, int y)
{
    pg->x = ((x * globvar.scale) + 0.5);
    pg->y = ((y * globvar.scale) + 0.5);
}
    
/***********************/

void inicia_globgpos(void)
{
    int i, posx, posy;
    int x1, x2, x3, x4, x5, y1, y2, y3, y4, y5;

    for (i=0; i<8; i++) {
        posx = 818 - (i * 40);
        set_posg(&(globgpos.corr[i][0]), posx, 352);
        set_posg(&(globgpos.corr[i][1]), posx, 304);
    }
    set_posg(&(globgpos.corr[7][1]), posx, 312);
    for (i=8; i<16; i++) {
        posy = 302 - ((i - 8) * 40);
        set_posg(&(globgpos.corr[i][0]), 488, posy);
        set_posg(&(globgpos.corr[i][1]), 536, posy);
    }
    set_posg(&(globgpos.corr[8][1]), 528, 302);
    set_posg(&(globgpos.corr[16][0]), 396, 22);
    set_posg(&(globgpos.corr[16][1]), 444, 22);
    for (i=17; i<25; i++) {
        posy = 22 + ((i - 17) * 40);
        set_posg(&(globgpos.corr[i][0]), 352, posy);
        set_posg(&(globgpos.corr[i][1]), 304, posy);
    }
    set_posg(&(globgpos.corr[24][1]), 312, posy);
    for (i=25; i<33; i++) {
        posx = 302 - ((i - 25) * 40);
        set_posg(&(globgpos.corr[i][0]), posx, 352);
        set_posg(&(globgpos.corr[i][1]), posx, 304);
    }
    set_posg(&(globgpos.corr[25][1]), 302, 312);
    set_posg(&(globgpos.corr[33][0]), 22, 396);
    set_posg(&(globgpos.corr[33][1]), 22, 444);
    for (i=34; i<42; i++) {
        posx = 22 + ((i - 34) * 40);
        set_posg(&(globgpos.corr[i][0]), posx, 488);
        set_posg(&(globgpos.corr[i][1]), posx, 536);
    }
    set_posg(&(globgpos.corr[41][1]), posx, 528);
    for (i=42; i<50; i++) {
        posy = 538 + ((i - 42) * 40);
        set_posg(&(globgpos.corr[i][0]), 352, posy);
        set_posg(&(globgpos.corr[i][1]), 304, posy);
    }
    set_posg(&(globgpos.corr[42][1]), 312, 538);
    set_posg(&(globgpos.corr[50][0]), 396, 818);
    set_posg(&(globgpos.corr[50][1]), 444, 818);
    for (i=51; i<59; i++) {
        posy = 818 - ((i - 51) * 40);
        set_posg(&(globgpos.corr[i][0]), 488, posy);
        set_posg(&(globgpos.corr[i][1]), 536, posy);
    }
    set_posg(&(globgpos.corr[58][1]), 528, posy);
    for (i=59; i<67; i++) {
        posx = 538 + ((i - 59) * 40);
        set_posg(&(globgpos.corr[i][0]), posx, 488);
        set_posg(&(globgpos.corr[i][1]), posx, 536);
    }
    set_posg(&(globgpos.corr[59][1]), 538, 528);
    set_posg(&(globgpos.corr[67][0]), 818, 396);
    set_posg(&(globgpos.corr[67][1]), 818, 444);

    globgpos.casa[0][0].x = globgpos.corr[ 2][0].x;
    globgpos.casa[0][0].y = globgpos.corr[13][0].y;
    globgpos.casa[0][1].x = globgpos.corr[ 2][0].x;
    globgpos.casa[0][1].y = globgpos.corr[11][0].y;
    globgpos.casa[0][2].x = globgpos.corr[ 4][0].x;
    globgpos.casa[0][2].y = globgpos.corr[11][0].y;
    globgpos.casa[0][3].x = globgpos.corr[ 4][0].x;
    globgpos.casa[0][3].y = globgpos.corr[13][0].y;
    globgpos.casa[1][0].x = globgpos.corr[30][0].x;
    globgpos.casa[1][0].y = globgpos.corr[19][0].y;
    globgpos.casa[1][1].x = globgpos.corr[28][0].x;
    globgpos.casa[1][1].y = globgpos.corr[19][0].y;
    globgpos.casa[1][2].x = globgpos.corr[28][0].x;
    globgpos.casa[1][2].y = globgpos.corr[21][0].y;
    globgpos.casa[1][3].x = globgpos.corr[30][0].x;
    globgpos.casa[1][3].y = globgpos.corr[21][0].y;
    globgpos.casa[2][0].x = globgpos.corr[36][0].x;
    globgpos.casa[2][0].y = globgpos.corr[47][0].y;
    globgpos.casa[2][1].x = globgpos.corr[36][0].x;
    globgpos.casa[2][1].y = globgpos.corr[45][0].y;
    globgpos.casa[2][2].x = globgpos.corr[38][0].x;
    globgpos.casa[2][2].y = globgpos.corr[45][0].y;
    globgpos.casa[2][3].x = globgpos.corr[38][0].x;
    globgpos.casa[2][3].y = globgpos.corr[47][0].y;
    globgpos.casa[3][0].x = globgpos.corr[64][0].x;
    globgpos.casa[3][0].y = globgpos.corr[53][0].y;
    globgpos.casa[3][1].x = globgpos.corr[62][0].x;
    globgpos.casa[3][1].y = globgpos.corr[53][0].y;
    globgpos.casa[3][2].x = globgpos.corr[62][0].x;
    globgpos.casa[3][2].y = globgpos.corr[55][0].y;
    globgpos.casa[3][3].x = globgpos.corr[64][0].x;
    globgpos.casa[3][3].y = globgpos.corr[55][0].y;

    for (i=0; i<7; i++) {
        posx = 778 - (i * 40);
        set_posg(&(globgpos.lleg[0][i][0]), posx, 396);
        set_posg(&(globgpos.lleg[0][i][1]), posx, 444);
    }
    for (i=0; i<7; i++) {
        posy = 62 + (i * 40);
        set_posg(&(globgpos.lleg[1][i][0]), 396, posy);
        set_posg(&(globgpos.lleg[1][i][1]), 444, posy);
    }
    for (i=0; i<7; i++) {
        posx = 62 + (i * 40);
        set_posg(&(globgpos.lleg[2][i][0]), posx, 396);
        set_posg(&(globgpos.lleg[2][i][1]), posx, 444);
    }
    for (i=0; i<7; i++) {
        posy = 778 - (i * 40);
        set_posg(&(globgpos.lleg[3][i][0]), 396, posy);
        set_posg(&(globgpos.lleg[3][i][1]), 444, posy);
    }

    x1 = 342; x2 = 382; x3 = 422; x4 = 462; x5 = 498;
    y1 = 342; y2 = 382; y3 = 422; y4 = 462; y5 = 498;
    set_posg(&(globgpos.meta[0][0]), x5, y2);
    set_posg(&(globgpos.meta[0][1]), x5, y3);
    set_posg(&(globgpos.meta[0][2]), x5, y4);
    set_posg(&(globgpos.meta[0][3]), x4-4, y3);
    set_posg(&(globgpos.meta[1][0]), x2, y1);
    set_posg(&(globgpos.meta[1][1]), x3, y1);
    set_posg(&(globgpos.meta[1][2]), x4, y1);
    set_posg(&(globgpos.meta[1][3]), x3, y2);
    set_posg(&(globgpos.meta[2][0]), x1, y2);
    set_posg(&(globgpos.meta[2][1]), x1, y3);
    set_posg(&(globgpos.meta[2][2]), x1, y4);
    set_posg(&(globgpos.meta[2][3]), x2, y3);
    set_posg(&(globgpos.meta[3][0]), x2, y5);
    set_posg(&(globgpos.meta[3][1]), x3, y5);
    set_posg(&(globgpos.meta[3][2]), x4, y5);
    set_posg(&(globgpos.meta[3][3]), x3, y4-4);

    set_posg(&(globgpos.medalC[0]), x4-15, y3);
    set_posg(&(globgpos.medalC[1]), x3, y2-13);
    set_posg(&(globgpos.medalC[2]), x2-13, y3);
    set_posg(&(globgpos.medalC[3]), x3, y4-15);

    set_posg(&(globgpos.medal[0]), 673, 115);
    set_posg(&(globgpos.medal[1]), 117, 115);
    set_posg(&(globgpos.medal[2]), 117, 672);
    set_posg(&(globgpos.medal[3]), 673, 672);

    set_posg(&(globgpos.muneco[0]), 760, 24);
    set_posg(&(globgpos.muneco[1]), 24, 24);
    set_posg(&(globgpos.muneco[2]), 24, 760);
    set_posg(&(globgpos.muneco[3]), 760, 760);

    set_posg(&(globgpos.podium), 305, 592);
    set_posg(&(globgpos.podpos[0]), 392, 649);
    set_posg(&(globgpos.podpos[1]), 304, 675);
    set_posg(&(globgpos.podpos[2]), 480, 693);
    set_posg(&(globgpos.podpos[3]), 568, 763);

    set_posg(&(globgpos.podiump), 275, 532);
    set_posg(&(globgpos.podppos[0]), 272, 675);
    set_posg(&(globgpos.podppos[1]), 350, 675);
    set_posg(&(globgpos.podppos[2]), 432, 693);
    set_posg(&(globgpos.podppos[3]), 510, 693);

//    set_posg(&(globgpos.moviola), 410, 410);
    set_posg(&(globgpos.moviola), 386, 386);
}

/***********************/

void get_xy_from_posficha(int *x, int *y, PosFicha *pf, int c)
{
    if (pf->place == CASA) {
        *x = globgpos.casa[c][pf->ind].x;
        *y = globgpos.casa[c][pf->ind].y;
    } else if (pf->place == CORR) {
        *x = globgpos.corr[pf->pos][pf->ind].x;
        *y = globgpos.corr[pf->pos][pf->ind].y;
    } else if (pf->place == LLEG) {
        *x = globgpos.lleg[c][pf->pos][pf->ind].x;
        *y = globgpos.lleg[c][pf->pos][pf->ind].y;
    } else { // meta
        *x = globgpos.meta[c][pf->ind].x;
        *y = globgpos.meta[c][pf->ind].y;
    }
}

/***********************/

void set_gposslide_from_posfichas(GPosSlide *gps, PosFicha *pfi,
                                  PosFicha *pff, int c, int steps)
{
    int xi, yi, xf, yf;

    get_xy_from_posficha(&xi, &yi, pfi, c);
    get_xy_from_posficha(&xf, &yf, pff, c);
    gps->xorg = xi;
    gps->yorg = yi;
    gps->deltax = (xf - xi) / steps;
    gps->deltay = (yf - yi) / steps;
}

/***********************/

void set_gposslide_for_medals(GPosSlide *gps, int c, int steps)
{
    int xi, yi, xf, yf;

    xi = globgpos.medalC[c].x;
    yi = globgpos.medalC[c].y;
    xf = globgpos.medal[c].x;
    yf = globgpos.medal[c].y;
    gps->xorg = xi;
    gps->yorg = yi;
    gps->deltax = (xf - xi) / steps;
    gps->deltay = (yf - yi) / steps;
}

/***********************/

void get_xy_from_gposslide(int *x, int *y, GPosSlide *gps, int step)
{
    *x = gps->xorg + gps->deltax * step;
    *y = gps->yorg + gps->deltay * step;
}

/***********************/

void calc_playable_pawn_positions(void)
{
    int i;

    if (globpt.status != PST_WAITJG) return;
    if (globpt.pp.dp.playertype[globpt.pp.turno] != PERSON) return;
    
    for (i=0; i<globpt.gjg.njg; i++) {
        get_xy_from_posficha(&(globgpos.jgorig[i].x), &(globgpos.jgorig[i].y),
                             &(globpt.gjg.jg[i].origen), globpt.pp.turno);
        get_xy_from_posficha(&(globgpos.jgdest[i].x), &(globgpos.jgdest[i].y),
                             &(globpt.gjg.jg[i].destino), globpt.pp.turno);
        if (globpt.gjg.jg[i].entra2) {
            get_xy_from_posficha(&(globgpos.jgorig2[i].x), &(globgpos.jgorig2[i].y),
                                 &(globpt.gjg.jg[i].origen2), globpt.pp.turno);
            get_xy_from_posficha(&(globgpos.jgdest2[i].x), &(globgpos.jgdest2[i].y),
                                 &(globpt.gjg.jg[i].destino2), globpt.pp.turno);
        }
    }
}

/***********************/

void calc_gcoord_rottbl(int *x, int *y)
{
    int w, h;
    float rad, angsin, angcos;
    float xc, yc, w2, h2;
    
    w = h = globvar.tbldim;
    w2 = h2 = (w - 1) / 2.0;
    rad = globcfg.rotang * 0.0174532925199;
    angsin = sinf(rad);
    angcos = cosf(rad);

    xc = *x - w2;
    yc = *y - h2;
    *x = roundf(angcos * xc - angsin * yc + w2);
    *y = roundf(angsin * xc + angcos * yc + h2);
}
    
/***********************/

void test_fichas(void)
{
    int i, j, k;
    int x, y;
    GrContext *pawn, *rotpawn;

    for (i=0; i<4; i++) {
        rotpawn = CtxRotate(imgpawnsad[i], -globcfg.rotang, TRANSPARENT & 0xFFFFFF);
        for (j=0; j<4; j++) {
            x = globgpos.casa[i][j].x;
            y = globgpos.casa[i][j].y;
            GrBitBlt(NULL, x, y, rotpawn, 0, 0,
                     rotpawn->gc_xmax, rotpawn->gc_ymax, TRANSPARENT);
        }
        GrDestroyContext(rotpawn);
    }

    for (i=0; i<4; i++) {
        rotpawn = CtxRotate(imgpawnhapy[i], -globcfg.rotang, TRANSPARENT & 0xFFFFFF);
        for (j=0; j<4; j++) {
            x = globgpos.meta[i][j].x;
            y = globgpos.meta[i][j].y;
            GrBitBlt(NULL, x, y, rotpawn, 0, 0,
                     rotpawn->gc_xmax, rotpawn->gc_ymax, TRANSPARENT);
        }
        GrDestroyContext(rotpawn);
    }

    for (i=0; i<4; i++) {
        for (j=0; j<7; j++) {
            pawn = imgpawncruz[i];
            if ((j%2) == 1) pawn = imgpawndot[i];
            for (k=0; k<2; k++) {
                x = globgpos.lleg[i][j][k].x;
                y = globgpos.lleg[i][j][k].y;
                GrBitBlt(NULL, x, y, pawn, 0, 0,
                         pawn->gc_xmax, pawn->gc_ymax, TRANSPARENT);
            }
        }
    }

    for (i=0; i<68; i++) {
        pawn = imgpawn[i%4]; 
        for (j=0; j<2; j++) {
            x = globgpos.corr[i][j].x;
            y = globgpos.corr[i][j].y;
            GrBitBlt(NULL, x, y, pawn, 0, 0,
                     pawn->gc_xmax, pawn->gc_ymax, TRANSPARENT);
        }
    }
}

/*
 * gpaint.c --- paint functions
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
#include <grgui.h>
#include <mgrxcolr.h>
#include "engine.h"
#include "gparchis.h"

GrContext * CtxRotate(GrContext *src, int angle, GrColor bg);

/***********************/

void paint_board(void)
{
    GrContext grcaux;
    GrContext *rottbl;

    GrSaveContext(&grcaux);
    GrSetContext(globvar.tblpaint);
    
    GrBitBlt(NULL, 0, 0, imgtbl, 0, 0, imgtbl->gc_xmax, imgtbl->gc_ymax, GrWRITE);

    if (globcfg.testingfichas)
        test_fichas();
    else
        paint_fichas();

    if (globgstatus.oncarrera)
        paint_carrera();

    rottbl = CtxRotate(globvar.tblpaint, globcfg.rotang, WEBC_SEAGREEN);
    if (!rottbl) {
        disaster("rotating board");
    }

    GrSetContext(rottbl);
    paint_munecos2();
    paint_medals2();
    paint_podium();
    paint_moviola();

    GrSetContext(globvar.tbl);
    GrBitBlt(NULL, 0, 0, rottbl, 0, 0, rottbl->gc_xmax, rottbl->gc_ymax, GrWRITE);
    GrDestroyContext(rottbl);
    GUIDBCurCtxBltToScreen();

    GrSetContext(&grcaux);
}

/***********************/

void paint_moviola(void)
{
    if (globonmoviola == 0) return;

    GrBitBlt(NULL, globgpos.moviola.x, globgpos.moviola.y, imgmoviola, 0, 0,
                 imgmoviola->gc_xmax, imgmoviola->gc_ymax, TRANSPARENT);
}

/***********************/

void paint_munecos(void)
{
    int c;
    int x, y;
    GrContext *mun;
    GrContext *rotmun;

    if (globpt.status == PST_ENDGAME) return;

    for (c=AMAR; c<=VERD; c++) {
        if (globpt.pp.dp.playertype[c] == NOPLAY) continue;
        x = globgpos.muneco[c].x;
        y = globgpos.muneco[c].y;
        mun = (globpt.pp.turno == c) ? imgmunecobl[c] : imgmuneco[c];
        rotmun = CtxRotate(mun, -globcfg.rotang, TRANSPARENT & 0xFFFFFF);
        GrBitBlt(NULL, x, y, rotmun, 0, 0,
                 rotmun->gc_xmax, rotmun->gc_ymax, TRANSPARENT);
        GrDestroyContext(rotmun);
    }
}

/***********************/

void paint_munecos2(void)
{
    int c;
    int x, y, w2;
    GrContext *mun;

    if (globpt.status == PST_ENDGAME) return;

    w2 =  globvar.munecodim / 2;
    for (c=AMAR; c<=VERD; c++) {
        if (globpt.pp.dp.playertype[c] == NOPLAY) continue;
        x = globgpos.muneco[c].x + w2;
        y = globgpos.muneco[c].y + w2;
        calc_gcoord_rottbl(&x, &y);
        x -= w2;
        y -= w2;
        mun = (globpt.pp.turno == c) ? imgmunecobl[c] : imgmuneco[c];
        GrBitBlt(NULL, x, y, mun, 0, 0,
                 mun->gc_xmax, mun->gc_ymax, TRANSPARENT);
    }
}

/***********************/

void paint_fichas(void)
{
    PosFicha *pf;
    int c, j, k;
    int x, y;
    GrContext *pawn;

    for (c=AMAR; c<=VERD; c++) {
        if (globpt.pp.dp.playertype[c] == NOPLAY) continue;
        pf = &(globpt.pp.pf[c][0]);
        for (j=0; j<4; j++) {
            if (globgstatus.oncarrera && globcar.jg.cjugador == c) {
                if (globcar.jg.nficha == j) continue;
                if (globcar.jg.entra2 && globcar.jg.nficha2 == j) continue;
            }
            if (globgstatus.mousingpawn && globpt.pp.turno == c) {
                if (globpt.gjg.jg[globpt.njgselec].nficha == j) continue;
                if (globpt.gjg.jg[globpt.njgselec].entra2 &&
                    (globpt.gjg.jg[globpt.njgselec].nficha2 == j)) continue;
            }
            get_xy_from_posficha(&x, &y, &(pf[j]), c);
            pawn = imgpawn[c];
            if (!globgstatus.blinkpawn &&
                globpt.status == PST_WAITJG && c == globpt.pp.turno) {
                for (k=0; k<globpt.gjg.njg; k++) {
                    if (globpt.pp.dp.playertype[c] == PERSON) {
                        if (j == globpt.gjg.jg[k].nficha)
                            pawn = imgpawndot[c];
                    }
                    if (k == globpt.njgselec) {
                        if (j == globpt.gjg.jg[k].nficha ||
                            (globpt.gjg.jg[k].entra2 &&
                             j == globpt.gjg.jg[k].nficha2)) {
                            pawn = imgpawncruz[c];
                            break;
                        }
                    }
                }
            }
            GrBitBlt(NULL, x, y, pawn, 0, 0,
                     pawn->gc_xmax, pawn->gc_ymax, TRANSPARENT);
        }
    }

    if (globpt.status == PST_WAITJG) {
        pawn = imgpawndest[0];
        if (globgstatus.blinkpawn || globgstatus.pawnoverdest)
            pawn = imgpawndest[1];
        pf = &(globpt.gjg.jg[globpt.njgselec].destino);
        get_xy_from_posficha(&x, &y, pf, globpt.gjg.jg[globpt.njgselec].cjugador);
        GrBitBlt(NULL, x, y, pawn, 0, 0, pawn->gc_xmax, pawn->gc_ymax, TRANSPARENT);
        if (globpt.gjg.jg[globpt.njgselec].entra2) {
            pf = &(globpt.gjg.jg[globpt.njgselec].destino2);
            get_xy_from_posficha(&x, &y, pf, globpt.gjg.jg[globpt.njgselec].cjugador);
            GrBitBlt(NULL, x, y, pawn, 0, 0, pawn->gc_xmax, pawn->gc_ymax, TRANSPARENT);
        }
    }

    if (globgstatus.mousingpawn) {
        c = globpt.pp.turno;
        pawn = imgpawncruz[c];
        GrBitBlt(NULL, globgpos.mouse.x, globgpos.mouse.y, pawn, 0, 0,
                 pawn->gc_xmax, pawn->gc_ymax, TRANSPARENT);
        if (globpt.gjg.jg[globpt.njgselec].entra2) {
            if (c == AMAR) {
                x = globgpos.mouse.x;
                y = globgpos.mouse.y - globvar.pawndim;
            } else if (c == AZUL) {
                x = globgpos.mouse.x - globvar.pawndim;
                y = globgpos.mouse.y;
            } else if (c == ROJO) {
                x = globgpos.mouse.x;
                y = globgpos.mouse.y + globvar.pawndim;
            } else { // VERD
                x = globgpos.mouse.x + globvar.pawndim;
                y = globgpos.mouse.y;
            }
            GrBitBlt(NULL, x, y, pawn, 0, 0, pawn->gc_xmax, pawn->gc_ymax, TRANSPARENT);
        }
    }
}

/***********************/

void paint_carrera(void)
{
    int c, ck, x, y;
    GrContext *pawn, *pawnk, *rotpawn, *rotpawnk;
    int angle;

    if (globcar.mode == 0 && globcar.step < globcar.stepsentra) {
        c = globcar.jg.cjugador;
        get_xy_from_gposslide(&x, &y, &(globcar.gpsentra), globcar.step);
        pawn = imgpawn[c];
        GrBitBlt(NULL, x, y, pawn, 0, 0,
                 pawn->gc_xmax, pawn->gc_ymax, TRANSPARENT);
        if (globcar.jg.entra2) {
            get_xy_from_gposslide(&x, &y, &(globcar.gpsentra2), globcar.step);
            GrBitBlt(NULL, x, y, pawn, 0, 0,
                     pawn->gc_xmax, pawn->gc_ymax, TRANSPARENT);
        }
    }
 
    if (globcar.mode == 1 && globcar.step < globcar.stepspasos) {
        c = globcar.jg.cjugador;
        get_xy_from_posficha(&x, &y, &(globcar.paso[globcar.step]), c);
        pawn = imgpawn[c];
        GrBitBlt(NULL, x, y, pawn, 0, 0,
                 pawn->gc_xmax, pawn->gc_ymax, TRANSPARENT);
    }

    if (globcar.mode == 2 && globcar.step < globcar.stepscome) {
        c = globcar.jg.cjugador;
        get_xy_from_posficha(&x, &y, &(globcar.jg.destino), c);
        pawn = imgpawnhapy[c];
        rotpawn = CtxRotate(pawn, -globcfg.rotang, TRANSPARENT & 0xFFFFFF);
        GrBitBlt(NULL, x, y, rotpawn, 0, 0,
                 rotpawn->gc_xmax, rotpawn->gc_ymax, TRANSPARENT);
        ck = globcar.jg.cjugdcom;
        get_xy_from_gposslide(&x, &y, &(globcar.gpscome), globcar.step);
        pawnk = imgpawnsad[ck];
        rotpawnk = CtxRotate(pawnk, -globcfg.rotang, TRANSPARENT & 0xFFFFFF);
        GrBitBlt(NULL, x, y, rotpawnk, 0, 0,
                 rotpawnk->gc_xmax, rotpawnk->gc_ymax, TRANSPARENT);
        GrDestroyContext(rotpawnk);
        if (globcar.jg.come2) {
            get_xy_from_posficha(&x, &y, &(globcar.jg.destino2), c);
            GrBitBlt(NULL, x, y, rotpawn, 0, 0,
                     rotpawn->gc_xmax, rotpawn->gc_ymax, TRANSPARENT);
            ck = globcar.jg.cjugdcom2;
            get_xy_from_gposslide(&x, &y, &(globcar.gpscome2), globcar.step);
            pawnk = imgpawnsad[ck];
            rotpawnk = CtxRotate(pawnk, -globcfg.rotang, TRANSPARENT & 0xFFFFFF);
            GrBitBlt(NULL, x, y, rotpawnk, 0, 0,
                     rotpawnk->gc_xmax, rotpawnk->gc_ymax, TRANSPARENT);
            GrDestroyContext(rotpawnk);
        }
        GrDestroyContext(rotpawn);
    }

    if (globcar.mode == 3 && globcar.step < globcar.stepsacasa) {
        c = globcar.jg.cjugador;
        get_xy_from_gposslide(&x, &y, &(globcar.gpsacasa), globcar.step);
        pawn = imgpawnsad[c];
        rotpawn = CtxRotate(pawn, -globcfg.rotang, TRANSPARENT & 0xFFFFFF);
        GrBitBlt(NULL, x, y, rotpawn, 0, 0,
                 rotpawn->gc_xmax, rotpawn->gc_ymax, TRANSPARENT);
        GrDestroyContext(rotpawn);
    }

    if (globcar.mode == 4 && globcar.step < globcar.stepsmeta) {
        //printf("en 4\n");
        c = globcar.jg.cjugador;
        get_xy_from_posficha(&x, &y, &(globcar.jg.destino), c);
        angle = -globcfg.rotang + 360 / (globcar.stepsmeta-1) * globcar.step;
        pawn = imgpawnhapy[c];
        rotpawn = CtxRotate(pawn, angle, TRANSPARENT & 0xFFFFFF);
        GrBitBlt(NULL, x, y, rotpawn, 0, 0,
                 rotpawn->gc_xmax, rotpawn->gc_ymax, TRANSPARENT);
        GrDestroyContext(rotpawn);
    }

    if (globcar.mode == 5 && globcar.step < globcar.stepsfin) {
        //printf("en 5\n");
        get_xy_from_gposslide(&x, &y, &(globcar.gpsfin), globcar.step);
        angle = -globcfg.rotang + 360 / globcar.stepsfin * globcar.step;
        pawn = imgmedal[globpt.pp.nextending]; // not really a pawn!!
        rotpawn = CtxRotate(pawn, angle, TRANSPARENT & 0xFFFFFF);
        GrBitBlt(NULL, x, y, rotpawn, 0, 0,
                 rotpawn->gc_xmax, rotpawn->gc_ymax, TRANSPARENT);
        if (globpt.pp.dp.gametype == GT_PAIRS) {
            get_xy_from_gposslide(&x, &y, &(globcar.gpsfin2), globcar.step);
            GrBitBlt(NULL, x, y, rotpawn, 0, 0,
                     rotpawn->gc_xmax, rotpawn->gc_ymax, TRANSPARENT);
        }
        GrDestroyContext(rotpawn);
    }
}

/***********************/

void paint_medals(void)
{
    int c, pos;
    int x, y;
    GrContext *medal;
    GrContext *rotmed;

    for (c=AMAR; c<=VERD; c++) {
        pos = globpt.pp.endingpos[c];
        if (pos >= 0) {
            x = globgpos.medal[c].x;
            y = globgpos.medal[c].y;
            if (globpt.pp.dp.gametype == GT_SINGLE) {
                medal = imgmedal[pos];
            } else if (pos < 2) {
                medal = imgmedal[0];
            } else {
                medal = imgmedal[1];
            }
            rotmed = CtxRotate(medal, -globcfg.rotang, TRANSPARENT & 0xFFFFFF);
            GrBitBlt(NULL, x, y, rotmed, 0, 0,
                     rotmed->gc_xmax, rotmed->gc_ymax, TRANSPARENT);
            GrDestroyContext(rotmed);
        }
    }
}

/***********************/

void paint_medals2(void)
{
    int c, pos;
    int x, y, w2;
    GrContext *medal;

    w2 =  globvar.medaldim / 2;
    for (c=AMAR; c<=VERD; c++) {
        pos = globpt.pp.endingpos[c];
        if (pos >= 0) {
            x = globgpos.medal[c].x + w2;
            y = globgpos.medal[c].y + w2;
            calc_gcoord_rottbl(&x, &y);
            x -= w2;
            y -= w2;
            if (globpt.pp.dp.gametype == GT_SINGLE) {
                medal = imgmedal[pos];
            } else if (pos < 2) {
                medal = imgmedal[0];
            } else {
                medal = imgmedal[1];
            }
            GrBitBlt(NULL, x, y, medal, 0, 0,
                     medal->gc_xmax, medal->gc_ymax, TRANSPARENT);
        }
    }
}

/***********************/

void paint_podium(void)
{
    int x, y, i, c;
    GrContext *mun;

    if (globpt.status != PST_ENDGAME) return;

    if (globpt.pp.dp.gametype == GT_SINGLE) {
        x = globgpos.podium.x;
        y = globgpos.podium.y;
        GrBitBlt(NULL, x, y, imgpodium, 0, 0,
                 imgpodium->gc_xmax, imgpodium->gc_ymax, TRANSPARENT);
        for (i=0; i<=4; i++) {
            for (c=AMAR; c<=VERD; c++) {
                if (globpt.pp.endingpos[c] == i) {
                    x = globgpos.podpos[i].x;
                    y = globgpos.podpos[i].y;
                    if (globpt.pp.dp.playertype[c] == PERSON) y -= 2;
                    mun = (i == 0) ? imgmunecobl[c] : imgmuneco[c];
                    GrBitBlt(NULL, x, y, mun, 0, 0,
                            mun->gc_xmax, mun->gc_ymax, TRANSPARENT);
                }
            }
        }
    } else {
        x = globgpos.podiump.x;
        y = globgpos.podiump.y;
        GrBitBlt(NULL, x, y, imgpodiump, 0, 0,
                 imgpodiump->gc_xmax, imgpodiump->gc_ymax, TRANSPARENT);
        for (i=0; i<=4; i++) {
            for (c=AMAR; c<=VERD; c++) {
                if (globpt.pp.endingpos[c] == i) {
                    x = globgpos.podppos[i].x;
                    y = globgpos.podppos[i].y;
                    if (globpt.pp.dp.playertype[c] == PERSON) y -= 2;
                    mun = (i < 2) ? imgmunecobl[c] : imgmuneco[c];
                    GrBitBlt(NULL, x, y, mun, 0, 0,
                            mun->gc_xmax, mun->gc_ymax, TRANSPARENT);
                }
            }
        }
    }
}

/***********************/

static void paint_progres_bar(int x, int y, int p, GrColor c)
{
    static GrTextOption grt;
    static int grt_inited = 0;
    char s[20];

    if (!grt_inited) {
        grt.txo_font = &GrFont_PX8x18;
        grt.txo_fgcolor = GrWhite() | GrXOR;
        grt.txo_bgcolor = GrNOCOLOR;
        grt.txo_chrtype = GR_UTF8_TEXT;
        grt.txo_direct = GR_TEXT_RIGHT;
        grt.txo_xalign = GR_ALIGN_CENTER;
        grt.txo_yalign = GR_ALIGN_CENTER;
        grt_inited = 1;
    }
    
    GrFilledBox(x, y, x+176, y+20, WEBC_LEMONCHIFFON);
    if (p >= 0) {
        GrFilledBox(x, y, x+(176*p)/100, y+20, c);
        sprintf(s, "%d%%", p);
        GrDrawString(s, 0, x+90, y+10, &grt);
    }
    GrBox(x, y, x+176, y+20, WEBC_BLACK);
}

/***********************/

void paint_progres(void)
{
    GrContext grcaux;
    int c, p, h;

    h = (globvar.tbldim > SMALLTBL) ? 30 : 22;
    GrSaveContext(&grcaux);
    GrSetContext(globvar.pgb);
    for (c=AMAR; c<=VERD; c++) {
        if (globpt.pp.dp.playertype[c] != NOPLAY)
            p = ((320 - globpt.pp.distance[c]) * 100) / 320;
        else
            p = -1;
        paint_progres_bar(12,  c*h, p, playercolor[c]);
    }
    GUIDBCurCtxBltToScreen();
    GrSetContext(&grcaux);
}

/***********************/

void paint_dice(int val, GrColor bg)
{
    GrContext grcaux;
    GrContext *img;
    int x1, y1;

    if (val == 10) val = 8;
    if (val == 20) val = 9;
    if (val < 0 || val > 9) val = 0;
    img = imgdice[val];
    GrSaveContext(&grcaux);
    GrSetContext(globvar.dce);
    GrBox(0, 0, GrMaxX(), GrMaxY(), WEBC_ORANGE);
    GrBox(1, 1, GrMaxX()-1, GrMaxY()-1, WEBC_ORANGE);
    GrFilledBox(2, 2, GrMaxX()-2, GrMaxY()-2, bg);
    x1 = GrSizeX() / 2 - img->gc_xmax / 2;
    y1 = GrSizeY() / 2 - img->gc_ymax / 2;
    GrBitBlt(NULL, x1, y1, img, 0, 0, img->gc_xmax, img->gc_ymax, TRANSPARENT);
    GUIDBCurCtxBltToScreen();
    GrSetContext(&grcaux);
}

/***********************/

void paint_instruction(char *l1, char *l2)
{
    GrContext grcaux;
    int xcenter, ycenter, yl1, yl2;
    static GrTextOption grt;
    static int grt_inited = 0;

    if (!grt_inited) {
        grt.txo_font = &GrFont_PX8x18;
        grt.txo_fgcolor = GrBlack();
        grt.txo_bgcolor = GrNOCOLOR;
        grt.txo_chrtype = GR_UTF8_TEXT;
        grt.txo_direct = GR_TEXT_RIGHT;
        grt.txo_xalign = GR_ALIGN_CENTER;
        grt.txo_yalign = GR_ALIGN_CENTER;
        grt_inited = 1;
    }

    GrSaveContext(&grcaux);
    GrSetContext(globvar.ins);
    GrBox(0, -2, GrMaxX(), GrMaxY(), WEBC_ORANGE);
    GrBox(1, -1, GrMaxX()-1, GrMaxY()-1, WEBC_ORANGE);
    GrFilledBox(2, 0, GrMaxX()-2, GrMaxY()-2, WEBC_LEMONCHIFFON);

    xcenter = GrMaxX() / 2;
    ycenter = GrMaxY() / 2;
    yl1 = yl2 = ycenter;
    if (l2) {
        yl1 = ycenter - 10;
        yl2 = ycenter + 10;
    }
    if (l1) GrDrawString(l1, 0, xcenter, yl1, &grt);
    if (l2) GrDrawString(l2, 0, xcenter, yl2, &grt);

    GUIDBCurCtxBltToScreen();
    GrSetContext(&grcaux);
}


/*
 * gloadimg.c --- load images
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

#define TITULO "pfiles/titulob.png"
#define TABLERO0 "pfiles/tblbasic.png"
#define TABLERO1 "pfiles/tblenhan.png"
#define TABLERO2 "pfiles/tblmarbl.png"
#define FAMAR "pfiles/famar.png"
#define FAZUL "pfiles/fazul.png"
#define FROJA "pfiles/froja.png"
#define FVERD "pfiles/fverd.png"
#define FAMARCRUZ "pfiles/famarcrz.png"
#define FAZULCRUZ "pfiles/fazulcrz.png"
#define FROJACRUZ "pfiles/frojacrz.png"
#define FVERDCRUZ "pfiles/fverdcrz.png"
#define FAMARDOT "pfiles/famardot.png"
#define FAZULDOT "pfiles/fazuldot.png"
#define FROJADOT "pfiles/frojadot.png"
#define FVERDDOT "pfiles/fverddot.png"
#define FAMARHAPY "pfiles/famarhap.png"
#define FAZULHAPY "pfiles/fazulhap.png"
#define FROJAHAPY "pfiles/frojahap.png"
#define FVERDHAPY "pfiles/fverdhap.png"
#define FAMARSAD "pfiles/famarsad.png"
#define FAZULSAD "pfiles/fazulsad.png"
#define FROJASAD "pfiles/frojasad.png"
#define FVERDSAD "pfiles/fverdsad.png"
#define FDEST1 "pfiles/fdest1.png"
#define FDEST2 "pfiles/fdest2.png"
#define BARRH "pfiles/barrerah.png"
#define BARRV "pfiles/barrerav.png"
#define DADOX "pfiles/xdadox.png"
#define DADO1 "pfiles/xdado1.png"
#define DADO2 "pfiles/xdado2.png"
#define DADO3 "pfiles/xdado3.png"
#define DADO4 "pfiles/xdado4.png"
#define DADO5 "pfiles/xdado5.png"
#define DADO6 "pfiles/xdado6.png"
#define DADO7 "pfiles/xdado7.png"
#define DADO10 "pfiles/xdado10.png"
#define DADO20 "pfiles/xdado20.png"
#define SDADOX "pfiles/dadox.png"
#define SDADO1 "pfiles/dado1.png"
#define SDADO2 "pfiles/dado2.png"
#define SDADO3 "pfiles/dado3.png"
#define SDADO4 "pfiles/dado4.png"
#define SDADO5 "pfiles/dado5.png"
#define SDADO6 "pfiles/dado6.png"
#define SDADO7 "pfiles/dado7.png"
#define SDADO10 "pfiles/dado10.png"
#define SDADO20 "pfiles/dado20.png"
#define MEDAL1 "pfiles/el1.png"
#define MEDAL2 "pfiles/el2.png"
#define MEDAL3 "pfiles/el3.png"
#define MEDAL4 "pfiles/el4.png"
#define DIBPERSON "pfiles/person.png"
#define DIBROBOT "pfiles/robot.png"
#define DIBPERSONBL "pfiles/personbl.png"
#define DIBROBOTBL "pfiles/robotbl.png"
#define PODIUM "pfiles/podium.png"
#define PODIUMP "pfiles/podiump.png"
#define MOVIOLA "pfiles/moviola3.png"
#define BARRERAH "pfiles/barrerah.png"
#define BARRERAV "pfiles/barrerav.png"

GrContext *imgtbl = NULL;
GrContext *imgtitle = NULL;
GrContext *imgdice[10] = {NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL};
GrContext *imgpawn[4] = {NULL, NULL, NULL, NULL};
GrContext *imgpawncruz[4] = {NULL, NULL, NULL, NULL};
GrContext *imgpawndot[4] = {NULL, NULL, NULL, NULL};
GrContext *imgpawndest[2] = {NULL, NULL};
GrContext *imgpawnhapy[4] = {NULL, NULL, NULL, NULL};
GrContext *imgpawnsad[4] = {NULL, NULL, NULL, NULL};
GrContext *imgmedal[4] = {NULL, NULL, NULL, NULL};
GrContext *imgmuneco[4] = {NULL, NULL, NULL, NULL};
GrContext *imgmunecobl[4] = {NULL, NULL, NULL, NULL};
GrContext *imgpodium = NULL;
GrContext *imgpodiump = NULL;
GrContext *imgmoviola = NULL;
GrContext *imgbarrerah = NULL;
GrContext *imgbarrerav = NULL;

static char *tblfn[3] = {TABLERO0, TABLERO1, TABLERO2};
static GrContext *imgperson = NULL;
static GrContext *imgrobot = NULL;
static GrContext *imgpersonbl = NULL;
static GrContext *imgrobotbl = NULL;

void StbStretchBlt(GrContext *dst,int dx1,int dy1,int dx2,int dy2,
                   GrContext *src,int x1,int y1,int x2,int y2);
void StbStretchBltT(GrContext *dst,int dx1,int dy1,int dx2,int dy2,
                    GrContext *src,int x1,int y1,int x2,int y2, GrColor trans);

/***********************/

static GrContext * load_img(char *fname, int trans)
{
    GrContext *imgctx = NULL;
    int imgloaded = 0;
    int imgwidth, imgheight;
    char s[81];

    if (GrQueryPng(fname, &imgwidth, &imgheight) == 0) {
        imgctx = GrCreateContext(imgwidth, imgheight, NULL, NULL);
        if (imgctx != NULL) {
            if (trans) {
                GrClearContextC(imgctx, TRANSPARENT);
            }
            if (GrLoadContextFromPng(imgctx, fname, trans) == 0) {
                imgloaded = 1;
            }
        }
    }
    if (!imgloaded) {
        sprintf(s, "loading image %s", fname);
        disaster(s);
    }

    return imgctx;
}

/***********************/

void load_board(void)
{
    GrContext *aux;

    if (imgtbl) GrDestroyContext(imgtbl);
    imgtbl = GrCreateContext(globvar.tbldim, globvar.tbldim, NULL, NULL);
    if (imgtbl == NULL) {
        disaster("creating imgtbl");
    }
    aux = load_img(tblfn[globcfg.nboard], 0);
    //GrStretchBlt(imgtbl, 0, 0, globvar.tbldim-1, globvar.tbldim-1, aux,
    //             0, 0, aux->gc_xmax, aux->gc_ymax, GrWRITE);
    StbStretchBlt(imgtbl, 0, 0, globvar.tbldim-1, globvar.tbldim-1,
                  aux, 0, 0, aux->gc_xmax, aux->gc_ymax);
    GrDestroyContext(aux);
    
}

/***********************/

void load_images(void)
{
    GrContext *aux;
    int i;

    imgtitle = load_img(TITULO, 0);

    load_board();

    for (i=0; i<4; i++) {
        if (imgpawn[i]) GrDestroyContext(imgpawn[i]);
        imgpawn[i] = GrCreateContext(globvar.pawndim, globvar.pawndim, NULL, NULL);
        if (imgpawn[i] == NULL) {
            disaster("creating imgpawn");
        }
    }
    aux = load_img(FAMAR, 1);
    StbStretchBltT(imgpawn[0], 0, 0, globvar.pawndim-1, globvar.pawndim-1,
                   aux, 0, 0, aux->gc_xmax, aux->gc_ymax, TRANSPARENT&0x00FFFFFF);
    GrDestroyContext(aux);
    aux = load_img(FAZUL, 1);
    StbStretchBltT(imgpawn[1], 0, 0, globvar.pawndim-1, globvar.pawndim-1,
                   aux, 0, 0, aux->gc_xmax, aux->gc_ymax, TRANSPARENT&0x00FFFFFF);
    GrDestroyContext(aux);
    aux = load_img(FROJA, 1);
    StbStretchBltT(imgpawn[2], 0, 0, globvar.pawndim-1, globvar.pawndim-1,
                   aux, 0, 0, aux->gc_xmax, aux->gc_ymax, TRANSPARENT&0x00FFFFFF);
    GrDestroyContext(aux);
    aux = load_img(FVERD, 1);
    StbStretchBltT(imgpawn[3], 0, 0, globvar.pawndim-1, globvar.pawndim-1,
                   aux, 0, 0, aux->gc_xmax, aux->gc_ymax, TRANSPARENT&0x00FFFFFF);
    GrDestroyContext(aux);
    
    for (i=0; i<4; i++) {
        if (imgpawncruz[i]) GrDestroyContext(imgpawncruz[i]);
        imgpawncruz[i] = GrCreateContext(globvar.pawndim, globvar.pawndim, NULL, NULL);
        if (imgpawncruz[i] == NULL) {
            disaster("creating imgpawncruz");
        }
    }
    aux = load_img(FAMARCRUZ, 1);
    StbStretchBltT(imgpawncruz[0], 0, 0, globvar.pawndim-1, globvar.pawndim-1,
                   aux, 0, 0, aux->gc_xmax, aux->gc_ymax, TRANSPARENT&0x00FFFFFF);
    GrDestroyContext(aux);
    aux = load_img(FAZULCRUZ, 1);
    StbStretchBltT(imgpawncruz[1], 0, 0, globvar.pawndim-1, globvar.pawndim-1,
                   aux, 0, 0, aux->gc_xmax, aux->gc_ymax, TRANSPARENT&0x00FFFFFF);
    GrDestroyContext(aux);
    aux = load_img(FROJACRUZ, 1);
    StbStretchBltT(imgpawncruz[2], 0, 0, globvar.pawndim-1, globvar.pawndim-1,
                   aux, 0, 0, aux->gc_xmax, aux->gc_ymax, TRANSPARENT&0x00FFFFFF);
    GrDestroyContext(aux);
    aux = load_img(FVERDCRUZ, 1);
    StbStretchBltT(imgpawncruz[3], 0, 0, globvar.pawndim-1, globvar.pawndim-1,
                   aux, 0, 0, aux->gc_xmax, aux->gc_ymax, TRANSPARENT&0x00FFFFFF);
    GrDestroyContext(aux);
    
    for (i=0; i<4; i++) {
        if (imgpawndot[i]) GrDestroyContext(imgpawndot[i]);
        imgpawndot[i] = GrCreateContext(globvar.pawndim, globvar.pawndim, NULL, NULL);
        if (imgpawndot[i] == NULL) {
            disaster("creating imgpawndot");
        }
    }
    aux = load_img(FAMARDOT, 1);
    StbStretchBltT(imgpawndot[0], 0, 0, globvar.pawndim-1, globvar.pawndim-1,
                   aux, 0, 0, aux->gc_xmax, aux->gc_ymax, TRANSPARENT&0x00FFFFFF);
    GrDestroyContext(aux);
    aux = load_img(FAZULDOT, 1);
    StbStretchBltT(imgpawndot[1], 0, 0, globvar.pawndim-1, globvar.pawndim-1,
                   aux, 0, 0, aux->gc_xmax, aux->gc_ymax, TRANSPARENT&0x00FFFFFF);
    GrDestroyContext(aux);
    aux = load_img(FROJADOT, 1);
    StbStretchBltT(imgpawndot[2], 0, 0, globvar.pawndim-1, globvar.pawndim-1,
                   aux, 0, 0, aux->gc_xmax, aux->gc_ymax, TRANSPARENT&0x00FFFFFF);
    GrDestroyContext(aux);
    aux = load_img(FVERDDOT, 1);
    StbStretchBltT(imgpawndot[3], 0, 0, globvar.pawndim-1, globvar.pawndim-1,
                   aux, 0, 0, aux->gc_xmax, aux->gc_ymax, TRANSPARENT&0x00FFFFFF);
    GrDestroyContext(aux);
    
    for (i=0; i<4; i++) {
        if (imgpawnhapy[i]) GrDestroyContext(imgpawnhapy[i]);
        imgpawnhapy[i] = GrCreateContext(globvar.pawndim, globvar.pawndim, NULL, NULL);
        if (imgpawn[i] == NULL) {
            disaster("creating imgpawnhapy");
        }
    }
    aux = load_img(FAMARHAPY, 1);
    StbStretchBltT(imgpawnhapy[0], 0, 0, globvar.pawndim-1, globvar.pawndim-1,
                   aux, 0, 0, aux->gc_xmax, aux->gc_ymax, TRANSPARENT&0x00FFFFFF);
    GrDestroyContext(aux);
    aux = load_img(FAZULHAPY, 1);
    StbStretchBltT(imgpawnhapy[1], 0, 0, globvar.pawndim-1, globvar.pawndim-1,
                   aux, 0, 0, aux->gc_xmax, aux->gc_ymax, TRANSPARENT&0x00FFFFFF);
    GrDestroyContext(aux);
    aux = load_img(FROJAHAPY, 1);
    StbStretchBltT(imgpawnhapy[2], 0, 0, globvar.pawndim-1, globvar.pawndim-1,
                   aux, 0, 0, aux->gc_xmax, aux->gc_ymax, TRANSPARENT&0x00FFFFFF);
    GrDestroyContext(aux);
    aux = load_img(FVERDHAPY, 1);
    StbStretchBltT(imgpawnhapy[3], 0, 0, globvar.pawndim-1, globvar.pawndim-1,
                   aux, 0, 0, aux->gc_xmax, aux->gc_ymax, TRANSPARENT&0x00FFFFFF);
    GrDestroyContext(aux);
    
   for (i=0; i<4; i++) {
        if (imgpawnsad[i]) GrDestroyContext(imgpawnsad[i]);
        imgpawnsad[i] = GrCreateContext(globvar.pawndim, globvar.pawndim, NULL, NULL);
        if (imgpawnsad[i] == NULL) {
            disaster("creating imgpawnsad");
        }
    }
    aux = load_img(FAMARSAD, 1);
    StbStretchBltT(imgpawnsad[0], 0, 0, globvar.pawndim-1, globvar.pawndim-1,
                   aux, 0, 0, aux->gc_xmax, aux->gc_ymax, TRANSPARENT&0x00FFFFFF);
    GrDestroyContext(aux);
    aux = load_img(FAZULSAD, 1);
    StbStretchBltT(imgpawnsad[1], 0, 0, globvar.pawndim-1, globvar.pawndim-1,
                   aux, 0, 0, aux->gc_xmax, aux->gc_ymax, TRANSPARENT&0x00FFFFFF);
    GrDestroyContext(aux);
    aux = load_img(FROJASAD, 1);
    StbStretchBltT(imgpawnsad[2], 0, 0, globvar.pawndim-1, globvar.pawndim-1,
                   aux, 0, 0, aux->gc_xmax, aux->gc_ymax, TRANSPARENT&0x00FFFFFF);
    GrDestroyContext(aux);
    aux = load_img(FVERDSAD, 1);
    StbStretchBltT(imgpawnsad[3], 0, 0, globvar.pawndim-1, globvar.pawndim-1,
                   aux, 0, 0, aux->gc_xmax, aux->gc_ymax, TRANSPARENT&0x00FFFFFF);
    GrDestroyContext(aux);
    
    //GrSaveContextToPng(imgpawn[0], "stamar.png");

    for (i=0; i<2; i++) {
        if (imgpawndest[i]) GrDestroyContext(imgpawndest[i]);
        imgpawndest[i] = GrCreateContext(globvar.pawndim, globvar.pawndim, NULL, NULL);
        if (imgpawndest[i] == NULL) {
            disaster("creating imgpawndest");
        }
    }
    aux = load_img(FDEST1, 1);
    StbStretchBltT(imgpawndest[0], 0, 0, globvar.pawndim-1, globvar.pawndim-1,
                   aux, 0, 0, aux->gc_xmax, aux->gc_ymax, TRANSPARENT&0x00FFFFFF);
    GrDestroyContext(aux);
    aux = load_img(FDEST2, 1);
    StbStretchBltT(imgpawndest[1], 0, 0, globvar.pawndim-1, globvar.pawndim-1,
                   aux, 0, 0, aux->gc_xmax, aux->gc_ymax, TRANSPARENT&0x00FFFFFF);
    GrDestroyContext(aux);

    for (i=0; i<4; i++) {
        if (imgmedal[i]) GrDestroyContext(imgmedal[i]);
        imgmedal[i] = GrCreateContext(globvar.medaldim, globvar.medaldim, NULL, NULL);
        if (imgpawn[i] == NULL) {
            disaster("creating imgmedal");
        }
    }
    aux = load_img(MEDAL1, 1);
    StbStretchBltT(imgmedal[0], 0, 0, globvar.medaldim-1, globvar.medaldim-1,
                   aux, 0, 0, aux->gc_xmax, aux->gc_ymax, TRANSPARENT&0x00FFFFFF);
    GrDestroyContext(aux);
    aux = load_img(MEDAL2, 1);
    StbStretchBltT(imgmedal[1], 0, 0, globvar.medaldim-1, globvar.medaldim-1,
                   aux, 0, 0, aux->gc_xmax, aux->gc_ymax, TRANSPARENT&0x00FFFFFF);
    GrDestroyContext(aux);
    aux = load_img(MEDAL3, 1);
    StbStretchBltT(imgmedal[2], 0, 0, globvar.medaldim-1, globvar.medaldim-1,
                   aux, 0, 0, aux->gc_xmax, aux->gc_ymax, TRANSPARENT&0x00FFFFFF);
    GrDestroyContext(aux);
    aux = load_img(MEDAL4, 1);
    StbStretchBltT(imgmedal[3], 0, 0, globvar.medaldim-1, globvar.medaldim-1,
                   aux, 0, 0, aux->gc_xmax, aux->gc_ymax, TRANSPARENT&0x00FFFFFF);
    GrDestroyContext(aux);

    for (i=0; i<10; i++) {
        if (imgdice[i]) GrDestroyContext(imgdice[i]);
    }
    if (globvar.tbldim > SMALLTBL) {
        imgdice[0] = load_img(DADOX, 1);
        imgdice[1] = load_img(DADO1, 1);
        imgdice[2] = load_img(DADO2, 1);
        imgdice[3] = load_img(DADO3, 1);
        imgdice[4] = load_img(DADO4, 1);
        imgdice[5] = load_img(DADO5, 1);
        imgdice[6] = load_img(DADO6, 1);
        imgdice[7] = load_img(DADO7, 1);
        imgdice[8] = load_img(DADO10, 1);
        imgdice[9] = load_img(DADO20, 1);
    } else {
        imgdice[0] = load_img(SDADOX, 1);
        imgdice[1] = load_img(SDADO1, 1);
        imgdice[2] = load_img(SDADO2, 1);
        imgdice[3] = load_img(SDADO3, 1);
        imgdice[4] = load_img(SDADO4, 1);
        imgdice[5] = load_img(SDADO5, 1);
        imgdice[6] = load_img(SDADO6, 1);
        imgdice[7] = load_img(SDADO7, 1);
        imgdice[8] = load_img(SDADO10, 1);
        imgdice[9] = load_img(SDADO20, 1);
    }

    if (imgperson) GrDestroyContext(imgperson);
    imgperson = load_img(DIBPERSON, 1);
    if (imgrobot) GrDestroyContext(imgrobot);
    imgrobot = load_img(DIBROBOT, 1);
    if (imgpersonbl) GrDestroyContext(imgpersonbl);
    imgpersonbl = load_img(DIBPERSONBL, 1);
    if (imgrobotbl) GrDestroyContext(imgrobotbl);
    imgrobotbl = load_img(DIBROBOTBL, 1);

    if (imgpodium) GrDestroyContext(imgpodium);
    imgpodium = GrCreateContext(globvar.podiumdim, globvar.podiumdim, NULL, NULL);
    if (imgpodium == NULL) {
        disaster("creating imgpodium");
    }
    aux = load_img(PODIUM, 1);
    StbStretchBltT(imgpodium, 0, 0, globvar.podiumdim-1, globvar.podiumdim-1,
                   aux, 0, 0, aux->gc_xmax, aux->gc_ymax, TRANSPARENT&0x00FFFFFF);
    GrDestroyContext(aux);
    if (imgpodiump) GrDestroyContext(imgpodiump);
    imgpodiump = GrCreateContext(globvar.podiumpdim, globvar.podiumpdim, NULL, NULL);
    if (imgpodiump == NULL) {
        disaster("creating imgpodiump");
    }
    aux = load_img(PODIUMP, 1);
    StbStretchBltT(imgpodiump, 0, 0, globvar.podiumpdim-1, globvar.podiumpdim-1,
                   aux, 0, 0, aux->gc_xmax, aux->gc_ymax, TRANSPARENT&0x00FFFFFF);
    GrDestroyContext(aux);

    if (imgmoviola) GrDestroyContext(imgmoviola);
    imgmoviola = GrCreateContext(globvar.movioladim, globvar.movioladim, NULL, NULL);
    if (imgmoviola == NULL) {
        disaster("creating imgmoviola");
    }
    aux = load_img(MOVIOLA, 1);
    StbStretchBltT(imgmoviola, 0, 0, globvar.movioladim-1, globvar.movioladim-1,
                   aux, 0, 0, aux->gc_xmax, aux->gc_ymax, TRANSPARENT&0x00FFFFFF);
    GrDestroyContext(aux);

    if (imgbarrerah) GrDestroyContext(imgbarrerah);
    imgbarrerah = GrCreateContext(globvar.barreradim, globvar.pawndim, NULL, NULL);
    if (imgbarrerah == NULL) {
        disaster("creating imgbarrerah");
    }
    aux = load_img(BARRERAH, 1);
    StbStretchBltT(imgbarrerah, 0, 0, globvar.barreradim, globvar.pawndim-1,
                   aux, 0, 0, aux->gc_xmax, aux->gc_ymax, TRANSPARENT&0x00FFFFFF);
    GrDestroyContext(aux);

    if (imgbarrerav) GrDestroyContext(imgbarrerav);
    imgbarrerav = GrCreateContext(globvar.pawndim, globvar.barreradim, NULL, NULL);
    if (imgbarrerav == NULL) {
        disaster("creating imgbarrerav");
    }
    aux = load_img(BARRERAV, 1);
    StbStretchBltT(imgbarrerav, 0, 0, globvar.pawndim-1, globvar.barreradim-1,
                   aux, 0, 0, aux->gc_xmax, aux->gc_ymax, TRANSPARENT&0x00FFFFFF);
    GrDestroyContext(aux);
}

/***********************/

void genera_munecos(DefPartida *dp)
{
    int c, cp;
    GrContext *aux, *orig;
    GrContext *auxbl, *origbl;

    for (c=AMAR; c<=VERD; c++) {
        if (imgmuneco[c]) {
            GrDestroyContext(imgmuneco[c]);
            imgmuneco[c] = NULL;
        }
        if (imgmunecobl[c]) {
            GrDestroyContext(imgmunecobl[c]);
            imgmunecobl[c] = NULL;
        }

        if (dp->playertype[c] == PERSON) {
            orig = imgperson;
            origbl = imgpersonbl;
        } else if (dp->playertype[c] == ROBOT) {
            orig = imgrobot;
            origbl = imgrobotbl;
        } else {
            continue;
        }

        aux = GrCreateContext(orig->gc_xmax+1, orig->gc_ymax+1, NULL, NULL);
        auxbl = GrCreateContext(origbl->gc_xmax+1, origbl->gc_ymax+1, NULL, NULL);
        if (aux == NULL || auxbl == NULL)
            disaster("in genera_munecos");

        GrBitBlt(aux, 0, 0, orig, 0, 0, orig->gc_xmax, orig->gc_ymax, GrWRITE);
        GrBitBlt(auxbl, 0, 0, origbl, 0, 0, origbl->gc_xmax, origbl->gc_ymax, GrWRITE);

        imgmuneco[c] = GrCreateContext(globvar.munecodim, globvar.munecodim, NULL, NULL);
        imgmunecobl[c] = GrCreateContext(globvar.munecodim, globvar.munecodim, NULL, NULL);
        if (imgmuneco[c] == NULL || imgmunecobl[c] == NULL)
            disaster("in genera_munecos");

        cp = c;
        if (dp->gametype == GT_PAIRS) {
            cp = (c > 1) ? c - 2 : c + 2;
        }

        GrFloodSpillC(aux, 0, 0, aux->gc_xmax, aux->gc_ymax,
                      GrAllocColor(0x88, 0x88, 0x88), playercolor[c]);
        GrFloodSpillC(aux, 0, 0, aux->gc_xmax, aux->gc_ymax,
                      GrAllocColor(0x77, 0x77, 0x77), playercolor[cp]);
        GrFloodSpillC(auxbl, 0, 0, auxbl->gc_xmax, auxbl->gc_ymax,
                      GrAllocColor(0x88, 0x88, 0x88), playercolor[c]);
        GrFloodSpillC(auxbl, 0, 0, auxbl->gc_xmax, auxbl->gc_ymax,
                      GrAllocColor(0x77, 0x77, 0x77), playercolor[cp]);

        if (dp->playertype[c] == PERSON) {
            GrFloodSpillC(aux, 0, 0, aux->gc_xmax, aux->gc_ymax,
                          GrAllocColor(0x80, 0x80, 0x80), playerlcolor[c]);
            GrFloodSpillC(auxbl, 0, 0, auxbl->gc_xmax, auxbl->gc_ymax,
                          GrAllocColor(0x80, 0x80, 0x80), playerlcolor[c]);
        } else {
            GrFloodSpillC(aux, 0, 0, aux->gc_xmax, aux->gc_ymax,
                          GrAllocColor(0x66, 0x66, 0x66),
                          cintcolor[dp->robotlevel[c]]);
            GrFloodSpillC(auxbl, 0, 0, auxbl->gc_xmax, auxbl->gc_ymax,
                          GrAllocColor(0x66, 0x66, 0x66),
                          cintcolor[dp->robotlevel[c]]);
        }
            
        StbStretchBltT(imgmuneco[c], 0, 0, globvar.munecodim-1,
                       globvar.munecodim-1, aux, 0, 0, aux->gc_xmax,
                       aux->gc_ymax, TRANSPARENT&0x00FFFFFF);
        StbStretchBltT(imgmunecobl[c], 0, 0, globvar.munecodim-1,
                       globvar.munecodim-1, auxbl, 0, 0, auxbl->gc_xmax,
                       auxbl->gc_ymax, TRANSPARENT&0x00FFFFFF);
        GrDestroyContext(aux);
        GrDestroyContext(auxbl);
    }
}


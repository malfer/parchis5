/*
 * epartida.c --- Parchis engine, Partida functions
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
#include "engine.h"

static char SAVE_SIGNATURE[4] = {'P','v','5','0'};

/***********************/

static void advanceTurno(Partida *pt)
{
    if (!PPAdvanceTurno(&(pt->pp))) {
        pt->status = PST_ENDGAME;
        return;
    }
    if (pt->nextvdice == -1) { // a color has finished his turn
        if (PPCheckEndPairs(&(pt->pp))) {
            pt->status = PST_ENDGAME;
            return;
        }
        if (PPCheckFastEnd(&(pt->pp))) {
            pt->status = PST_ENDGAME;
            return;
        }
    }
}

/***********************/

static int defaultJg(Partida *pt)
{
    int color, i;
    int jeleg = 0;
    int nj = pt->gjg.njg;

    if (nj < 2) return jeleg;

    color = pt->gjg.jg[0].cjugador;
    if (pt->lastfselec[color] < 0) return jeleg;

    for (i=0; i<nj; i++) {
        if (pt->gjg.jg[i].nficha == pt->lastfselec[color]) {
            jeleg = i;
            break;
        }
    }
    return jeleg;
}

/***********************/

void PTInit(Partida *pt, DefPartida *dp)
{
    int c;

    PPSetInitialPosition(&(pt->pp), dp);
    pt->status = PST_WAITDICE;
    pt->nextvdice = 0;
    pt->njgselec = -1;
    for (c=AMAR; c<=VERD; c++)
        pt->lastfselec[c] = -1;
    if (PPHasFinished(&(pt->pp))) pt->status = PST_ENDGAME; // no hay jugadores
}

/***********************/

void PTSetDice(Partida *pt, int vdice)
{
    pt->vdice = vdice;
    pt->pp.nsix = (vdice == 6) ? pt->pp.nsix+1 : 0;
    // en algun caso al interrumpir moviola el 10 y el 20 entran por aquí
    if (vdice > 0 && vdice <= 6) {
        pt->pp.stats_dice[pt->pp.turno][vdice-1]++;
        pt->pp.stats_totdice[pt->pp.turno]++;
    }
    if (vdice == 6 && pt->pp.nfcasa[pt->pp.turno] == 0) pt->vdice = 7; // hace 6 a 7
}

/***********************/

void PTSetJg(Partida *pt, int njg)
{
    pt->njgselec = njg;
}

/***********************/

void PTSelectRobotJg(Partida *pt)
{
    int i, v, valor = -32000;

    if (pt->gjg.njg < 2) return;
        
    GJGFillInfoAnl(&(pt->gjg), &(pt->pp));

    for (i=0; i<pt->gjg.njg; i++) {
        v = JGCalcWeight( &(pt->gjg.jg[i]),
                          pt->pp.dp.robotlevel[pt->pp.turno],
                          pt->pp.dp.gametype);
        if (v > valor) {
            valor = v;
            pt->njgselec = i;
        }
    }
}

/***********************/

void PTNextStep(Partida *pt)
{
    switch (pt->status) {
        case PST_WAITDICE:
            if (GJGGen(&(pt->gjg), &(pt->pp), pt->pp.turno, pt->vdice) > 0) {
                pt->njgselec = defaultJg(pt);
                pt->status = PST_WAITJG;
            } else {
                pt->njgselec = -1;
                pt->status = PST_WAITPASSACK;
            }
            break;
        case PST_WAITPASSACK:
            pt->pp.lastmoved = -1; // para que no se vaya a casa si pasa en el segundo 6
            pt->pp.stats_pass[pt->pp.turno]++;
            pt->status = PST_WAITDICE;
            if (pt->nextvdice == -1 || pt->pp.nsix == 0 || pt->pp.nsix > 2)
                advanceTurno(pt);
            pt->nextvdice = 0;
            break;
        case PST_WAITJG:
            pt->nextvdice = PPDoJg(&(pt->pp), &(pt->gjg.jg[pt->njgselec]));
            PPCalcDistances(&(pt->pp));
            if (pt->gjg.njg > 1) {
                pt->lastfselec[pt->gjg.jg[pt->njgselec].cjugador] =
                    pt->gjg.jg[pt->njgselec].nficha;
            }
            pt->njgselec = -1;
            if (pt->nextvdice == 10 || pt->nextvdice == 20) {
                pt->vdice = pt->nextvdice;
                if (GJGGen(&(pt->gjg), &(pt->pp), pt->pp.turno, pt->vdice) > 0) {
                    pt->njgselec = defaultJg(pt);
                    pt->status = PST_WAITJG;
                } else {
                    pt->njgselec = -1;
                    pt->status = PST_WAITPASSACK;
                }
            } else {
                pt->status = PST_WAITDICE;
                if (pt->nextvdice == -1 || pt->pp.nsix == 0 || pt->pp.nsix > 2)
                    advanceTurno(pt);
            }
            break;
    }
}

/***********************/

int PTSaveToFile(Partida *pt, char *fname)
{
    FILE *f;

    f = fopen(fname, "wb");
    if (f == NULL) return 0;

    fwrite((void *)SAVE_SIGNATURE, 4, 1, f);
    fwrite((void *)pt, sizeof(Partida), 1, f);
    fclose(f);

    return 1;
}

/***********************/

int PTLoadFromFile(Partida *pt, char *fname)
{
    FILE *f;
    char buf[4];
    Partida pt2;
    int len;

    f = fopen(fname, "rb");
    if (f == NULL) return 0;

    fread(buf, 4, 1, f);
    if (memcmp(buf, SAVE_SIGNATURE, 4) != 0) {
        fclose(f);
        return 0;
    }
    len = fread(&pt2, sizeof(Partida), 1, f);
    if (len != 1) {
        fclose(f);
        return 0;
    }
    *pt = pt2;
    fclose(f);

    return 1;
}

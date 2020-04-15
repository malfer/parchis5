/*
 * etablero.c --- Parchis engine, Tablero functions
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

void TBLInit(Tablero *tbl)
{
    int c, i, j;

    for (i=0; i<68; i++) {
        tbl->corr[i].type = NORMAL;
        tbl->corr[i].cf[0] = VACIO;
        tbl->corr[i].cf[1] = VACIO;
    }
    tbl->corr[4].type   = ENTRADA_AMAR;
    tbl->corr[11].type  = SEGURO;
    tbl->corr[16].type  = SALIDA;
    tbl->corr[21].type  = ENTRADA_AZUL;
    tbl->corr[28].type  = SEGURO;
    tbl->corr[33].type  = SALIDA;
    tbl->corr[38].type  = ENTRADA_ROJO;
    tbl->corr[45].type  = SEGURO;
    tbl->corr[50].type  = SALIDA;
    tbl->corr[55].type  = ENTRADA_VERD;
    tbl->corr[62].type  = SEGURO;
    tbl->corr[67].type  = SALIDA;

    for (c=AMAR; c<=VERD; c++) {
        for (j=0; j<4; j++)
            tbl->casa[c].cf[j] = VACIO;
        for (j=0; j<4; j++)
            tbl->meta[c].cf[j] = VACIO;
        for (j=0; j<7; j++) {
            tbl->lleg[c][j].cf[0] = VACIO;
            tbl->lleg[c][j].cf[1] = VACIO;
        }
    }
}

void TBLDelPawns(Tablero *tbl)
{
    int c, i, j;

    for (i=0; i<68; i++) {
        tbl->corr[i].cf[0] = VACIO;
        tbl->corr[i].cf[1] = VACIO;
    }

    for (c=AMAR; c<=VERD; c++) {
        for (j=0; j<4; j++)
            tbl->casa[c].cf[j] = VACIO;
        for (j=0; j<4; j++)
            tbl->meta[c].cf[j] = VACIO;
        for (j=0; j<7; j++) {
            tbl->lleg[c][j].cf[0] = VACIO;
            tbl->lleg[c][j].cf[1] = VACIO;
        }
    }
}

void TBLSetPawns(Tablero *tbl, PosPartida *pp)
{
    int c, nf, pos, ind;

    //TBLDelPawns(tbl); we expect to be clean and inited

    for (c=AMAR; c<=VERD; c++) {
        if (pp->dp.playertype[c] == NOPLAY) continue;
        for (nf=0; nf<4; nf++) {
            pos = pp->pf[c][nf].pos;
            ind = pp->pf[c][nf].ind;
            if (pp->pf[c][nf].place == CASA) {
                tbl->casa[c].cf[ind] = c;
                tbl->casa[c].nf[ind] = nf;
            } else if (pp->pf[c][nf].place == CORR) {
                tbl->corr[pos].cf[ind] = c;
                tbl->corr[pos].nf[ind] = nf;
            } else if (pp->pf[c][nf].place == LLEG) {
                tbl->lleg[c][pos].cf[ind] = c;
                tbl->lleg[c][pos].nf[ind] = nf;
            } else if (pp->pf[c][nf].place == META) {
                tbl->meta[c].cf[ind] = c;
                tbl->meta[c].nf[ind] = nf;
            }
        }
    }
}

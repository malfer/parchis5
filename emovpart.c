/*
 * emovpart.c --- Parchis engine, RegMov functions
 * 
 * Copyright (c) 2023 Mariano Alvarez Fernandez
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
#include "engine.h"

#define SIZE_INCR 200

int MPExtend(MovPartida *mp)
{
    RegMov *new;

    new = realloc(mp->r, (mp->maxnjg+SIZE_INCR)*sizeof(RegMov));
    if (new == NULL) return 0;

    mp->maxnjg += SIZE_INCR;
    mp->r = new;
    return 1;
}

void MPClear(MovPartida *mp)
{
    mp->njg = mp->posmoviola = 0;
}

void MPClean(MovPartida *mp)
{
    mp->njg = mp->posmoviola = 0;
    free(mp->r);
    mp->maxnjg = 0;
}

int MPAddReg(MovPartida *mp, int color, int vdice, int nf, int hpc)
{
    int res;

    if (mp->njg >= mp->maxnjg) {
        res = MPExtend(mp);
        if (!res) return 0;
    }

    mp->r[mp->njg].color = color;
    mp->r[mp->njg].vdice = vdice;
    mp->r[mp->njg].nf = nf;
    mp->r[mp->njg].hpc = hpc;
    mp->njg++;
    return 1;
}

int MPAddPass(MovPartida *mp, int color, int vdice)
{
    return MPAddReg(mp, color, vdice, -1, 0);
}

int MPAddJg(MovPartida *mp, Jugada *jg)
{
    return MPAddReg(mp, jg->cjugador, jg->vdado, jg->nficha, jg->hptep);
}

RegMov *MPGetJg(MovPartida *mp, int n)
{
    if (n<0 || n>=mp->njg) return NULL;
    return &(mp->r[n]);
}

void MPResetMovl(MovPartida *mp)
{
    mp->posmoviola = 0;
}

int MPForwardMovl(MovPartida *mp)
{
    if (mp->posmoviola >= mp->njg) return 0;
    mp->posmoviola++;
    return 1;
}

RegMov *MPGetJgMovl(MovPartida *mp)
{
    if (mp->posmoviola >= mp->njg) return NULL;
    return &(mp->r[mp->posmoviola]);
}

void MPPrint(MovPartida *mp, FILE *f)
{
    int i;

    for (i=0; i<mp->njg; i++) {
        fprintf(f,"%4d: %2d %2d %2d %2d\n", i,
                mp->r[i].color, mp->r[i].vdice, mp->r[i].nf, mp->r[i].hpc);
    }
}

void MPSaveToFile(MovPartida *mp, FILE *f)
{
    fwrite((void *)&(mp->njg), sizeof(int), 1, f);
    fwrite((void *)&(mp->posmoviola), sizeof(int), 1, f);
    if (mp->njg < 1) return;
    fwrite((void *)(mp->r), sizeof(RegMov)*mp->njg, 1, f);
}

int MPLoadFromFile(MovPartida *mp, FILE *f)
{
    int len, njg, posmoviola;

    len = fread(&njg, sizeof(int), 1, f);
    if (len != 1) return 0;
    len = fread(&posmoviola, sizeof(int), 1, f);
    if (len != 1) return 0;
    MPClear(mp);
    while (njg > mp->maxnjg) MPExtend(mp);
    len = fread(mp->r, sizeof(RegMov), njg, f);
    if (len != njg) return 0;
    mp->njg = njg;
    mp->posmoviola = posmoviola;
    return 1;
}

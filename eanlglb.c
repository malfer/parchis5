/*
 * eanlglb.c --- Parchis engine, AnlGlobal functions
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

static void clear(AnlGlobal *ag)
{
    int c, j;

    for (c=AMAR; c<=VERD; c++) {
        for (j=0; j<4; j++){
            ag->nficom[c][j] = 0;
            ag->nfipel[c][j] = 0;
            ag->nfi6com[c][j] = 0;
            ag->nfi6pel[c][j] = 0;
        }
    }
}

void AGlobalN2(AnlGlobal *ag, PosPartida *pp)
{
    GrupoJugadas gjg;
    int c, vdice, nj, j;

    clear(ag);

    for (c=AMAR; c<=VERD; c++) {
        if (pp->dp.playertype[c] == NOPLAY) continue;
        for (vdice=1; vdice<7; vdice++){
            if (vdice == 6 && pp->nfcasa[c] == 0) vdice = 7;
            nj = GJGGen(&gjg, pp, c, vdice);
            for (j=0; j<nj; j++) {
                if (gjg.jg[j].comep) continue;
                if (gjg.jg[j].come && gjg.jg[j].cjugdcom != VACIO) {
                    ag->nficom[c][gjg.jg[j].nficha]++;
                    ag->nfipel[gjg.jg[j].cjugdcom][gjg.jg[j].nfichcom]++;
                }
            }
        }
    }
}

void AGlobalN4(AnlGlobal *ag, PosPartida *pp)
{
    GrupoJugadas gjg, gjg2;
    int c, vdice, nj, j, nj2, j2, yacontmeta, d6, k;

    clear(ag);

    for (c=AMAR; c<=VERD; c++) {
        if (pp->dp.playertype[c] == NOPLAY) continue;
        yacontmeta = 0;
        for (vdice=1; vdice<7; vdice++){
            if (vdice == 6 && pp->nfcasa[c] == 0) vdice = 7;
            nj = GJGGen(&gjg, pp, c, vdice);
            for (j=0; j<nj; j++) {
                if (gjg.jg[j].comep) continue;
                if (gjg.jg[j].come && gjg.jg[j].cjugdcom != VACIO) {
                    ag->nficom[c][gjg.jg[j].nficha]++;
                    ag->nfipel[gjg.jg[j].cjugdcom][gjg.jg[j].nfichcom]++;
                    if (gjg.jg[j].come2 && gjg.jg[j].cjugdcom2 != VACIO) {
                        ag->nficom[c][gjg.jg[j].nficha2]++;
                        ag->nfipel[gjg.jg[j].cjugdcom2][gjg.jg[j].nfichcom2]++;
                    }
                } else if (!yacontmeta && gjg.jg[j].meta) {
                    nj2 = GJGGen(&gjg2, pp, c, 10);
                    for (j2=0; j2<nj2; j2++) {
                        if (gjg2.jg[j2].comep) continue;
                        if (gjg2.jg[j2].come && gjg2.jg[j2].cjugdcom != VACIO) {
                            ag->nficom[c][gjg2.jg[j2].nficha]++;
                            ag->nfipel[gjg2.jg[j2].cjugdcom][gjg2.jg[j2].nfichcom]++;
                        }
                    }
                    yacontmeta = 1;
                }
            }
        }
        /* ahora ver comibles y peligrosas si 6+n */
        d6 = (pp->nfcasa[c] == 0) ? 7 : 6;
        for (k=1; k<7; k++){
            vdice = (k == 6) ? d6 + d6 : d6 + k;
            nj = GJGGen(&gjg, pp, c, vdice);
            for (j=0; j<nj; j++) {
                if (gjg.jg[j].comep) continue;
                if (gjg.jg[j].come && gjg.jg[j].cjugdcom != VACIO) {
                    ag->nfi6com[c][gjg.jg[j].nficha]++;
                    ag->nfi6pel[gjg.jg[j].cjugdcom][gjg.jg[j].nfichcom]++;
                }
            }
        }
    }
}

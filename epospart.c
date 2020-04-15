/*
 * epospart.c --- Parchis engine, PosPartida functions
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

void PPSetInitialPosition(PosPartida *pp, DefPartida *dp)
{
    int c, nf, vd;

    pp->dp = *dp;

    for (c=AMAR; c<=VERD; c++) {
        for (nf=0; nf<4; nf++) {
            PFSetPosition(&(pp->pf[c][nf]), CASA, 0, nf);
        }
        pp->hasbegun[c] = 0;
        pp->endingpos[c] = -1;
        pp->nfcasa[c] = 4;
        pp->nfmeta[c] = 0;
        for (vd=0; vd<6; vd++) {
            pp->stats_dice[c][vd] = 0;
        }
        pp->stats_totdice[c] = 0;
        pp->stats_pass[c] = 0;
        pp->stats_eat[c] = 0;
        pp->stats_eaten[c] = 0;
        pp->stats_gocasa[c] = 0;
        pp->distance[c] = 320;
    }
    
    pp->nextending = 0;
    pp->nsix = 0;
    pp->lastmoved = -1;
    pp->turno = pp->dp.whobegin;
    if (pp->dp.playertype[pp->turno] == NOPLAY)
        PPAdvanceTurno(pp);
}

int PPHasBegun(PosPartida *pp)
{
    int c;

    for (c=AMAR; c<=VERD; c++) {
        if (pp->hasbegun[c]) return 1;
    }
    
    return 0;
}

int PPHasFinished(PosPartida *pp)
{
    int c;

    for (c=AMAR; c<=VERD; c++) {
        if (pp->dp.playertype[c] != NOPLAY && pp->endingpos[c] < 0) return 0;
    }
    
    return 1;
}

static int calcDistance(PosPartida *pp, int c)
{
    int dis = 0;
    int nf, pos;
    
    for (nf=0; nf<4; nf++) {
        if (pp->pf[c][nf].place == META)
            continue;
        else if (pp->pf[c][nf].place == LLEG)
            dis += 5 + 7 - pp->pf[c][nf].pos;
        else if (pp->pf[c][nf].place == CORR) {
            pos = pp->pf[c][nf].pos;
            if (pos < corrent[c]) pos += 68;
            pos = pos - (17 * c);
            dis += 5 + 7 + 68 - pos;
        } else // CASA
            dis += 5 + 7 + 68;
    }

    return dis;
}

void PPCalcDistances(PosPartida *pp)
{
    int c;

    for (c=AMAR; c<=VERD; c++) {
        pp->distance[c] = calcDistance(pp, c);
    }
}

static void addTurno(PosPartida *pp)
{
    if (pp->turno == VERD)
        pp->turno = AMAR;
    else
        pp->turno++;
}

int PPAdvanceTurno(PosPartida *pp)
{
    int i;
    int found = 0;

    pp->nsix = 0;
    pp->lastmoved = -1;
    addTurno(pp);

    for (i=0; i<4; i++) {
        if (pp->dp.playertype[pp->turno] != NOPLAY &&
            pp->endingpos[pp->turno] < 0) {
            found = 1;
            break;
        } else
            addTurno(pp);
    }

    return found;
}

int PPCheckEndPairs(PosPartida *pp)
{
    if (pp->dp.gametype != GT_PAIRS) return 0;
    if (pp->nextending < 1) return 0;
    if (pp->endingpos[AMAR] == 0 || pp->endingpos[ROJO] == 0) {
        pp->endingpos[AMAR] = 0;
        pp->endingpos[ROJO] = 1;
        pp->endingpos[AZUL] = 2;
        pp->endingpos[VERD] = 3;
    } else {
        pp->endingpos[AZUL] = 0;
        pp->endingpos[VERD] = 1;
        pp->endingpos[AMAR] = 2;
        pp->endingpos[ROJO] = 3;
    }

    return 1;
}

static void finishTheGame(PosPartida *pp)
{
    int lplynf[4];
    int nplynf = 0;
    int c, i, k;
    int dis[4];
    int min, posmin;

    for (c=AMAR; c<=VERD; c++) {
        if (pp->endingpos[c] >= 0) {
            continue;
        } else if (pp->dp.playertype[c] != NOPLAY) {
            lplynf[nplynf] = c;
            nplynf++;
        }
    }

    if (nplynf == 1) { // only one to finish
        pp->endingpos[lplynf[0]] = pp->nextending++;
    } else if (nplynf > 1) { // 2, 3, or 4 to finish
        for (k=0; k<nplynf; k++) {
            dis[k] = pp->distance[lplynf[k]];
        }
        for (i=0; i<nplynf; i++) {
            min = 1000;
            posmin = 0;
            for (k=0; k<nplynf; k++) {
                if (dis[k] < min) {
                    min = dis[k];
                    posmin = k;
                }
            }
            dis[posmin] = 2000;
            pp->endingpos[lplynf[posmin]] = pp->nextending++;
        }
    }
}

int PPCheckFastEnd(PosPartida *pp)
{
    int nplyhf, c;

    if (pp->dp.superfastend && pp->nextending > 0) {
        finishTheGame(pp);
        return 1;
    }

    if (pp->dp.fastend && pp->dp.nplayers > 1) {
        if (pp->dp.nplayers == pp->nextending+1) {
            pp->endingpos[pp->turno] = pp->nextending++;
            return 1;
        }
    }

    //printf("cfe %d %d\n", pp->dp.fastrobotend, pp->dp.nplayers);
    if (pp->dp.fastrobotend && pp->dp.nplayers > 1) {
        nplyhf = 0;
        for (c=AMAR; c<=VERD; c++) {
            if (pp->endingpos[c] >= 0 && pp->dp.playertype[c] == PERSON)
                nplyhf++;
        }
        //printf("fre %d %d\n", nplyhf, pp->dp.nhumplayers);
        if (pp->dp.nhumplayers > 0 && nplyhf == pp->dp.nhumplayers) {
            finishTheGame(pp);
            return 1;
        }
    }

    return 0;
}

int PPDoJg(PosPartida *pp, Jugada *jg)
{
    int ficha_comida = 0;
    int en_meta = 0;
    int jfin = 0;

    pp->lastmoved = jg->nficha;

    /* si hay ficha desplazada la recoloca */
    if (jg->cjugddpl != VACIO) {
        pp->pf[jg->cjugddpl][jg->nfichdpl].ind--;
    }
        
    /* coloca la nueva posicion en los datos de la ficha */
    pp->pf[jg->cjugador][jg->nficha] = jg->destino;
    	
    /* caso especial a casa por tres seises */
    if (jg->acasa) {
        pp->stats_gocasa[jg->cjugador]++;
        pp->nfcasa[jg->cjugador]++;
        return 0;
    }

    /* manda fichas comidas a casa */
    if (jg->come) {
        pp->stats_eat[jg->cjugador]++;
        pp->stats_eaten[jg->cjugdcom]++;
        ficha_comida = 1;
        pp->pf[jg->cjugdcom][jg->nfichcom] = jg->destfcom;
        pp->nfcasa[jg->cjugdcom]++;
    }
    if (jg->come2) {
        pp->stats_eat[jg->cjugador]++;
        pp->stats_eaten[jg->cjugdcom2]++;
        pp->pf[jg->cjugdcom2][jg->nfichcom2] = jg->destfcom2;
        pp->nfcasa[jg->cjugdcom2]++;
    }

    /* comprueba si ha terminado el jugador */
    if (jg->meta) {
        pp->nfmeta[jg->cjugador]++;
        if (pp->nfmeta[jg->cjugador] < 4) {
            en_meta = 1;
        } else {
            jfin = 1;
            pp->endingpos[jg->cjugador] = pp->nextending++;
        }
    }

    if (jg->entra)
        pp->nfcasa[jg->cjugador]--;

    /* si es el primer 5 hay que sacar dos fichas */
    if (jg->entra2) {
        pp->nfcasa[jg->cjugador]--;
        pp->pf[jg->cjugador][jg->nficha2] = jg->destino2;
        pp->hasbegun[jg->cjugador] = 1;
    }

    /* devuelve valor de dado de la jugada */
    if (ficha_comida) return 20;
    if (en_meta) return 10;
    if (jfin) return -1; /* indica que ya han llegado todas sus fichas */
    return 0;
}

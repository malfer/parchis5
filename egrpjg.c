/*
 * egrpjg.c --- Parchis engine, GrupoJugadas functions
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

int GJGGen(GrupoJugadas *gjg, PosPartida *pp, int cjg, int vdice)
{
    Tablero tbl;
    int i, saleCasa, posind, rpte, njc;

    TBLInit(&tbl);
    TBLSetPawns(&tbl, pp);
    
    gjg->njg = 0;
    /* mira si son tres seises y la nfmovida está en el corredor */
    /* entonces se va a casa, si no pasa */
    if (pp->nsix == 3) {
        if (pp->lastmoved >= 0 && pp->pf[cjg][pp->lastmoved].place == CORR) {
            JGClean(&(gjg->jg[0]));
            if (JGGenGoHome(&(gjg->jg[0]), pp, &tbl, cjg, pp->lastmoved, vdice))
                gjg->njg = 1;
        }
        return gjg->njg;
    }

    /* mira si es 5 y no ha salido aun, saca dos fichas y es jugada unica */
    if (vdice == 5 && !pp->hasbegun[cjg]) {
        JGClean(&(gjg->jg[0]));
        if (JGGenFirst5(&(gjg->jg[0]), pp, &tbl, cjg))
            gjg->njg = 1;
        return gjg->njg;
    }

    for (i=0; i<4; i++) {
        JGClean(&(gjg->jg[gjg->njg]));
        if (JGGen(&(gjg->jg[gjg->njg]), pp, &tbl, cjg, i, vdice, 1)) {
            gjg->njg++;
            /* en juego por parejas si come pareja (sin entrar) tambien puede
               hacer puente */
            if (pp->dp.gametype == GT_PAIRS) {
                if (gjg->jg[gjg->njg-1].comep && !gjg->jg[gjg->njg-1].entra) {
                    JGClean(&(gjg->jg[gjg->njg]));
                    if (JGGen(&(gjg->jg[gjg->njg]), pp, &tbl, cjg, i, vdice, 0))
                        gjg->njg++;
                }
            }
        }
    }

    /* mira si es 5 y sale de casa, en cuyo caso solo hay */
    /* una jugada permitida */
    if (vdice == 5) {
        saleCasa = -1;
        posind = -1; // para coger la última colocada
        for (i=0; i<gjg->njg; i++) {
            if (gjg->jg[i].entra && gjg->jg[i].origen.ind > posind) {
                saleCasa = i;
                posind = gjg->jg[i].origen.ind;
            }
        }
        if (saleCasa >= 0) {
            if (saleCasa > 0) gjg->jg[0] = gjg->jg[saleCasa];
            gjg->njg = 1;
        }
    }

    /* mira si es 6 o 7 y rompe puente, entonces es obligado */
    if (vdice == 6 || vdice == 7) {
        rpte = 0;
        for (i=0; !rpte && i<gjg->njg; i++)
            if ( gjg->jg[i].rpte) rpte = 1;
        if (rpte) {
            njc = 0;
            for (i=0; i<gjg->njg; i++) {
                if (gjg->jg[i].rpte) gjg->jg[njc++] = gjg->jg[i];
            }
            gjg->njg = njc;
        }
    }

    return gjg->njg;
}

static void fillInfoAnlN2(GrupoJugadas *gjg, PosPartida *pp) {
    PosPartida pp2;
    AnlGlobal anlini, anlfin;
    int i, cj, nf, n;

    AGlobalN2(&anlini, pp);
    for (i=0; i<gjg->njg; i++) {
        cj = gjg->jg[i].cjugador;
        nf = gjg->jg[i].nficha;
        gjg->jg[i].nficom = anlini.nficom[cj][nf];
        gjg->jg[i].nfipel = anlini.nfipel[cj][nf];
        pp2 = *pp;
        PPDoJg(&pp2, &(gjg->jg[i]));
        AGlobalN2(&anlfin, &pp2);
        gjg->jg[i].nffcom = anlfin.nficom[cj][nf];
        gjg->jg[i].nffpel = anlfin.nfipel[cj][nf];
        if (gjg->jg[i].destino.place == CORR) {
            n = gjg->jg[i].destino.pos;
            if (n < corrent[cj]) n += 67;
            gjg->jg[i].ncfin = n - (17 * cj);
        }
    }
}

static void fillInfoAnlN4(GrupoJugadas *gjg, PosPartida *pp) {
    PosPartida pp2;
    AnlGlobal anlini, anlfin;
    GrupoJugadas gjg2;
    int c, i, cj, nf, n;
    int k, dif, cpar;
    int njgliza = 0;

    AGlobalN4(&anlini, pp);

    //njgliza = pp->dp.nplayers - pp->nextending; /* mejora: quitar los que estan en llegada */
    for (c=AMAR; c<=VERD; c++) {
        if (pp->dp.playertype[c] == NOPLAY) continue;
        if (pp->endingpos[c] >= 0) continue;
        for (i=0; i<4; i++) {
            if (pp->pf[c][i].place == CASA || pp->pf[c][i].place == CORR) {
                njgliza++;
                break;
            }
        }
    }

    for (i=0; i<gjg->njg; i++) {
        cj = gjg->jg[i].cjugador;
        nf = gjg->jg[i].nficha;
        gjg->jg[i].njgliza = njgliza;
        gjg->jg[i].nficom = anlini.nficom[cj][nf];
        gjg->jg[i].nfipel = anlini.nfipel[cj][nf];
        gjg->jg[i].nfi6com = anlini.nfi6com[cj][nf];
        gjg->jg[i].nfi6pel = anlini.nfi6pel[cj][nf];
        pp2 = *pp;
        PPDoJg(&pp2, &(gjg->jg[i]));
        AGlobalN2(&anlfin, &pp2);
        gjg->jg[i].nffcom = anlfin.nficom[cj][nf];
        gjg->jg[i].nffpel = anlfin.nfipel[cj][nf];
        gjg->jg[i].nff6com = anlfin.nfi6com[cj][nf];
        gjg->jg[i].nff6pel = anlfin.nfi6pel[cj][nf];
        if (pp->nsix == 2) gjg->jg[i].segun6 = 1;
        if (gjg->jg[i].destino.place == CORR) {
            n = gjg->jg[i].destino.pos;
            if (n < corrent[cj]) n += 67;
            gjg->jg[i].ncfin = n - (17 * cj);
        }
        if (gjg->jg[i].come) {
            if (GJGGen(&gjg2, &pp2, cj, 20) == 0) gjg->jg[i].npc = 1;
        }
        if (gjg->jg[i].meta) {
            if (GJGGen(&gjg2, &pp2, cj, 10) == 0) gjg->jg[i].npc = 1;
        }
        for (k=0; k<4; k++) {
            dif = pp->distance[cj] - pp->distance[k];
            if (dif > gjg->jg[i].difdpri) gjg->jg[i].difdpri = dif;
        }
        cpar = (cj > 1) ? cj - 2 : cj + 2;
        gjg->jg[i].difdpar = pp->distance[cpar] - pp->distance[cj];
    }
}

void GJGFillInfoAnl(GrupoJugadas *gjg, PosPartida *pp)
{
    switch (pp->dp.robotlevel[pp->turno]) {
        case 0: break;
        case 1: break;
        case 2:
        case 3: fillInfoAnlN2(gjg, pp);
                break;
        case 4: fillInfoAnlN4(gjg, pp);
                break;
        default: fillInfoAnlN4(gjg, pp);
                break;
    }
}

static char analtxt[33][81] = {
    "nficha   ",          
    "entra    ",          
    "entra2   ",         
    "sale     ",           
    "meta     ",           
    "acasa    ",          
    "come     ",           
    "comep    ",          
    "come2    ",          
    "hpte     ",           
    "hptep    ",          
    "rpte     ",           
    "dseg     ",           
    "aseg     ",           
    "hpcor    ",          
    "cor01    ",          
    "rpte5    ",          
    "npc      ",            
    "segun6   ",         
    "nficom   ",         
    "nfipel   ",         
    "nffcom   ",         
    "nffpel   ",         
    "ncfin    ",          
    "nfi6com  ",        
    "nfi6pel  ",        
    "nff6com  ",        
    "nff6pel  ",        
    "njgliza  ",        
    "difdpri  ",        
    "difdpar  ",        
    "weight   ",
    "randw    "
};

void GJGPrintAnal(GrupoJugadas *gjg, FILE *f)
{
    int i, j, *value;
    int winfo[32] = {0};

    if (gjg->njg < 1) return;

    for (i=0; i<gjg->njg; i++) {
        sprintf(&(analtxt[0][9+i*8]), "%8d", gjg->jg[i].nficha);
        value = &(gjg->jg[i].entra);
        for (j=0; j<32; j++) {
            sprintf(&(analtxt[j+1][9+i*8]), "%8d", value[j]);
            winfo[j] += abs(value[j]);
        }
    }

    fprintf(f, "** player %d, dice %d\n",
            gjg->jg[0].cjugador, gjg->jg[0].vdado);
    for (i=0; i<33; i++) {
        if (i==0 || winfo[i-1] > 0)
            fprintf(f, "%s\n", &(analtxt[i][0]));
    }
}

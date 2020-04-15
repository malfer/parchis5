/*
 * ejugada.c --- Parchis engine, Jugada functions
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

void JGClean(Jugada *jg)
{
    jg->cjugdcom = VACIO;
    jg->cjugdcom2 = VACIO;
    jg->cjugddpl = VACIO;
    jg->nfichcom = 0;
    jg->nfichcom2 = 0;
    jg->nfichdpl = 0;
    jg->entra = 0;
    jg->entra2 = 0;
    jg->sale = 0;
    jg->meta = 0;
    jg->acasa = 0;
    jg->come = 0;
    jg->comep = 0;
    jg->come2 = 0;
    jg->hpte = 0;
    jg->hptep = 0;
    jg->rpte = 0;
    jg->dseg = 0;
    jg->aseg = 0;
    jg->hpcor = 0;
    jg->cor01 = 0;
    jg->rpte5 = 0;
    jg->npc = 0;
    jg->segun6 = 0;
    jg->nficom = 0;
    jg->nfipel = 0;
    jg->nffcom = 0;
    jg->nffpel = 0;
    jg->ncfin = 0;
    jg->nfi6com = 0;
    jg->nfi6pel = 0;
    jg->nff6com = 0;
    jg->nff6pel = 0;
    jg->njgliza = 0;
    jg->difdpri = 0;
    jg->difdpar = 0;
    jg->weight = 0;
    jg->randw = 0;
}

static int buscaIndCasa(Tablero *tbl, int color)
{
    int ix;

    for (ix=0; tbl->casa[color].cf[ix] != VACIO; ix++);

    return ix;
}


int JGGen(Jugada *jg, PosPartida *pp, Tablero *tbl,
          int cjg, int nf, int vdice, int eatmate)
{
    CasillaC *cac;
    int ind, type, i, en_llegada;

    jg->cjugador = cjg;
    jg->nficha = nf;
    jg->vdado = vdice;
    jg->origen = pp->pf[cjg][nf];

    if (jg->origen.place == META) return 0;

    /* siempre se supone que las casillas se rellenan por orden, no
       puede haber una ficha en ind=1 si no la hay en ind=0 */
    /* solo sale de casa si se saco un 5 y no hay dos fichas propias en
       la salida, si hay 2 fichas y una no es propia, ocupa su puesto */

    if (jg->origen.place == CASA) {
        if (jg->vdado != 5) return 0;
        jg->entra = 1;
        cac = &(tbl->corr[corrent[cjg]]);
        if (cac->cf[0] == cjg && cac->cf[1] == cjg) return 0;
        jg->destino.place = CORR;
        jg->destino.pos = corrent[cjg];
        jg->destino.ind = 0;
        if (cac->cf[0] == VACIO) return 1;
        if (cac->cf[1] == VACIO) {
            jg->destino.ind = 1;
            return 1;
        }
        jg->come = 1;
        /* en juego individual come la última que llegó salvo que sea propia */
        ind = 1;
        if (cac->cf[1] == cjg) ind = 0;
        /* por parejas come la última salvo que sea propia o que la otra no sea
           de su pareja */
        if (ind == 1 && pp->dp.gametype == GT_PAIRS && cac->cf[0] != cjg &&
            DPSameTeam(cac->cf[1], cjg) && !DPSameTeam(cac->cf[0], cjg))
            ind = 0;
        jg->cjugdcom = cac->cf[ind];
        jg->nfichcom = cac->nf[ind];
        PFSetPosition(&(jg->destfcom), CASA, 0, buscaIndCasa(tbl, jg->cjugdcom));
        jg->destino.ind = ind;
        /* mira si la ficha comida es pareja, aunque aqui no importa mucho */
        if (pp->dp.gametype == GT_PAIRS && DPSameTeam(cac->cf[ind], cjg))
            jg->comep = 1;
        return 1;
    }

    if (jg->origen.place == CORR) {
        /* Mira si hay que desplazar la ficha compañera */
        if (jg->origen.ind == 0) {
            jg->cjugddpl = tbl->corr[jg->origen.pos].cf[1]; // can be VACIO
            jg->nfichdpl = tbl->corr[jg->origen.pos].nf[1];
        }
        /* Mira si rompe puente */
        if (tbl->corr[jg->origen.pos].cf[0] == tbl->corr[jg->origen.pos].cf[1]) {
            jg->rpte = 1;
            /* Mira si es en salida y quedan fichas en casa */
            if (jg->origen.pos == corrent[cjg] && pp->nfcasa[cjg] > 0)
                jg->rpte5 = 1;
        }
        /* Mira si rompe puente con pareja */
        if (pp->dp.gametype == GT_PAIRS &&
            DPSameTeam(tbl->corr[jg->origen.pos].cf[0], tbl->corr[jg->origen.pos].cf[1]))
            jg->rpte = 1;
        /* Mira si deja seguro */
        type = tbl->corr[jg->origen.pos].type;
        if (type == SEGURO || type == SALIDA)
            jg->dseg = 1;
        if (type >= ENTRADA) {
            i = type - ENTRADA;
            if (i == cjg || pp->dp.playertype[i] == NOPLAY || pp->nfcasa[i] == 0 ||
                (pp->nfcasa[i] < 4 && tbl->corr[jg->origen.pos].cf[1] == VACIO))
                jg->dseg = 1;
        }
        /* Mueve */
        jg->destino.place = CORR;
        jg->destino.pos = jg->origen.pos;
        en_llegada = 0;
        for (i=0; i<vdice; i++) {
            if (jg->destino.pos == corrsal[cjg]) {
                jg->sale = 1;
                jg->destino.place = LLEG;
                jg->destino.pos = -1; /* posicion ficticia */
                vdice -= i;
                en_llegada = 1;
                break;
            }
            jg->destino.pos++;
            if (jg->destino.pos > 67) jg->destino.pos = 0;
            if (tbl->corr[jg->destino.pos].cf[1] != VACIO) {
                /* hay una barrera, pero puede ser de fichas distintas en seguro */
                /* de todas formas, si es la posicion final, no vale */
                if (i == vdice - 1) return 0;
                if (tbl->corr[jg->destino.pos].cf[0] ==
                    tbl->corr[jg->destino.pos].cf[1])
                    return 0;
                /* pero puede ser una barrera con ficha de la pareja */
                if (pp->dp.gametype == GT_PAIRS &&
                    DPSameTeam(tbl->corr[jg->destino.pos].cf[0],
                               tbl->corr[jg->destino.pos].cf[1])) return 0;
            }
        }
        if (!en_llegada) {
            /* estamos en la posicion final, ver si estamos solos o si se come */
            /* o si formamos barrera, o si estamos en seguro y no se come */
            type = tbl->corr[jg->destino.pos].type;
            if (type == SEGURO || type == SALIDA)
                jg->aseg = 1;
            if (type >= ENTRADA) {
                i = type - ENTRADA;
                if (i == cjg || pp->dp.playertype[i] == NOPLAY || pp->nfcasa[i] == 0 ||
                    (pp->nfcasa[i] < 4 && tbl->corr[jg->destino.pos].cf[0] == VACIO))
                    jg->aseg = 1;
            }
            if (tbl->corr[jg->destino.pos].cf[0] == VACIO) {
                jg->destino.ind = 0;
                return 1; /* ok, sola */
            }
            if (tbl->corr[jg->destino.pos].cf[0] == cjg) {
                jg->destino.ind = 1;
                jg->hpte = 1;
                return 1; /* ok, hace puente */
            }
            if (type != NORMAL) {
                jg->destino.ind = 1;
                /* si es pareja hace puente */
                if (pp->dp.gametype == GT_PAIRS &&
                    DPSameTeam(tbl->corr[jg->destino.pos].cf[0], cjg)) {
                    jg->hpte = 1;
                    jg->hptep = 1;
                }
                return 1; /* ok, en seguro con otra */
            }
            /* mira si la ficha es pareja, comerá o hará puente */
            if (pp->dp.gametype == GT_PAIRS &&
                DPSameTeam(tbl->corr[jg->destino.pos].cf[0], cjg)) {
                if (eatmate) {
                    jg->comep = 1;
                } else {
                    jg->destino.ind = 1;
                    jg->hpte = 1;
                    jg->hptep = 1;
                    return 1; /* ok, hace puente con pareja */
                }
            }
            jg->destino.ind = 0;
            jg->come = 1;
            jg->cjugdcom = tbl->corr[jg->destino.pos].cf[0];
            jg->nfichcom = tbl->corr[jg->destino.pos].nf[0];
            PFSetPosition(&(jg->destfcom), CASA, 0, buscaIndCasa(tbl, jg->cjugdcom));
            return 1; /* ok, come */
        }
    }

    if (jg->origen.place == LLEG) {
        /* Mira si hay que desplazar la ficha compañera */
        if (jg->origen.ind == 0) {
            jg->cjugddpl = tbl->lleg[cjg][jg->origen.pos].cf[1]; // can be VACIO
            jg->nfichdpl = tbl->lleg[cjg][jg->origen.pos].nf[1];
        }
        jg->destino.place = LLEG;
        jg->destino.pos = jg->origen.pos;
    }
    /* si estamos aquí es porque el origen es LLEGADA */
    /* o CORREDOR con el dado descontado */
    if (jg->destino.pos + vdice > 7) return 0;
    /* mirar si hay algún puente en el camino (gracias M.Angeles) */
    for (i=1; i<vdice; i++) {
        if (tbl->lleg[cjg][jg->destino.pos+i].cf[1] != VACIO)
            return 0; /* hay puente */
    }
    /* camino libre */
    if (jg->destino.pos + vdice == 7) {
        jg->destino.place = META;
        jg->destino.pos = 0;
        jg->meta = 1;
        for (i=0; tbl->meta[cjg].cf[i] != VACIO; i++);
        jg->destino.ind = i;
        return 1; /* ok, en la meta */
    }
    jg->destino.pos += vdice;
    if (tbl->lleg[cjg][jg->destino.pos].cf[1] != VACIO)
        return 0;
    /* mira si sale de las 2 primeras casillas */
    if (jg->origen.place == LLEG && jg->origen.pos < 2 && jg->destino.pos > 1)
        jg->cor01 = 1;
    jg->destino.ind = 0;
    if (tbl->lleg[cjg][jg->destino.pos].cf[0] != VACIO) {
        jg->destino.ind = 1;
        jg->hpcor = 1; /* hace puente en corredor, malo */
    }
    return 1;
}

int JGGenGoHome(Jugada *jg, PosPartida *pp, Tablero *tbl,
                int cjg, int nf, int vdice)
{
    jg->cjugador = cjg;
    jg->nficha = nf;
    jg->vdado = vdice;
    jg->origen = pp->pf[cjg][nf];

    if (jg->origen.place != CORR) return 0; // pero debe ser

    /* Mira si hay que desplazar la ficha compañera */
    if (jg->origen.ind == 0) {
        jg->cjugddpl = tbl->corr[jg->origen.pos].cf[1]; // can be VACIO
        jg->nfichdpl = tbl->corr[jg->origen.pos].nf[1];
    }

    jg->acasa = 1;
    PFSetPosition(&(jg->destino), CASA, 0, buscaIndCasa(tbl, cjg));
    return 1;
}

int JGGenFirst5(Jugada *jg, PosPartida *pp, Tablero *tbl, int cjg)
{
    CasillaC *cac;

    jg->cjugador = cjg;
    jg->nficha = 2;
    jg->nficha2 = 3;
    jg->vdado = 5;
    jg->origen = pp->pf[cjg][jg->nficha];
    jg->origen2 = pp->pf[cjg][jg->nficha2];

    if (jg->origen.place != CASA) return 0; // pero debe ser
    if (jg->origen2.place != CASA) return 0; // pero debe ser

    jg->entra = 1;
    jg->entra2 = 1;
    cac = &(tbl->corr[corrent[cjg]]);
    jg->destino.place = CORR;
    jg->destino.pos = corrent[cjg];
    jg->destino.ind = 0;
    jg->destino2.place = CORR;
    jg->destino2.pos = corrent[cjg];
    jg->destino2.ind = 1;
    if (cac->cf[0] != VACIO) {
        jg->come = 1;
        jg->cjugdcom = cac->cf[0];
        jg->nfichcom = cac->nf[0];
        PFSetPosition(&(jg->destfcom), CASA, 0, buscaIndCasa(tbl, jg->cjugdcom));
        if (cac->cf[1] != VACIO) {
            jg->come2 = 1;
            jg->cjugdcom2 = cac->cf[1];
            jg->nfichcom2 = cac->nf[1];
            PFSetPosition(&(jg->destfcom2), CASA, 0, buscaIndCasa(tbl, jg->cjugdcom2));
            if (jg->cjugdcom == jg->cjugdcom2)
                jg->destfcom2.ind++; // porque ya está ocupado
        }
    }
    return 1;
}

static int calcWeightN1(Jugada *jg)
{
    int weight = 0;

    if (jg->sale) weight += 100;
    if (jg->meta) weight += 100;
    if (jg->come) weight += 150;
    if (jg->comep) weight -= 160;  /* no comer pareja */
    if (jg->dseg) weight -= 20;
    if (jg->aseg) weight += 20;
    if (jg->dseg && jg->aseg) weight += 30;

    return weight;
}

static int calcWeightN2(Jugada *jg)
{
    int weight = 0;

    if (jg->sale) weight += 100;
    if (jg->meta) weight += 200;
    if (jg->come) weight += 500;
    if (jg->comep) weight -= 510;  /* no comer pareja */
    if (jg->dseg) weight -= 20;
    if (jg->aseg) weight += 20;
    if (jg->dseg && jg->aseg) weight += 30;

    return weight;
}

static int calcWeightN3(Jugada *jg)
{
    int weight = 0;

    if (jg->sale) weight += 100;
    if (jg->meta) weight += 200;
    if (jg->come && !jg->comep) weight += 500;
    if (jg->hpte) weight -= 15;
    if (jg->dseg) weight -= 20;
    if (jg->aseg) weight += 20;
    if (jg->dseg && jg->aseg) weight += 30;
    weight += (jg->nffcom - jg->nficom) * 55;
    weight += (jg->nfipel - jg->nffpel) * 50;
    weight += jg->ncfin / 5;

    return weight;
}

static int calcWeightN4(Jugada *jg)
{
    int weight = 0;

    if (jg->sale) weight += 100;
    if (jg->meta) weight += jg->npc ? 10 : 200;
    if (jg->come && !jg->comep) weight += jg->npc ? 20 : 500;
    if (jg->comep && !jg->npc && jg->difdpri > 50 && jg->difdpar > 50) weight += 100;
    if (jg->njgliza > 1) {
        if (jg->hpte) weight -= 15;
        if (jg->dseg) weight -= 20;
        if (jg->aseg) weight += 20;
        if (jg->dseg && jg->aseg) weight += 30;
    }
    weight += (jg->nffcom - jg->nficom) * 55;
    weight += (jg->nfipel - jg->nffpel) * 50;
    weight += (jg->nff6com - jg->nfi6com) * 5;
    weight += (jg->nfi6pel - jg->nff6pel) * 4;
    if (jg->segun6) {
        if (jg->destino.place == CORR)
            weight -= jg->ncfin / 5;
        else
            weight += 10;
    } else {
        weight += jg->ncfin / 5;
        //weight += (jg->nfipel - jg->nffpel) * (30 + jg->ncfin);
    }
    //weight += jg->ncfin / 5;

    return weight;
}

int JGCalcWeight(Jugada *jg, int level, int gametype)
{
    int weight = 0;

    if (level == 0) {  // nivel 0 juega a lo loco
        jg->randw = random() % 10;
    } else if (level == 1) {
        jg->randw = random() % 200;
        weight = calcWeightN1(jg);
    } else if (level == 2) {
        jg->randw = random() % 30;
        weight = calcWeightN2(jg);
    } else if (level == 3) {
        jg->randw = random() % 10;
        weight = calcWeightN3(jg);
    } else if (level >= 4) {
        jg->randw = 0;
        weight = calcWeightN4(jg);
    }

    jg->weight = weight + jg->randw;
    
    return jg->weight;
}
/*
int JGCalcWeight(Jugada *jg, int level, int gametype)
{
    int weight = 0;

    if (level == 0) {  // nivel 0 juega a lo loco
        weight += random() % 10;
    } else if (level == 1) {
        weight = calcWeightN1(jg);
    } else if (level == 2) {
        weight = calcWeightN2(jg);
    } else if (level == 3) {
        weight = calcWeightN3(jg);
    } else if (level >= 4) {
        weight = calcWeightN4(jg);
    }

    return weight;
}
*/

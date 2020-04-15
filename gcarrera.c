/*
 * gcarrera.c --- Carrera functions
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
#include <math.h>
#include <grgui.h>
#include <mgrxcolr.h>
#include "engine.h"
#include "gparchis.h"

GCarrera globcar;

void set_globcarr(Jugada *jg)
{
    int i, c2;

    globcar.jg = *jg;
    globcar.stepsentra = 0;
    globcar.stepspasos = jg->vdado - 1;
    if (globcar.stepspasos > 20) globcar.stepspasos = 20;
    globcar.stepscome = 0;
    globcar.stepsacasa = 0;
    globcar.stepsmeta = 0;
    globcar.stepsfin = 0;
    globcar.mode = 1;
    globcar.step = 0;

    if (jg->entra && !globgstatus.pawnfingermoved) {
        globcar.mode = 0;
        globcar.stepspasos = 0;
        globcar.stepsentra = 5;
        set_gposslide_from_posfichas(&(globcar.gpsentra), &(jg->origen),
                                     &(jg->destino), jg->cjugador,
                                     globcar.stepsentra);
        if (jg->entra2) {
            set_gposslide_from_posfichas(&(globcar.gpsentra2), &(jg->origen2),
                                         &(jg->destino2), jg->cjugador,
                                         globcar.stepsentra);
        }
    }

    if (jg->acasa && !globgstatus.pawnfingermoved) {
        globcar.mode = 3;
        globcar.stepspasos = 0;
        globcar.stepsacasa = 12;
        set_gposslide_from_posfichas(&(globcar.gpsacasa), &(jg->origen),
                                     &(jg->destino), jg->cjugador,
                                     globcar.stepsacasa);
    }

    if (globgstatus.pawnfingermoved) globcar.stepspasos = 0;
    for (i=0; i<globcar.stepspasos; i++) {
        if (i == 0) {
            globcar.paso[0].place = jg->origen.place;
            globcar.paso[0].pos = jg->origen.pos + 1;
            globcar.paso[0].ind = 0;
        } else {
            globcar.paso[i].place = globcar.paso[i-1].place;
            globcar.paso[i].pos = globcar.paso[i-1].pos + 1;
            globcar.paso[i].ind = 0;
        }
        if (globcar.paso[i].place == CORR) {
            if (globcar.paso[i].pos-1 == corrsal[jg->cjugador]) {
                globcar.paso[i].place = LLEG;
                globcar.paso[i].pos = 0;
            } else if (globcar.paso[i].pos > 67) {
                globcar.paso[i].pos = 0;
            }
        }
    }

    if (jg->come) {
        globcar.stepscome = 12;
        set_gposslide_from_posfichas(&(globcar.gpscome), &(jg->destino),
                                     &(jg->destfcom), jg->cjugdcom,
                                     globcar.stepscome);
        if (jg->come2) {
            set_gposslide_from_posfichas(&(globcar.gpscome2), &(jg->destino2),
                                         &(jg->destfcom2), jg->cjugdcom2,
                                         globcar.stepscome);
        }
    }

    if (jg->meta) {
        globcar.stepsmeta = 13;
        
        if (jg->destino.ind == 3) { // fin
            globcar.stepsfin = 12;
            set_gposslide_for_medals(&(globcar.gpsfin), jg->cjugador,
                                     globcar.stepsfin);
            if (globpt.pp.dp.gametype == GT_PAIRS) {
                c2 = (jg->cjugador < 2) ? jg->cjugador + 2 : jg->cjugador - 2;
                set_gposslide_for_medals(&(globcar.gpsfin2), c2, globcar.stepsfin);
            }
        }
    }

    globcar.stepstotal = globcar.stepsentra + globcar.stepspasos + globcar.stepscome +
                      globcar.stepsacasa + globcar.stepsmeta + globcar.stepsfin;
}

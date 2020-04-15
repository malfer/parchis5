/*
 * ganimat.c --- graphic animations
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

#define MAX_ANIMATIONS 10

#define DICETO       60
#define ROTATETO     20
#define RPASSTO     100
#define RSHOWTO     150
#define CARRERATO    12

static Animation *anitbl[MAX_ANIMATIONS];

/***********************/

void AnimationsInit(void)
{
    int i;

    for (i=0; i<MAX_ANIMATIONS; i++) {
        anitbl[i] = NULL;
    }
}

/***********************/

int AnimationsAdd(Animation *ani)
{
    int i;

    for (i=0; i<MAX_ANIMATIONS; i++) {
        if (anitbl[i] == NULL) {
            anitbl[i] = ani;
            ani->lasttime = 0;
            ani->stepnumber = 0;
            ani->running = 1;
            return i;
        }
    }

    return -1;
}

/***********************/

void AnimationsRun(long time)
{
    int i, ret;

    for (i=0; i<MAX_ANIMATIONS; i++) {
        if (anitbl[i] != NULL) {
            if (time > anitbl[i]->lasttime + anitbl[i]->steptime) {
                ret = (anitbl[i]->doNextStep)(anitbl[i], 0);
                if (ret == -1) {
                    anitbl[i]->running = 0;
                    anitbl[i] = NULL;
                } else {
                    anitbl[i]->lasttime = time;
                    anitbl[i]->stepnumber++;
                }
            }
        }
    }
}

/***********************/

void AnimationsStopAll(void)
{
    int i;

    for (i=0; i<MAX_ANIMATIONS; i++) {
        if (anitbl[i] != NULL) {
            //anitbl[i]->stepnumber = anitbl[i]->numsteps;
            (anitbl[i]->doNextStep)(anitbl[i], 1);
            anitbl[i]->running = 0;
            anitbl[i] = NULL;
        }
    }
}

/***********************/

static Animation dice_ani = {0};
static struct st_dice_data {
    GrColor color;
    int vdado;
} dice_data;
static int dice_order[6] = {5, 2, 1, 4, 6, 3};

static int diceAniDoNextStep(Animation *ani, int stop)
{
    struct st_dice_data *dd;
    int vaux;
    
    dd = (struct st_dice_data *)ani->data;

    if (ani->stepnumber >= ani->numsteps || stop) {
        paint_dice(dd->vdado, dd->color);
        globgstatus.workingdice = 2;
        return -1;
    }

    vaux = ani->stepnumber % 6;
    paint_dice(dice_order[vaux], dd->color);
    
    return 0;
}

void StartDiceAnimation(int nsteps, int vdado, GrColor bg)
{
    if (dice_ani.running) return;

    dice_ani.numsteps = nsteps;
    dice_ani.steptime = DICETO;
    dice_ani.doNextStep = diceAniDoNextStep;
    dice_data.vdado = vdado;
    dice_data.color = bg;
    dice_ani.data = (void *)&dice_data;
    if (AnimationsAdd(&dice_ani) < 0) {
        diceAniDoNextStep(&dice_ani, 1);
    }
}

/***********************/

static Animation rotbl_ani = {0};
static int angincr = 1;
static int angles[360];
static int numangles = 0;

static int rotblAniDoNextStep(Animation *ani, int stop)
{
    if (ani->stepnumber >= ani->numsteps) {
        globgstatus.rotating = 0;
        return -1;
    }
    
    if (stop) {
        globcfg.rotang += angincr * (ani->numsteps - ani->stepnumber);
        if (globcfg.rotang == 360 || globcfg.rotang == -360) globcfg.rotang = 0;
        paint_board();
        globgstatus.rotating = 0;
        return -1;
    }

    globcfg.rotang += angincr;
    if (globcfg.rotang == 360 || globcfg.rotang == -360) globcfg.rotang = 0;
    paint_board();
    
    return 0;
}

void StartRoTblAnimation(int nsteps, int incr)
{
    if (rotbl_ani.running) return;

    rotbl_ani.numsteps = nsteps;
    angincr = incr;
    rotbl_ani.steptime = ROTATETO;
    rotbl_ani.doNextStep = rotblAniDoNextStep;
    rotbl_ani.data = NULL;
    globgstatus.rotating = 1;
    if (AnimationsAdd(&rotbl_ani) < 0) {
        rotblAniDoNextStep(&rotbl_ani, 1);
    }
}

static int rotblAniDoNextStep2(Animation *ani, int stop)
{
    if (ani->stepnumber >= ani->numsteps) {
        globgstatus.rotating = 0;
        return -1;
    }
    
    if (stop) {
        globcfg.rotang = angles[numangles-1];
        paint_board();
        globgstatus.rotating = 0;
        return -1;
    }

    globcfg.rotang = angles[ani->stepnumber];
    paint_board();
    
    return 0;
}

void StartRoTblAnimation2(int rotfinal)
{
    int rotdist, dir, steps3, steps2, rotang3, rotang2, i;

    if (rotbl_ani.running) return;

    rotdist = rotfinal - globcfg.rotang;
    if (globcfg.rotang >= 315) rotdist = 360 - globcfg.rotang;
    dir = 1;
    if (rotdist < 0) {
        rotdist *= -1;
        dir = -1;
    }
    steps3 = 0;
    rotang3 = globcfg.rotang;
    if (rotdist > 20) {
        steps3 = (rotdist - 18) / 3;
        for (i=0; i<steps3; i++) {
            angles[i] = globcfg.rotang + ((i + 1) * 3 * dir);
        }
        rotang3 = angles[steps3-1];
    }
    rotdist -= steps3 * 3;
    steps2 = 0;
    rotang2 = rotang3;
    if (rotdist > 5) {
        steps2 = (rotdist - 4) / 2;
        for (i=0; i<steps2; i++) {
            angles[i+steps3] = rotang3 + ((i + 1) * 2 * dir);
        }
        rotang2 = angles[steps3+steps2-1];
    }
    rotdist -= steps2 * 2;
    for (i=0; i<rotdist; i++) {
        angles[i+steps3+steps2] = rotang2 + ((i + 1) * dir);
    }
    numangles = steps3 + steps2 + rotdist;        
    rotbl_ani.numsteps = numangles;
    rotbl_ani.steptime = ROTATETO;
    rotbl_ani.doNextStep = rotblAniDoNextStep2;
    rotbl_ani.data = NULL;
    globgstatus.rotating = 1;
    if (AnimationsAdd(&rotbl_ani) < 0) {
        rotblAniDoNextStep(&rotbl_ani, 1);
    }
}

/***********************/

static Animation rpass_ani = {0};

static int rpassAniDoNextStep(Animation *ani, int stop)
{
    if (ani->stepnumber >= ani->numsteps || stop) {
        globgstatus.robotpassing = 2;
        return -1;
    }

    return 0;
}

void StartRPassAnimation(int nsteps)
{
    if (rpass_ani.running) return;

    rpass_ani.numsteps = nsteps;
    rpass_ani.steptime = RPASSTO;
    rpass_ani.doNextStep = rpassAniDoNextStep;
    rpass_ani.data = NULL;
    if (AnimationsAdd(&rpass_ani) < 0) {
        rpassAniDoNextStep(&rpass_ani, 1);
    }
}

/***********************/

static Animation rshow_ani = {0};

static int rshowAniDoNextStep(Animation *ani, int stop)
{
    if (ani->stepnumber >= ani->numsteps || stop) {
        globgstatus.robotpawnshow = 2;
        globgstatus.blinkpawn = 0;
        return -1;
    }

    globgstatus.blinkpawn = !globgstatus.blinkpawn;
    paint_board();
    return 0;
}

void StartRShowAnimation(int nsteps)
{
    if (rshow_ani.running) return;

    rshow_ani.numsteps = nsteps;
    rshow_ani.steptime = RSHOWTO;
    rshow_ani.doNextStep = rshowAniDoNextStep;
    rshow_ani.data = NULL;
    if (AnimationsAdd(&rshow_ani) < 0) {
        rshowAniDoNextStep(&rshow_ani, 1);
    }
}

/***********************/

static Animation carrera_ani = {0};

static int carreraAniDoNextStep(Animation *ani, int stop)
{
    if (ani->stepnumber >= ani->numsteps || stop) {
        globgstatus.oncarrera = 2;
        return -1;
    }

    paint_board();

    if (globcar.mode == 0) { // entra
        globcar.step++;
        if (globcar.step >= globcar.stepsentra) {
            if (globcar.jg.come) {
                globcar.mode = 2;
                globcar.step = 0;
            }
        }
    } else if (globcar.mode == 1) { // normal move
        globcar.step++;
        if (globcar.step >= globcar.stepspasos) {
            if (globcar.jg.come) {
                globcar.mode = 2;
                globcar.step = 0;
            } else if (globcar.jg.meta) {
                globcar.mode = 4;
                globcar.step = 0;
            }
        }
    } else if (globcar.mode == 2) { // come
        globcar.step++;
    } else if (globcar.mode == 3) { // acasa
        globcar.step++;
    } else if (globcar.mode == 4) { // meta
        globcar.step++;
        if (globcar.step >= globcar.stepsmeta) {
            if (globcar.stepsfin > 0) {
                globcar.mode = 5;
                globcar.step = 0;
            }
        }
    } else if (globcar.mode == 5) { // fin
        globcar.step++;
    }

    return 0;
}

void StartCarreraAnimation(int speed)
{
    if (carrera_ani.running) return;

    if (speed == 0 || globcar.stepstotal == 0) {
        globgstatus.oncarrera = 2;
        return;
    }

    carrera_ani.numsteps = globcar.stepstotal;
    carrera_ani.steptime = CARRERATO * speed;
    carrera_ani.doNextStep = carreraAniDoNextStep;
    carrera_ani.data = NULL;
    if (AnimationsAdd(&carrera_ani) < 0) {
        carreraAniDoNextStep(&carrera_ani, 1);
    }
}

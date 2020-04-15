/*
 * dlgnewgm.c --- new game dialog
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
#include <grgui.h>
#include <mgrxcolr.h>
#include "engine.h"
#include "gparchis.h"
#include "i18n.h"

#define COMMAND_YH           1 // note commands are color*10 + op
#define COMMAND_YR0          2
#define COMMAND_YR1          3
#define COMMAND_YR2          4
#define COMMAND_YR3          5
#define COMMAND_YR4          6
#define COMMAND_YNP          7
#define COMMAND_BH          11
#define COMMAND_BR0         12
#define COMMAND_BR1         13
#define COMMAND_BR2         14
#define COMMAND_BR3         15
#define COMMAND_BR4         16
#define COMMAND_BNP         17
#define COMMAND_RH          21
#define COMMAND_RR0         22
#define COMMAND_RR1         23
#define COMMAND_RR2         24
#define COMMAND_RR3         25
#define COMMAND_RR4         26
#define COMMAND_RNP         27
#define COMMAND_GH          31
#define COMMAND_GR0         32
#define COMMAND_GR1         33
#define COMMAND_GR2         34
#define COMMAND_GR3         35
#define COMMAND_GR4         36
#define COMMAND_GNP         37

#define COMMAND_GTSINGLE    81
#define COMMAND_GTPAIRS     82
#define COMMAND_OPTIONS     91

#define COMMAND_OK         101
#define COMMAND_CANCEL     102

static GUIDialog *newgmdlg = NULL;
static GUIGroup *newgmgo = NULL;
static DefPartida gdp;

static int optDlgCreate(void);
static void optDlgDestroy(void);
static int optDlgRun(void);

/***********************/

int process_newgmdlg_event(void *udata, GrEvent *ev)
{
    int c, op;

    GUIDialog *d = (GUIDialog *)udata;
    GUIGroup *go = (GUIGroup *)(d->exdata);

    if (ev->type == GREV_COMMAND) {
        switch (ev->p1) {
            case COMMAND_GTSINGLE :
                gdp.gametype = GT_SINGLE;
                break;
            case COMMAND_GTPAIRS :
                gdp.gametype = GT_PAIRS;
                break;
            case COMMAND_OPTIONS :
                optDlgCreate();
                optDlgRun();
                break;
            case COMMAND_OK :
                d->result = 1;
                return -1;
            case COMMAND_CANCEL :
                d->result = -1;
                return -1;
            default :
                c = ev->p1 / 10;
                if (c < AMAR || c > VERD) break;
                op = ev->p1 - c * 10;
                if (op < 1 || op > 7) break;
                if (op == 1)
                    gdp.playertype[c] = PERSON;
                else if (op == 7)
                    gdp.playertype[c] = NOPLAY;
                else {
                    gdp.playertype[c] = ROBOT;
                    gdp.robotlevel[c] = op - 2;
                }
        }
    }

    return GUIGroupProcessEvent(go, ev);
}

int NewGameDlgCreate(void)
{
    if (newgmdlg) return 1; // dialog already created

    newgmgo = GUIGroupCreate(34, 0, 0);
    if (newgmgo == NULL) return 0;
    GUIObjectSetButton(&(newgmgo->o[ 0]),  0,   0,   0,180, 45, WEBC_GREEN, WEBC_YELLOW, _(SDNG_SGAME), COMMAND_GTSINGLE, 105, 1);
    GUIObjectSetButton(&(newgmgo->o[ 1]),  1, 180,   0,180, 45, WEBC_GREEN, WEBC_YELLOW, _(SDNG_PGAME), COMMAND_GTPAIRS, 105, 1);
    GUIObjectSetButton(&(newgmgo->o[ 2]),  2,   0,  50, 60, 40, playercolor[0], WEBC_YELLOW, _(SDNG_H),  COMMAND_YH,  101, 1);
    GUIObjectSetButton(&(newgmgo->o[ 3]),  3,  65,  50, 46, 40, playercolor[0], WEBC_YELLOW, _(SDNG_R0), COMMAND_YR0, 101, 0);
    GUIObjectSetButton(&(newgmgo->o[ 4]),  4, 111,  50, 46, 40, playercolor[0], WEBC_YELLOW, _(SDNG_R1), COMMAND_YR1, 101, 0);
    GUIObjectSetButton(&(newgmgo->o[ 5]),  5, 157,  50, 46, 40, playercolor[0], WEBC_YELLOW, _(SDNG_R2), COMMAND_YR2, 101, 0);
    GUIObjectSetButton(&(newgmgo->o[ 6]),  6, 203,  50, 46, 40, playercolor[0], WEBC_YELLOW, _(SDNG_R3), COMMAND_YR3, 101, 0);
    GUIObjectSetButton(&(newgmgo->o[ 7]),  7, 249,  50, 46, 40, playercolor[0], WEBC_YELLOW, _(SDNG_R4), COMMAND_YR4, 101, 0);
    GUIObjectSetButton(&(newgmgo->o[ 8]),  8, 300,  50, 60, 40, playercolor[0], WEBC_YELLOW, _(SDNG_NP), COMMAND_YNP, 101, 0);
    GUIObjectSetButton(&(newgmgo->o[ 9]),  9,   0,  90, 60, 40, playercolor[1], WEBC_YELLOW, _(SDNG_H),  COMMAND_BH,  102, 1);
    GUIObjectSetButton(&(newgmgo->o[10]), 10,  65,  90, 46, 40, playercolor[1], WEBC_YELLOW, _(SDNG_R0), COMMAND_BR0, 102, 0);
    GUIObjectSetButton(&(newgmgo->o[11]), 11, 111,  90, 46, 40, playercolor[1], WEBC_YELLOW, _(SDNG_R1), COMMAND_BR1, 102, 0);
    GUIObjectSetButton(&(newgmgo->o[12]), 12, 157,  90, 46, 40, playercolor[1], WEBC_YELLOW, _(SDNG_R2), COMMAND_BR2, 102, 0);
    GUIObjectSetButton(&(newgmgo->o[13]), 13, 203,  90, 46, 40, playercolor[1], WEBC_YELLOW, _(SDNG_R3), COMMAND_BR3, 102, 0);
    GUIObjectSetButton(&(newgmgo->o[14]), 14, 249,  90, 46, 40, playercolor[1], WEBC_YELLOW, _(SDNG_R4), COMMAND_BR4, 102, 0);
    GUIObjectSetButton(&(newgmgo->o[15]), 15, 300,  90, 60, 40, playercolor[1], WEBC_YELLOW, _(SDNG_NP), COMMAND_BNP, 102, 0);
    GUIObjectSetButton(&(newgmgo->o[16]), 16,   0, 130, 60, 40, playercolor[2], WEBC_YELLOW, _(SDNG_H),  COMMAND_RH,  103, 1);
    GUIObjectSetButton(&(newgmgo->o[17]), 17,  65, 130, 46, 40, playercolor[2], WEBC_YELLOW, _(SDNG_R0), COMMAND_RR0, 103, 0);
    GUIObjectSetButton(&(newgmgo->o[18]), 18, 111, 130, 46, 40, playercolor[2], WEBC_YELLOW, _(SDNG_R1), COMMAND_RR1, 103, 0);
    GUIObjectSetButton(&(newgmgo->o[19]), 19, 156, 130, 46, 40, playercolor[2], WEBC_YELLOW, _(SDNG_R2), COMMAND_RR2, 103, 0);
    GUIObjectSetButton(&(newgmgo->o[20]), 20, 203, 130, 46, 40, playercolor[2], WEBC_YELLOW, _(SDNG_R3), COMMAND_RR3, 103, 0);
    GUIObjectSetButton(&(newgmgo->o[21]), 21, 249, 130, 46, 40, playercolor[2], WEBC_YELLOW, _(SDNG_R4), COMMAND_RR4, 103, 0);
    GUIObjectSetButton(&(newgmgo->o[22]), 22, 300, 130, 60, 40, playercolor[2], WEBC_YELLOW, _(SDNG_NP), COMMAND_RNP, 103, 0);
    GUIObjectSetButton(&(newgmgo->o[23]), 23,   0, 170, 60, 40, playercolor[3], WEBC_YELLOW, _(SDNG_H),  COMMAND_GH,  104, 1);
    GUIObjectSetButton(&(newgmgo->o[24]), 24,  65, 170, 46, 40, playercolor[3], WEBC_YELLOW, _(SDNG_R0), COMMAND_GR0, 104, 0);
    GUIObjectSetButton(&(newgmgo->o[25]), 25, 111, 170, 46, 40, playercolor[3], WEBC_YELLOW, _(SDNG_R1), COMMAND_GR1, 104, 0);
    GUIObjectSetButton(&(newgmgo->o[26]), 26, 156, 170, 46, 40, playercolor[3], WEBC_YELLOW, _(SDNG_R2), COMMAND_GR2, 104, 0);
    GUIObjectSetButton(&(newgmgo->o[27]), 27, 203, 170, 46, 40, playercolor[3], WEBC_YELLOW, _(SDNG_R3), COMMAND_GR3, 104, 0);
    GUIObjectSetButton(&(newgmgo->o[28]), 28, 249, 170, 46, 40, playercolor[3], WEBC_YELLOW, _(SDNG_R4), COMMAND_GR4, 104, 0);
    GUIObjectSetButton(&(newgmgo->o[29]), 29, 300, 170, 60, 40, playercolor[3], WEBC_YELLOW, _(SDNG_NP), COMMAND_GNP, 104, 0);
    GUIObjectSetLabel(&(newgmgo->o[30]),  30,   0, 210,360, 20, GrNOCOLOR, WEBC_BLACK, _(SDNG_EXPL));
    GUIObjectSetButton(&(newgmgo->o[31]), 31,   0, 230,100, 40, WEBC_TAN, WEBC_BLACK, _(SGN_OK), COMMAND_OK, 0, 0);
    GUIObjectSetButton(&(newgmgo->o[32]), 32, 130, 230,100, 40, WEBC_TAN, WEBC_BLACK, _(SDNG_OPT), COMMAND_OPTIONS, 0, 0);
    GUIObjectSetButton(&(newgmgo->o[33]), 33, 260, 230,100, 40, WEBC_TAN, WEBC_BLACK, _(SGN_CANCEL), COMMAND_CANCEL, 0, 0);
    GUIGroupSetSelected(newgmgo, 0, 0);

    newgmdlg = GUIGroupDialogCreate(_(SDNG_TITLE),
                                    newgmgo, process_newgmdlg_event);
    if (newgmdlg == NULL) {
        GUIGroupDestroy(newgmgo);
        newgmgo = NULL;
        return 0;
    }

    return 1;
}

void NewGameDlgDestroy(void)
{
    optDlgDestroy();

    if (newgmdlg) {
        GUIDialogDestroy(newgmdlg);
        newgmdlg = NULL;
    }
    if (newgmgo) {
        GUIGroupDestroy(newgmgo);
        newgmgo = NULL;
    }
}

void NewGameDlgSetData(DefPartida *dp)
{
    int c;
    int idbut;

    if (!newgmdlg) return;

    gdp = *dp;

    if (dp->gametype == GT_SINGLE) {
        GUIGroupSetOn(newgmgo, 0, 0);
    } else {
        GUIGroupSetOn(newgmgo, 1, 0);
    }

    for (c=AMAR; c<=VERD; c++) {
        idbut = c * 7;
        if (dp->playertype[c] == ROBOT)
            idbut += dp->robotlevel[c] + 1;
        else if (dp->playertype[c] == NOPLAY)
            idbut += 6;
        idbut += 2; // first object on group
        GUIGroupSetOn(newgmgo, idbut, 0);
    }
}

void NewGameDlgGetData(DefPartida *dp)
{
    if (!newgmdlg) return;

    *dp = gdp;
    DPCountPlayers(dp);
}

int NewGameDlgRun(void)
{
    if (!newgmdlg) return -1; 

    newgmdlg->result = -1;
    GUIDialogRun(newgmdlg);
    return newgmdlg->result;
}

/***********************/

#define COMOPT_WHO          1
#define COMOPT_FE           2
#define COMOPT_FRE          3
#define COMOPT_SFE          4
#define COMOPT_MD           5

#define COMOPT_OK         101
#define COMOPT_CANCEL     102

static GUIDialog *optdlg = NULL;
static GUIGroup *optgo = NULL;
static DefPartida gdp2;

static void setLight(int on, int ind, int paint)
{
    char *s;

    s = on ? _(SGN_ON) : _(SGN_OFF);
    GUIGroupSetText(optgo, ind, s);

    if (GUIGroupGetOn(optgo, ind)) GUIGroupSetOn(optgo, ind, 0);
    if (on) GUIGroupSetOn(optgo, ind, 0);
    if (paint) GUIGroupRePaintObject(optgo, ind);
}

static void setWhoBeginColor(GrColor c, int paint)
{
    if (!optdlg) return;
    // ToDO, add fucntion to change color to grgui.h instead this!!
    optgo->o[1].bg = c;
    if (paint) GUIGroupRePaintObject(optgo, 1);
}

static int process_optdlg_event(void *udata, GrEvent *ev)
{
    GUIDialog *d = (GUIDialog *)udata;
    GUIGroup *go = (GUIGroup *)(d->exdata);

    if (ev->type == GREV_COMMAND) {
        switch (ev->p1) {
            case COMOPT_WHO :
                gdp2.whobegin = (gdp2.whobegin >= 3) ? 0 : gdp2.whobegin + 1;
                setWhoBeginColor(playercolor[gdp2.whobegin], 1);
                return 1;
            case COMOPT_FE :
                gdp2.fastend = ! gdp2.fastend;
                setLight(gdp2.fastend, 3, 1);
                return 1;
            case COMOPT_FRE :
                gdp2.fastrobotend = ! gdp2.fastrobotend;
                setLight(gdp2.fastrobotend, 5, 1);
                return 1;
            case COMOPT_SFE :
                gdp2.superfastend = ! gdp2.superfastend;
                setLight(gdp2.superfastend, 7, 1);
                return 1;
            case COMOPT_MD :
                gdp2.manualdice = ! gdp2.manualdice;
                setLight(gdp2.manualdice, 9, 1);
                return 1;
            case COMOPT_OK :
                gdp = gdp2;
                d->result = 1;
                return -1;
            case COMOPT_CANCEL :
                d->result = -1;
                return -1;
        }
    }

    return GUIGroupProcessEvent(go, ev);
}

static int optDlgCreate(void)
{
    if (optdlg) return 1; // dialog already created

    optgo = GUIGroupCreate(12, 0, 0);
    if (optgo == NULL) return 0;
    GUIObjectSetButton(&(optgo->o[ 0]),  0,   0,  10,270,  40, WEBC_GREEN, WEBC_YELLOW, _(SDGO_WHOB), COMOPT_WHO, 0, 0);
    GUIObjectSetLight( &(optgo->o[ 1]),  1, 280,  15, 80,  30, playercolor[0], WEBC_YELLOW, "", 1);
    GUIObjectSetButton(&(optgo->o[ 2]),  2,   0,  50,270,  40, WEBC_GREEN, WEBC_YELLOW, _(SDGO_FEND), COMOPT_FE, 0, 0);
    GUIObjectSetLight( &(optgo->o[ 3]),  3, 280,  55, 80,  30, WEBC_ORANGE, WEBC_YELLOW, _(SGN_OFF), 0);
    GUIObjectSetButton(&(optgo->o[ 4]),  4,   0,  90,270,  40, WEBC_GREEN, WEBC_YELLOW, _(SDGO_FREND), COMOPT_FRE, 0, 0);
    GUIObjectSetLight( &(optgo->o[ 5]),  5, 280,  95, 80,  30, WEBC_ORANGE, WEBC_YELLOW, _(SGN_OFF), 0);
    GUIObjectSetButton(&(optgo->o[ 6]),  6,   0, 130,270,  40, WEBC_GREEN, WEBC_YELLOW, _(SDGO_SFEND), COMOPT_SFE, 0, 0);
    GUIObjectSetLight( &(optgo->o[ 7]),  7, 280, 135, 80,  30, WEBC_ORANGE, WEBC_YELLOW, _(SGN_OFF), 0);
    GUIObjectSetButton(&(optgo->o[ 8]),  8,   0, 170,270,  40, WEBC_GREEN, WEBC_YELLOW, _(SDGO_MDICE), COMOPT_MD, 0, 0);
    GUIObjectSetLight( &(optgo->o[ 9]),  9, 280, 175, 80,  30, WEBC_ORANGE, WEBC_YELLOW, _(SGN_OFF), 0);

    GUIObjectSetButton(&(optgo->o[10]), 10,   0, 230,100, 40, WEBC_TAN, WEBC_BLACK, _(SGN_OK), COMOPT_OK, 0, 0);
    GUIObjectSetButton(&(optgo->o[11]), 11, 260, 230,100, 40, WEBC_TAN, WEBC_BLACK, _(SGN_CANCEL), COMOPT_CANCEL, 0, 0);
    GUIGroupSetSelected(optgo, 0, 0);

    optdlg = GUIGroupDialogCreate(_(SDGO_TITLE), optgo, process_optdlg_event);
    if (optdlg == NULL) {
        GUIGroupDestroy(optgo);
        return 0;
    }

    return 1;
}

static void optDlgDestroy(void)
{
    if (optdlg) {
        GUIDialogDestroy(optdlg);
        optdlg = NULL;
    }
    if (optgo) {
        GUIGroupDestroy(optgo);
        optgo = NULL;
    }
}

static int optDlgRun(void)
{
    if (!optdlg) return -1; 

    gdp2 = gdp;

    setWhoBeginColor(playercolor[gdp2.whobegin], 0);

    setLight(gdp2.fastend, 3, 0);
    setLight(gdp2.fastrobotend, 5, 0);
    setLight(gdp2.superfastend, 7, 0);
    setLight(gdp2.manualdice, 9, 0);

    optdlg->result = -1;
    GUIDialogRun(optdlg);
    return optdlg->result;
}

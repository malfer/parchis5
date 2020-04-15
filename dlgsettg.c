/*
 * dlgsettg.c --- settings dialog
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

#define COMMAND_OK         101
#define COMMAND_CANCEL     102
#define COMMAND_ADVANCED   103

static GUIDialog *settgdlg = NULL;
static GUIGroup *settggo = NULL;

static int resolw[5] = {640, 800, 1024, 1150, 4000};
static int resolh[5] = {480, 600,  768,  950, 4000};

static int nboard, gwidth, gheight, resolind, lang;
static int maxrsz, testopt;

static int asetDlgCreate(void);
static void asetDlgDestroy(void);
static int asetDlgRun(void);

/***********************/

int process_settgdlg_event(void *udata, GrEvent *ev)
{
    GUIDialog *d = (GUIDialog *)udata;
    GUIGroup *go = (GUIGroup *)(d->exdata);

    if (ev->type == GREV_COMMAND) {
        switch (ev->p1) {
            case COMMAND_ADVANCED :
                asetDlgCreate();
                asetDlgRun();
                break;
            case COMMAND_OK :
                d->result = 1;
                return -1;
            case COMMAND_CANCEL :
                d->result = -1;
                return -1;
        }
    }

    if (ev->type == GREV_FCHANGE) {
        if (ev->p1 == 1) {
            gwidth = resolw[ev->p2];
            gheight = resolh[ev->p2];
        } else if (ev->p1 == 3) {
            lang = ev->p2;
        } else if (ev->p1 == 5) {
            nboard = ev->p2;
        }
    }

    return GUIGroupProcessEvent(go, ev);
}

static int settingsDlgCreate(void)
{
    static char *listresol[5];
    static char *listboard[3];
    static char *listlang[NUM_LANGUAGES];
    int i;

    if (settgdlg) return 1; // dialog already created

    for(i=0; i<NUM_LANGUAGES; i++) {
        listlang[i] = (char *)GrI18nGetLabel(i);
    }

    listresol[0] = _(SDST_RES1);
    listresol[1] = _(SDST_RES2);
    listresol[2] = _(SDST_RES3);
    listresol[3] = _(SDST_RES4);
    listresol[4] = _(SDST_RES5);
    listboard[0] = _(SDST_TBL1);
    listboard[1] = _(SDST_TBL2);
    listboard[2] = _(SDST_TBL3);

    settggo = GUIGroupCreate(9, 0, 0);
    if (settggo == NULL) return 0;

    GUIObjectSetLabel(&(settggo->o[0]),  0,   0,   0, 180, 40, GrNOCOLOR, WEBC_BLACK, _(SDST_GOGDIM));
    GUIObjectSetList(&(settggo->o[1]),   1, 180,   5, 180, 30, WEBC_WHITE, WEBC_BLACK, (void **)listresol, 5, 5, resolind);
    GUIObjectSetLabel(&(settggo->o[2]),  2,   0,  40, 180, 40, GrNOCOLOR, WEBC_BLACK, _(SDST_GOLANG));
    GUIObjectSetList(&(settggo->o[3]),   3, 180,  45, 180, 30, WEBC_WHITE, WEBC_BLACK, (void **)listlang, NUM_LANGUAGES, 10, lang);
    GUIObjectSetLabel(&(settggo->o[4]),  4,   0,  80, 180, 40, GrNOCOLOR, WEBC_BLACK, _(SDST_GOTBL));
    GUIObjectSetList(&(settggo->o[5]),   5, 180,  85, 180, 30, WEBC_WHITE, WEBC_BLACK, (void **)listboard, 3, 3, nboard);
    GUIObjectSetButton(&(settggo->o[6]), 6,   0, 230, 100, 40, WEBC_TAN, WEBC_BLACK, _(SGN_OK), COMMAND_OK, 0, 0);
    GUIObjectSetButton(&(settggo->o[7]), 7, 130, 230, 100, 40, WEBC_TAN, WEBC_BLACK, _(SDST_ADV), COMMAND_ADVANCED, 0, 0);
    GUIObjectSetButton(&(settggo->o[8]), 8, 260, 230, 100, 40, WEBC_TAN, WEBC_BLACK, _(SGN_CANCEL), COMMAND_CANCEL, 0, 0);
    GUIGroupSetSelected(settggo, 1, 0);

    settgdlg = GUIGroupDialogCreate(_(SDST_TITLE),
                                    settggo, process_settgdlg_event);
    if (settgdlg == NULL) {
        GUIGroupDestroy(settggo);
        settggo = NULL;
        return 0;
    }

    return 1;
}

static void settingsDlgDestroy(void)
{
    asetDlgDestroy();

    if (settgdlg) {
        GUIDialogDestroy(settgdlg);
        settgdlg = NULL;
    }
    if (settggo) {
        GUIGroupDestroy(settggo);
        settggo = NULL;
    }
}

int SettingsDlgRun(void)
{
    int result, i;

    lang = globcfg.lang;
    nboard = globcfg.nboard;
    gwidth = globcfg.gwidth;
    gheight = globcfg.gheight;
    maxrsz = globcfg.maxrsz;
    testopt = globcfg.testopt;

    resolind = 0;
    for (i=1; i<5; i++) {
        if (gwidth >= resolw[i]) resolind = i;
    }

    settingsDlgCreate();
    if (!settgdlg) return -1; 

    settgdlg->result = -1;
    GUIDialogRun(settgdlg);
    result = settgdlg->result;

    if (result == 1) {
        globcfg.nboard = nboard;
        globcfg.gwidth = gwidth;
        globcfg.gheight = gheight;
        globcfg.lang = lang;
        globcfg.maxrsz = maxrsz;
        globcfg.testopt = testopt;
    }

    settingsDlgDestroy();
    return result;
}

/***********************/

#define COMOPT_RSZ          1
#define COMOPT_TEST         2

#define COMOPT_OK         101
#define COMOPT_CANCEL     102

static GUIDialog *asetdlg = NULL;
static GUIGroup *asetgo = NULL;
static int tmp_maxrsz, tmp_testopt;

static void setLight(int on, int ind, int paint)
{
    char *s;

    s = on ? _(SGN_ON) : _(SGN_OFF);
    GUIGroupSetText(asetgo, ind, s);

    if (GUIGroupGetOn(asetgo, ind)) GUIGroupSetOn(asetgo, ind, 0);
    if (on) GUIGroupSetOn(asetgo, ind, 0);
    if (paint) GUIGroupRePaintObject(asetgo, ind);
}

static int process_asetdlg_event(void *udata, GrEvent *ev)
{
    GUIDialog *d = (GUIDialog *)udata;
    GUIGroup *go = (GUIGroup *)(d->exdata);

    if (ev->type == GREV_COMMAND) {
        switch (ev->p1) {
            case COMOPT_RSZ :
                tmp_maxrsz = ! tmp_maxrsz;
                setLight(tmp_maxrsz, 1, 1);
                return 1;
            case COMOPT_TEST :
                tmp_testopt = ! tmp_testopt;
                setLight(tmp_testopt, 3, 1);
                return 1;
            case COMOPT_OK :
                maxrsz = tmp_maxrsz;
                testopt = tmp_testopt;
                d->result = 1;
                return -1;
            case COMOPT_CANCEL :
                d->result = -1;
                return -1;
        }
    }

    return GUIGroupProcessEvent(go, ev);
}

static int asetDlgCreate(void)
{
    if (asetdlg) return 1; // dialog already created

    asetgo = GUIGroupCreate(6, 0, 0);
    if (asetgo == NULL) return 0;
    GUIObjectSetButton(&(asetgo->o[0]), 0,   0,  10,270,  40, WEBC_GREEN, WEBC_YELLOW, _(SDAST_MAXRSZ), COMOPT_RSZ, 0, 0);
    GUIObjectSetLight( &(asetgo->o[1]), 1, 280,  15, 80,  30, WEBC_ORANGE, WEBC_YELLOW, _(SGN_OFF), 0);
    GUIObjectSetButton(&(asetgo->o[2]), 2,   0,  50,270,  40, WEBC_GREEN, WEBC_YELLOW, _(SDAST_TOPT), COMOPT_TEST, 0, 0);
    GUIObjectSetLight( &(asetgo->o[3]), 3, 280,  55, 80,  30, WEBC_ORANGE, WEBC_YELLOW, _(SGN_OFF), 0);

    GUIObjectSetButton(&(asetgo->o[4]), 4,   0, 230,100, 40, WEBC_TAN, WEBC_BLACK, _(SGN_OK), COMOPT_OK, 0, 0);
    GUIObjectSetButton(&(asetgo->o[5]), 5, 260, 230,100, 40, WEBC_TAN, WEBC_BLACK, _(SGN_CANCEL), COMOPT_CANCEL, 0, 0);
    GUIGroupSetSelected(asetgo, 0, 0);

    asetdlg = GUIGroupDialogCreate(_(SDAST_TITLE), asetgo, process_asetdlg_event);
    if (asetdlg == NULL) {
        GUIGroupDestroy(asetgo);
        return 0;
    }

    return 1;
}

static void asetDlgDestroy(void)
{
    if (asetdlg) {
        GUIDialogDestroy(asetdlg);
        asetdlg = NULL;
    }
    if (asetgo) {
        GUIGroupDestroy(asetgo);
        asetgo = NULL;
    }
}

static int asetDlgRun(void)
{
    if (!asetdlg) return -1; 

    tmp_maxrsz = maxrsz;
    tmp_testopt = testopt;

    setLight(tmp_maxrsz, 1, 0);
    setLight(tmp_testopt, 3, 0);

    asetdlg->result = -1;
    GUIDialogRun(asetdlg);
    return asetdlg->result;
}

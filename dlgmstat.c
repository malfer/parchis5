/*
 * dlgmstat.c --- game statistics dialog
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

#define COMMAND_OK 101

static GUIDialog *gmstatdlg = NULL;
static GUIGroup *gmstatgo = NULL;
static char info_data[4][81];
static char *info[4] = {info_data[0],info_data[1],info_data[2],info_data[3]};

int process_gmstatdlg_event(void *udata, GrEvent *ev)
{
    GUIDialog *d = (GUIDialog *)udata;
    GUIGroup *go = (GUIGroup *)(d->exdata);

    if (ev->type == GREV_COMMAND && ev->p1 == COMMAND_OK) {
        return -1;
    }

    return GUIGroupProcessEvent(go, ev);
}

int GameStatDlgCreate(void)
{
    if (gmstatdlg) return 1; // dialog already created

    gmstatgo = GUIGroupCreate(6, 0, 0);
    if (gmstatgo == NULL) return 0;
    GUIObjectSetLabel( &(gmstatgo->o[0]), 0,   0,   0, 500, 22, WEBC_WHITE, WEBC_BLACK, _(SDES_CAB));
    GUIObjectSetLabel( &(gmstatgo->o[1]), 1,   0,  22, 500, 22, playercolor[0], WEBC_WHITE, "");
    GUIObjectSetLabel( &(gmstatgo->o[2]), 2,   0,  44, 500, 22, playercolor[1], WEBC_WHITE, "");
    GUIObjectSetLabel( &(gmstatgo->o[3]), 3,   0,  66, 500, 22, playercolor[2], WEBC_WHITE, "");
    GUIObjectSetLabel( &(gmstatgo->o[4]), 4,   0,  88, 500, 22, playercolor[3], WEBC_WHITE, "");
    GUIObjectSetButton(&(gmstatgo->o[5]), 5, 200, 115, 100, 40, WEBC_TAN, WEBC_BLACK, _(SGN_OK), COMMAND_OK, 0, 0);
    GUIGroupSetSelected(gmstatgo, 1, 0);

    gmstatdlg = GUIGroupDialogCreate(_(SDES_TITLE), gmstatgo, process_gmstatdlg_event);
    if (gmstatdlg == NULL) {
        GUIGroupDestroy(gmstatgo);
        gmstatgo = NULL;
        return 0;
    }

    return 1;
}

void GameStatDlgDestroy(void)
{
    if (gmstatdlg) {
        GUIDialogDestroy(gmstatdlg);
        gmstatdlg = NULL;
    }
    if (gmstatgo) {
        GUIGroupDestroy(gmstatgo);
        gmstatgo = NULL;
    }
}

void GameStatDlgSetData(PosPartida *pp)
{
    int c, i;
    int pctj[6], adj;

    if (!gmstatdlg) return;

    for (c=AMAR; c<=VERD; c++) {
        for (i=0; i<6; i++) {
            if (pp->stats_totdice[c] == 0) {
                pctj[i] = 0;
            } else {
                adj = pp->stats_totdice[c] / 2;
                pctj[i] = (pp->stats_dice[c][i] * 100 + adj) / pp->stats_totdice[c];
            }
        }
        sprintf(info[c]," %3d%% %3d%% %3d%% %3d%% %3d%% %3d%%  "
                "%6d %6d %6d %6d", pctj[0],
                pctj[1], pctj[2], pctj[3],
                pctj[4], pctj[5], pp->stats_pass[c],
                pp->stats_eat[c], pp->stats_eaten[c], pp->stats_gocasa[c]);
        GUIGroupSetText(gmstatgo, c+1, info[c]);
    }
}

int GameStatDlgRun(void)
{
    if (!gmstatdlg) return -1; 

    gmstatdlg->result = -1;
    GUIDialogRun(gmstatdlg);
    return gmstatdlg->result;
}

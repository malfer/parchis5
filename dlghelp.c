/*
 * dlghelp.c --- help dialog
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

#define MAX_LINES 300

#define COMMAND_OK 101

#define HFILE_RI_ENG "pfiles/reglieng.txt"
#define HFILE_RP_ENG "pfiles/reglpeng.txt"
#define HFILE_GI_ENG "pfiles/ginsteng.txt"
#define HFILE_RI_ESP "pfiles/regliesp.txt"
#define HFILE_RP_ESP "pfiles/reglpesp.txt"
#define HFILE_GI_ESP "pfiles/ginstesp.txt"

static GUIDialog *helpdlg = NULL;
static GUIGroup *helpgo = NULL;
static char *helptext[MAX_LINES];
static int numlines = 0;

static int process_helpdlg_event(void *udata, GrEvent *ev)
{
    GUIDialog *d = (GUIDialog *)udata;
    GUIGroup *go = (GUIGroup *)(d->exdata);

    if (ev->type == GREV_COMMAND && ev->p1 == COMMAND_OK) {
        return -1;
    }

    return GUIGroupProcessEvent(go, ev);
}

static int load_hfile(int hfile)
{
    char *fname;
    FILE *f;
    char s[101];
    int l;

    numlines = 0;

    if (globcfg.lang == 1) {
        if (hfile == HELP_SRULES)
            fname = HFILE_RI_ESP;
        else if (hfile == HELP_PRULES)
            fname = HFILE_RP_ESP;
        else if (hfile == HELP_GINSTR)
            fname = HFILE_GI_ESP;
        else
            return 0;
    } else{
        if (hfile == HELP_SRULES)
            fname = HFILE_RI_ENG;
        else if (hfile == HELP_PRULES)
            fname = HFILE_RP_ENG;
        else if (hfile == HELP_GINSTR)
            fname = HFILE_GI_ENG;
        else
            return 0;
    }

    f = fopen(fname, "rt");
    if (f == NULL) return 0;

    while (fgets(s, 100, f)) {
        l = strlen(s);
        if (l > 0 && s[l-1] == '\n') s[l-1] = '\0';
        helptext[numlines++] = strdup(s);
    }

    fclose(f);
    return 1;
}

static int HelpDlgCreate(int hfile)
{
    static char * title;
    int h;

    if (helpdlg) return 1; // dialog already created

    if (hfile == HELP_SRULES)
        title = _(SDHL_TSRUL);
    else if (hfile == HELP_PRULES)
        title = _(SDHL_TPRUL);
    else
        title = _(SDHL_TGINS);

    helpgo = GUIGroupCreate(2, 0, 0);
    if (helpgo == NULL) return 0;

    h = (globvar.tbldim > SMALLTBL) ? 406 : 306;

    GUIObjectSetDList( &(helpgo->o[0]), 0,   0,    0, 510,  h, WEBC_WHITE, WEBC_BLACK, (void **)helptext, numlines, 0);
    GUIObjectSetButton(&(helpgo->o[1]), 1, 205, h+15, 100, 40, WEBC_TAN, WEBC_BLACK, _(SGN_OK), COMMAND_OK, 0, 0);
    GUIGroupSetSelected(helpgo, 0, 0);

    helpdlg = GUIGroupDialogCreate(title, helpgo, process_helpdlg_event);
    if (helpdlg == NULL) {
        GUIGroupDestroy(helpgo);
        helpgo = NULL;
        return 0;
    }

    return 1;
}

static void HelpDlgDestroy(void)
{
    int i;

    if (helpdlg) {
        GUIDialogDestroy(helpdlg);
        helpdlg = NULL;
    }
    if (helpgo) {
        GUIGroupDestroy(helpgo);
        helpgo = NULL;
    }

    for (i=0; i<numlines; i++) {
        free(helptext[i]);
    }
    numlines = 0;
}

int HelpDlgRun(int hfile)
{
    int result;

    if (!load_hfile(hfile)) return -1;
    HelpDlgCreate(hfile);
    helpdlg->result = -1;
    GUIDialogRun(helpdlg);
    result = helpdlg->result;
    HelpDlgDestroy();
    return result;
}

/*
 * dlgglbst.c --- global statistics dialog
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
#include "addons.h"
#include "gparchis.h"
#include "i18n.h"

#define COMMAND_OK 101

static GUIDialog *gmglbstdlg = NULL;
static GUIGroup *glbstgo = NULL;

static GrTextOption grt;
static int grt_lineheight;

static int print_player(char *s, int pt, int rl)
{
    if (pt == PERSON)
        sprintf(s, "%s  ", _(SDGE_H));
    else if (pt == ROBOT)
        sprintf(s, "%s%d ", _(SDGE_R), rl);
    else
        sprintf(s, "-- ");
    return 3;
}

static int print_date(char *s, int date)
{
    int y, m, d;
    
    y = date / 10000;
    m = date / 100 - y * 100;
    d = date - m * 100 - y * 10000;
    sprintf(s,"%04d-%02d-%02d", y, m, d);
    return 10;
}

static void GlbStatPaintReg(int x, int y, int ind, int selected, GrColor bg, GrColor fg)
{
    RHistPart *r;
    char s[81], *p;
    int nc, c, j, adj, pctj;

    r = HistPartGetReg(ind);
    if (r == NULL) return;

    grt.txo_fgcolor = (selected) ? fg : bg;
    grt.txo_bgcolor = (selected) ? bg : fg;
    p = s;
    nc = sprintf(p, "%s ", (r->gametype == GT_SINGLE) ? _(SDGE_SING) : _(SDGE_PAIR));
    p += nc;
    for (c=AMAR; c<=VERD; c++) {
        p += print_player(p, r->playertype[c], r->robotlevel[c]);
    }
    GrDrawString(s, 0, x, y, &grt);
    
    sprintf(s, "%d %s", r->totgames, _(SDGE_GPLY));
    GrDrawString(s, 0, x, y+grt_lineheight, &grt);

    p = s;
    p += print_date(p, r->lastdate);
    sprintf(p, " %s", _(SDGE_LAST));
    GrDrawString(s, 0, x, y+grt_lineheight*2, &grt);

    p = s;
    sprintf(p, "%s", (r->manualdice ? _(SDGE_MDICE) : _(SDGE_ADICE)));
    GrDrawString(s, 0, x, y+grt_lineheight*3, &grt);

    if (r->gametype == GT_SINGLE) {
        sprintf(s, "          1º       2º       3º       4º");
        GrDrawString(s, 0, x+180, y, &grt);
        grt.txo_fgcolor = GrWhite();
        for (c=AMAR; c<=VERD; c++) {
            grt.txo_bgcolor = playercolor[c];
            p = s;
            p += sprintf(p, " ");
            p += print_player(p, r->playertype[c], r->robotlevel[c]);
            if (r->playertype[c] != NOPLAY) {
                for (j=0; j<4; j++) {
                    adj = r->totgames / 2;
                    pctj = (r->stats[c][j] * 100 + adj) / r->totgames;
                    if (pctj < 100)
                        p += sprintf(p, "%4d(%2d%%)", r->stats[c][j], pctj);
                    else
                        p += sprintf(p, "%3d(%3d%%)", r->stats[c][j], pctj);
                }
                p += sprintf(p, " ");
            } else {
                p += sprintf(p, "%37s", " ");
            }
            GrDrawString(s, 0, x+180, y+(c+1)*grt_lineheight, &grt);
        }
        GrHLine(x, x+540, y+grt_lineheight*5+4, ((selected) ? fg : bg));
    } else {
        sprintf(s, "              1º                2º");
        GrDrawString(s, 0, x+180, y, &grt);
        grt.txo_fgcolor = GrWhite();
        for (c=AMAR; c<=VERD; c++) {
            grt.txo_bgcolor = playercolor[c];
            p = s;
            p += sprintf(p, " ");
            p += print_player(p, r->playertype[c], r->robotlevel[c]);
            if (r->playertype[c] != NOPLAY) {
                for (j=0; j<4; j+=2) {
                    adj = r->totgames / 2;
                    pctj = ((r->stats[c][j] + r->stats[c][j+1]) * 100 + adj) /
                            r->totgames;
                    if (pctj < 100)
                        p += sprintf(p, "    %4d(%2d%%)     ",
                                     r->stats[c][j]+r->stats[c][j+1], pctj);
                    else
                        p += sprintf(p, "    %3d(%3d%%)     ",
                                     r->stats[c][j]+r->stats[c][j+1], pctj);
                }
                p += sprintf(p, " ");
            } else {
                p += sprintf(p, "%37s", " ");
            }
            GrDrawString(s, 0, x+180, y+(c+1)*grt_lineheight, &grt);
        }
        GrHLine(x, x+540, y+grt_lineheight*5+4, ((selected) ? fg : bg));
    }
}

static int process_gmglbstdlg_event(void *udata, GrEvent *ev)
{
    GUIDialog *d = (GUIDialog *)udata;
    GUIGroup *go = (GUIGroup *)(d->exdata);

    if (ev->type == GREV_COMMAND && ev->p1 == COMMAND_OK) {
        return -1;
    }

    return GUIGroupProcessEvent(go, ev);
}

static int GlbStatDlgCreate(void)
{
    int nreg, regheight, h;

    if (gmglbstdlg) return 1; // dialog already created

    nreg = HistPartGetNReg();
    grt.txo_font = &GrFont_PX8x18;
    grt.txo_fgcolor = GrWhite();
    grt.txo_bgcolor = GrNOCOLOR;
    grt.txo_chrtype = GR_UTF8_TEXT;
    grt.txo_direct = GR_TEXT_RIGHT;
    grt.txo_xalign = GR_ALIGN_LEFT;
    grt.txo_yalign = GR_ALIGN_TOP;
    grt_lineheight = grt.txo_font->h.height;
    regheight = grt.txo_font->h.height * 5 + 6;

    glbstgo = GUIGroupCreate(2, 0, 0);
    if (glbstgo == NULL) return 0;

    h = (globvar.tbldim > SMALLTBL) ? 390 : 300;

    GUIObjectSetRegList(&(glbstgo->o[0]), 0,   0,   0, 540, h, WEBC_WHITE, WEBC_BLACK, nreg, regheight, 0, GlbStatPaintReg);
    GUIObjectSetButton(&(glbstgo->o[1]),  1, 220, h+15, 100, 40, WEBC_TAN, WEBC_BLACK, _(SGN_OK), COMMAND_OK, 0, 0);
    GUIGroupSetSelected(glbstgo, 0, 0);

    gmglbstdlg = GUIGroupDialogCreate(_(SDGE_TITLE),
                                    glbstgo, process_gmglbstdlg_event);
    if (gmglbstdlg == NULL) {
        GUIGroupDestroy(glbstgo);
        glbstgo = NULL;
        return 0;
    }

    return 1;
}

int GlbStatDlgRun(void)
{
    int result;

    if (!GlbStatDlgCreate()) return -1;

    gmglbstdlg->result = -1;
    GUIDialogRun(gmglbstdlg);
    result = gmglbstdlg->result;

    if (gmglbstdlg) {
        GUIDialogDestroy(gmglbstdlg);
        gmglbstdlg = NULL;
    }
    if (glbstgo) {
        GUIGroupDestroy(glbstgo);
        glbstgo = NULL;
    }

    return result;
}

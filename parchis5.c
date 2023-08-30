/*
 * parchis5.c --- parchis5 main program
 * 
 * Copyright (c) 2020,2023 Mariano Alvarez Fernandez
 * (malfer@telefonica.net)
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
#include <time.h>
#include <stdio.h>
#include <string.h>
#include <math.h>
#include <grgui.h>
#include <mgrxcolr.h>
#include "i18n.h"
#include "engine.h"
#include "addons.h"
#include "gparchis.h"

#define COMMAND_MENU         1
#define COMMAND_A            2
#define COMMAND_B            3
#define COMMAND_MD           4
#define COMMAND_NEWGAME     21
#define COMMAND_REPEATGAME  22
#define COMMAND_GAMESTATS   31
#define COMMAND_SPEED1      41
#define COMMAND_SPEED2      42
#define COMMAND_SPEED3      43
#define COMMAND_SPEED4      44
#define COMMAND_SPEED5      45
#define COMMAND_SETTINGS    51
#define COMMAND_GLBSTATS    61
#define COMMAND_HELPSRULES  71
#define COMMAND_HELPPRULES  72
#define COMMAND_HELPGAME    73
#define COMMAND_ABOUT       74

#define TEST_FICHAS        201
#define TEST_GIRAIZQ       202
#define TEST_GIRADER       203
#define TEST_SAVEGAME      204
#define TEST_LOADGAME      205
#define TEST_AUTOREPEAT    206
#define TEST_PRINTANAL     207
#define TEST_PRINTMOVL     208

#define COMMAND_EXIT       500

#define ID_MAINMENU          1
#define ID_SPEEDMENU         2
#define ID_HELPMENU          3
#define ID_TESTMENU          4

#define CFG_FILE "parchis.cfg"
#define SAVE_FILE "parchis5.sav"
#define RESTART_FILE "parchis5.res"
#define HISTP_FILE "parchis.hpt"

#define PI 3.14159265359

GrContext *globctx;
GrColor TRANSPARENT;
GrColor playercolor[4];
GrColor playerlcolor[4];
GrColor cintcolor[5];

GlobVars globvar;
GlobCfg globcfg;
GStatus globgstatus;

int globonpause = 0;
int globonmoviola = 0;
Partida globpt;
Partida globptsaved;
MovPartida globmovpt = {0, 0, 0, NULL};

static int gwidth = 1150;
static int gheight = 950;
static int gbpp = 24;

static GUIGroup *globgrp;
static int req_speed; // required speed to be set after menus setup
static int cfgfile_exist = 0;

/***********************/

void setup_i18n(void);
void mgrx_setup(int argc, char **argv);
void grgui_setup(void);
void setup_globvar(void);
void setup_menus(void);
void setup_globgrp(void);
void setup_gparchis(void);
void ini_globgstatus(void);
void set_speed(int speed);
void check_testoptions(void);
void clean_up(void);
void settings_changed(GlobCfg *savedcfg);
void window_resized(int width, int height);
void paint_main_screen(void);
void show_manual_dice(int show);
int check_saved_game(char *fname, int ask);
int process_board_event(GrEvent *ev);
void set_buttons_text(char *tb1, char *tb2);
void set_instructions(void);
void do_next_step(void);
void play_partida_command(int command, int id);
void play_partida_auto(void);
void play_partida_moviola(void);
void dlg_about(void);
int get_diceval(void);
void obscure(void);
void desobscure(void);
void load_globcfg(char *fname);
void save_globcfg(char *fname);
int save_game(char *fname);
int load_game(char *fname);

/***********************/

int main(int argc, char **argv)
{
    static char *infoexit[1];

    GlobCfg savedcfg;
    DefPartida dpaux;
    GrEvent ev, ev2;
    int ret;

    load_globcfg(CFG_FILE);
    setup_i18n();
    HistPartInit(HISTP_FILE);

    GrSetDriverExt(NULL, "rszwin"); // ask for window resize support

    mgrx_setup(argc, argv);
    grgui_setup();
    setup_globvar();
    setup_menus();
    setup_globgrp();
    load_images();
    inicia_globgpos();
    PTInit(&globpt, &(globcfg.defaultdp));
    MPClear(&globmovpt);
    setup_gparchis();
    ini_globgstatus();

    GrSetContext(globctx);
    paint_main_screen();

    if (!cfgfile_exist) { // only one command can be queued before the main
                          // loop, so we repeat the command_settings code here
        savedcfg = globcfg;
        obscure();
        ret = SettingsDlgRun();
        desobscure();
        if (ret == 1) {
            settings_changed(&savedcfg);
        }
    }

    if (!check_saved_game(SAVE_FILE, 1))
        GrEventParEnqueue(GREV_COMMAND, COMMAND_NEWGAME, 0, 0, 0);

    while(1) {
        GrEventRead(&ev);

        if (((ev.type == GREV_KEY) && (ev.p1 == GrKey_Escape)) ||
            ((ev.type == GREV_COMMAND) && (ev.p1 == COMMAND_EXIT)) ||
            (ev.type == GREV_WMEND)) {
            if (PPHasBegun(&(globpt.pp)) && !PPHasFinished(&(globpt.pp))) {
                obscure();
                infoexit[0] = _(SDEX_MESSAGE);
                ret = GUICDialogYesNoCancel(_(SDEX_TITLE), (void **)infoexit, 1,
                                            _(SGN_YES), _(SGN_NO), _(SGN_CANCEL));
                desobscure();
                if (ret == 1) save_game(SAVE_FILE);
                if (ret < 0) continue;
            }
            break;
        }

        if (ev.type == GREV_COMMAND) {
            if (ev.p1 == COMMAND_MENU) {
                GUIMenuRun(ID_MAINMENU, globvar.xmenu, globvar.ymenu, 0);
            } else if (ev.p1 == COMMAND_SETTINGS) {
                savedcfg = globcfg;
                obscure();
                ret = SettingsDlgRun();
                desobscure();
                if (ret == 1) {
                    settings_changed(&savedcfg);
                }
            } else if (ev.p1 == COMMAND_NEWGAME) {
                obscure();
                NewGameDlgCreate();
                NewGameDlgSetData(&(globpt.pp.dp));
                ret = NewGameDlgRun();
                desobscure();
                if (ret == 1) {
                    NewGameDlgGetData(&(globcfg.defaultdp));
                    AnimationsStopAll();
                    PTInit(&globpt, &(globcfg.defaultdp));
                    MPClear(&globmovpt);
                    genera_munecos(&(globcfg.defaultdp));
                    ini_globgstatus();
                    globonmoviola = 0;
                    globonpause = 0;
                    paint_main_screen();
                }
            } else if (ev.p1 == COMMAND_REPEATGAME) {
                AnimationsStopAll();
                dpaux = globpt.pp.dp;
                PTInit(&globpt, &dpaux);
                MPClear(&globmovpt);
                //genera_munecos(&dpaux);
                ini_globgstatus();
                globonmoviola = 0;
                globonpause = 0;
                paint_main_screen();
            } else if (ev.p1 == COMMAND_GAMESTATS) {
                obscure();
                GameStatDlgCreate();
                GameStatDlgSetData(&(globpt.pp));
                GameStatDlgRun();
                desobscure();
            } else if (ev.p1 == COMMAND_GLBSTATS) {
                obscure();
                GlbStatDlgRun();
                desobscure();
            } else if (ev.p1 == COMMAND_SPEED1) {
                set_speed(10);
            } else if (ev.p1 == COMMAND_SPEED2) {
                set_speed(7);
            } else if (ev.p1 == COMMAND_SPEED3) {
                set_speed(3);
            } else if (ev.p1 == COMMAND_SPEED4) {
                set_speed(1);
            } else if (ev.p1 == COMMAND_SPEED5) {
                set_speed(0);
            } else if (ev.p1 == COMMAND_HELPSRULES) {
                obscure();
                HelpDlgRun(HELP_SRULES);
                desobscure();
            } else if (ev.p1 == COMMAND_HELPPRULES) {
                obscure();
                HelpDlgRun(HELP_PRULES);
                desobscure();
            } else if (ev.p1 == COMMAND_HELPGAME) {
                obscure();
                HelpDlgRun(HELP_GINSTR);
                desobscure();
            } else if (ev.p1 == COMMAND_ABOUT) {
                obscure();
                dlg_about();
                desobscure();
            } else if (ev.p1 == TEST_FICHAS) {
                globcfg.testingfichas = !globcfg.testingfichas;
                GUIMenuSetTag(ID_TESTMENU, TEST_FICHAS, globcfg.testingfichas);
                paint_board();
            } else if (ev.p1 == TEST_GIRAIZQ) {
                StartRoTblAnimation(90, -1);
            } else if (ev.p1 == TEST_GIRADER) {
                //StartRoTblAnimation(90, 1);
                StartRoTblAnimation(30, 3);
            } else if (ev.p1 == TEST_SAVEGAME) {
                save_game(SAVE_FILE);
            } else if (ev.p1 == TEST_LOADGAME) {
                if (load_game(SAVE_FILE)) {
                    AnimationsStopAll();
                    genera_munecos(&(globpt.pp.dp));
                    ini_globgstatus();
                    paint_main_screen();
                }
            } else if (ev.p1 == TEST_AUTOREPEAT) {
                globcfg.autorepeat = !globcfg.autorepeat;
                GUIMenuSetTag(ID_TESTMENU, TEST_AUTOREPEAT, globcfg.autorepeat);
            } else if (ev.p1 == TEST_PRINTANAL) {
                globcfg.printanal = !globcfg.printanal;
                GUIMenuSetTag(ID_TESTMENU, TEST_PRINTANAL, globcfg.printanal);
            } else if (ev.p1 == TEST_PRINTMOVL) {
                MPPrint(&globmovpt, stdout);
            } else if (ev.p1 == COMMAND_A ||
                       ev.p1 == COMMAND_B ||
                       ev.p1 == COMMAND_MD) {
                play_partida_command(ev.p1, ev.p2);
            }
            continue;
        }

        if (ev.type == GREV_WSZCHG) {
            window_resized(ev.p3, ev.p4);
            continue;
        }

        GrSetContext(globvar.ctl);
        ev2 = ev;
        if ((ev.type == GREV_MOUSE) || (ev.type == GREV_MMOVE)) {
            ev2.p2 -= GrCurrentContext()->gc_xoffset;
            ev2.p3 -= GrCurrentContext()->gc_yoffset;
        }
        ret = GUIGroupProcessEvent(globgrp, &ev2);
        GrSetContext(globctx);
        if (ret != 0) continue;

        GrSetContext(globvar.tbl);
        ev2 = ev;
        if ((ev.type == GREV_MOUSE) || (ev.type == GREV_MMOVE)) {
            ev2.p2 -= GrCurrentContext()->gc_xoffset;
            ev2.p3 -= GrCurrentContext()->gc_yoffset;
        }
        ret = process_board_event(&ev2);
        GrSetContext(globctx);
        if (ret != 0) continue;

        if (ev.type == GREV_NULL) {
            AnimationsRun(ev.time);
            if (globonmoviola)
                play_partida_moviola();
            else
                play_partida_auto();
        }
    }

    clean_up();
    MPClean(&globmovpt);
    GUIEnd();
    GrSetMode(GR_default_text);

    HistPartSave(HISTP_FILE);
    save_globcfg(CFG_FILE);

    return 0;
}

/***********************/

void setup_i18n(void)
{
    if (!GrI18nInit(2, SID_LAST, "undef")) {
        disaster("initing GrI18n");
    }

    AddEngStrings(0);
    AddEspStrings(1);
    GrI18nSetLang(globcfg.lang);
}

/***********************/

void mgrx_setup(int argc, char **argv)
{
    if (argc >= 4) {
        gwidth = atoi(argv[1]);
        gheight = atoi(argv[2]);
        gbpp = atoi(argv[3]);
    } else {
        gwidth = globcfg.gwidth;
        gheight = globcfg.gheight;
        gbpp = 24;
    }

    srandom(time(NULL));

    GrSetMode(GR_width_height_bpp_graphics, gwidth, gheight, gbpp);
    if (GrNumColors() < 256 || !GrPngSupport()) {
        disaster("not enough colors or png not supported");
    }
    GrSetRGBcolorMode();

    GrSetUserEncoding(GRENC_UTF_8); // this source is UTF8 coded
    GrEventGenWMEnd(GR_GEN_WMEND_YES);
    GrGenWebColorTable();
    //TRANSPARENT = GrAllocColor(0, 1, 0) | GrIMAGE;
    TRANSPARENT = GrAllocColor(10, 10, 10) | GrIMAGE;
    //TRANSPARENT = GrAllocColor(250, 250, 250) | GrIMAGE;
    //GrSetFontPath("../pfiles/;./");
    GrSetWindowTitle("Parchis v5.1");
}

/***********************/

void grgui_setup(void)
{
    GUIInit(1, 1);
    //GUIDBManageExposeEvents(1);

    //GUIObjectsSetChrType(GR_UTF8_TEXT); // this source is UTF8 coded
    GUIObjectsSetColors(WEBC_KHAKI, WEBC_PERU, WEBC_SIENNA);
    GUIObjectsSetFontByName("pfiles/parch08.fnt");
    GUIDialogsSetColors(WEBC_BLACK, WEBC_ORANGE, WEBC_ANTIQUEWHITE, WEBC_MAROON);
    GUICDialogsSetColors(WEBC_TAN, WEBC_BLACK);
    GUIDialogsSetTitleFontByName("pfiles/parch11.fnt");
    GUICDialogsSetFont(&GrFont_PX8x18);

    globctx = GUIGetGlobalContext();
}

/***********************/

void disaster(char *message)
{
    GrSetMode(GR_default_text);
    printf("Disaster: %s\n", message);
    exit(1);
}

/***********************/

void setup_globvar(void)
{
#define TBLDIM 878     // this must be the TABLERO1 image dim
#define PAWNDIM 38     // this must be the Fxxxx image dim
#define MEDALDIM 90    // this must be the MEDALx image dim
#define MUNECODIM 96   // this must be the muñeco image dim
#define PODIUMDIM 268  // this must be the single podium image dim
#define PODIUMPDIM 328 // this must be the pairs podium image dim
#define MOVIOLADIM 130 // this must be moviola image dim
#define BARRERADIM 86  // this must be the barrera image long dim

#define DICEDIM 61               // this must be the DICE image dim
#define DICEBOXH (DICEDIM + 80)  // Dice box height dimension
#define SMALLDICEDIM 41          // this must be the DICE image dim
#define SMALLDICEBOXH (SMALLDICEDIM + 20)  // Dice box height dimension

#define CTLDIMW 200    // Control area width dimension
#define PGBDIMY 115    // progress bars y pos
#define PGBDIMH 110    // progress bars height dimension
#define SMALLPGBDIMH 90    // progress bars height dimension

    int y1, y2, dim;

    globvar.tbldim = ((GrSizeX() - CTLDIMW - 4) > GrSizeY()) ?
                     GrSizeY() - 8: GrSizeX() - CTLDIMW - 12;
    if (globvar.tbldim > TBLDIM) {
        if (globvar.tbldim < TBLDIM+80 || globcfg.maxrsz)
            globvar.tbldim = TBLDIM;
        else
            globvar.tbldim -= 20;
    }
    globvar.scale = (double)globvar.tbldim / TBLDIM;
    globvar.pawndim = round(PAWNDIM * globvar.scale);
    globvar.medaldim = round(MEDALDIM * globvar.scale);
    globvar.munecodim = round(MUNECODIM * globvar.scale);
    globvar.podiumdim = round(PODIUMDIM * globvar.scale);
    globvar.podiumpdim = round(PODIUMPDIM * globvar.scale);
    globvar.movioladim = round(MOVIOLADIM * globvar.scale);
    globvar.barreradim = round(BARRERADIM * globvar.scale);
    globvar.xorg = (GrSizeX() - (globvar.tbldim + CTLDIMW + 4)) / 2;
    globvar.yorg = (GrSizeY() - globvar.tbldim) / 2;
    //printf("%d\n", globvar.tbldim);
    globvar.tbl = GrCreateSubContext(globvar.xorg, globvar.yorg,
                                     globvar.xorg+globvar.tbldim-1,
                                     globvar.yorg+globvar.tbldim-1,
                                     globctx, NULL);
    globvar.tblpaint = GrCreateContext(globvar.tbldim, globvar.tbldim,
                                       NULL, NULL);
    globvar.ctl = GrCreateSubContext(globvar.xorg+globvar.tbldim+4, globvar.yorg,
                                     globvar.xorg+globvar.tbldim+CTLDIMW+3,
                                     globvar.yorg+globvar.tbldim-1,
                                     globctx, NULL);
    dim = (globvar.tbldim > SMALLTBL) ? PGBDIMH : SMALLPGBDIMH;
    globvar.pgb = GrCreateSubContext(globvar.xorg+globvar.tbldim+4,
                                     globvar.yorg+PGBDIMY,
                                     globvar.xorg+globvar.tbldim+CTLDIMW+3,
                                     globvar.yorg+PGBDIMY+dim,
                                     globctx, NULL);
    y1 = globvar.yorg + globvar.tbldim - 158;
    y2 = globvar.yorg + globvar.tbldim - 116;
    globvar.ins = GrCreateSubContext(globvar.xorg+globvar.tbldim+16, y1,
                                     globvar.xorg+globvar.tbldim+CTLDIMW-9,
                                     y2, globctx, NULL);

    y2 = y1 - 1;
    if (globvar.tbldim > SMALLTBL)
        y1 = y2 - DICEBOXH;
    else
        y1 = y2 - SMALLDICEBOXH;
    globvar.dce = GrCreateSubContext(globvar.xorg+globvar.tbldim+16, y1,
                                     globvar.xorg+globvar.tbldim+CTLDIMW-9,
                                     y2, globctx, NULL);

    //globvar.xmenu = globvar.xorg + globvar.tbldim - 40;
    //globvar.ymenu = globvar.yorg + 55;

    if (!globvar.tbl || !globvar.tblpaint || !globvar.ctl ||
        !globvar.pgb || !globvar.dce) {
        disaster("on setup_globvar");
    }
}

/***********************/

void setup_menus(void)
{
    int w, h;

    static GUIMenuItem itemsmm[12] = {
        {GUI_MI_TITLE, 0, "", 0, NULL, 0, 0, 0, SMP_TITLE},
        {GUI_MI_SEP, 1, "", 0, NULL, 0, 0, 0, -1},
        {GUI_MI_OPER, 1, "", 0, NULL, 0, COMMAND_NEWGAME, 0, SMP_NEWGAME},
        {GUI_MI_OPER, 1, "", 0, NULL, 0, COMMAND_GAMESTATS, 0, SMP_GAMESTATS},
        {GUI_MI_MENU, 1, "", 0, NULL, 0, ID_SPEEDMENU, 0, SMP_SPEED},
        {GUI_MI_OPER, 1, "", 0, NULL, 0, COMMAND_SETTINGS, 0, SMP_SETTINGS},
        {GUI_MI_OPER, 1, "", 0, NULL, 0, COMMAND_GLBSTATS, 0, SMP_GLBSTATS},
        {GUI_MI_MENU, 1, "", 0, NULL, 0, ID_HELPMENU, 0, SMP_HELP}, 
        {GUI_MI_SEP, 1, "", 0, NULL, 0, 0, 0, -1},
        {GUI_MI_MENU, 1, "", 0, NULL, 0, ID_TESTMENU, 0, SMP_TESTING}, 
        {GUI_MI_SEP, 1, "", 0, NULL, 0, 0, 0, -1},
        {GUI_MI_OPER, 1, "", 0, "Esc", 0, COMMAND_EXIT, 0, SMP_EXIT}};
    static GUIMenu mainmenu = {ID_MAINMENU, 12, 2, itemsmm, 1};

    static GUIMenuItem itemsspeed[5] = {
        {GUI_MI_OPER, 1, "", 0, NULL, 0, COMMAND_SPEED1, 0, SMV_SLOW},
        {GUI_MI_OPER, 1, "", 0, NULL, 0, COMMAND_SPEED2, 2, SMV_NORMAL},
        {GUI_MI_OPER, 1, "", 0, NULL, 0, COMMAND_SPEED3, 0, SMV_FAST},
        {GUI_MI_OPER, 1, "", 0, NULL, 0, COMMAND_SPEED4, 0, SMV_VFAST},
        {GUI_MI_OPER, 1, "", 0, NULL, 0, COMMAND_SPEED5, 0, SMV_INSTANT}};
    static GUIMenu speedmenu = {ID_SPEEDMENU, 5, 1, itemsspeed, 1};

    static GUIMenuItem itemstest[8] = {
        {GUI_MI_OPER, 1, "", 0, NULL, 0, TEST_FICHAS, 0, SMT_TPAWN},
        {GUI_MI_OPER, 1, "", 0, NULL, 0, TEST_GIRAIZQ, 0, SMT_TLEFT},
        {GUI_MI_OPER, 1, "", 0, NULL, 0, TEST_GIRADER, 0, SMT_TRIGHT},
        {GUI_MI_OPER, 1, "", 0, NULL, 0, TEST_SAVEGAME, 0, SMT_SAVEG},
        {GUI_MI_OPER, 1, "", 0, NULL, 0, TEST_LOADGAME, 0, SMT_LOADG},
        {GUI_MI_OPER, 1, "", 0, NULL, 0, TEST_AUTOREPEAT, 0, SMT_AUTOR},
        {GUI_MI_OPER, 1, "", 0, NULL, 0, TEST_PRINTANAL, 0, SMT_PANAL},
        {GUI_MI_OPER, 1, "", 0, NULL, 0, TEST_PRINTMOVL, 0, SMT_PMOVL}};
    static GUIMenu testmenu = {ID_TESTMENU, 8, 0, itemstest, 1};

    static GUIMenuItem itemshelp[4] = {
        {GUI_MI_OPER, 1, "", 0, NULL, 0, COMMAND_HELPSRULES, 0, SMH_SRULES},
        {GUI_MI_OPER, 1, "", 0, NULL, 0, COMMAND_HELPPRULES, 0, SMH_PRULES},
        {GUI_MI_OPER, 1, "", 0, NULL, 0, COMMAND_HELPGAME, 0, SMH_INSTR},
        {GUI_MI_OPER, 1, "", 0, NULL, 0, COMMAND_ABOUT, 0, SMH_ABOUT}};
    static GUIMenu helpmenu = {ID_HELPMENU, 4, 0, itemshelp, 1};

    GUIMenusSetColors(WEBC_ANTIQUEWHITE, WEBC_MAROON, WEBC_MAROON,
                      WEBC_ANTIQUEWHITE, WEBC_LIGHTGRAY, WEBC_DARKGRAY);
    GUIMenusSetColors2(WEBC_SIENNA, WEBC_WHITE, WEBC_ORANGE);

    if (globvar.tbldim > SMALLTBL)
        GUIMenusSetFontByName("pfiles/parch11.fnt");
    else
        GUIMenusSetFontByName("pfiles/parch08.fnt");

    GUIMenuRegister(&mainmenu);
    GUIMenuRegister(&speedmenu);
    GUIMenuRegister(&helpmenu);
    GUIMenuRegister(&testmenu);

    GUIMenusSetI18nFields();

    set_speed(req_speed);
    check_testoptions();

    GUIMenuGetDims(ID_MAINMENU, &w, &h);
    //globvar.xmenu = (GrSizeX() - w) / 2;
    //globvar.ymenu = (GrSizeY() - h) / 2;
    globvar.xmenu = globvar.xorg + globvar.tbldim + CTLDIMW + 4 - 8 - w;
    globvar.ymenu = globvar.yorg + 8;
}

/***********************/

void setup_globgrp(void)
{
    int y2;
    
    y2 = globvar.tbldim - 1;
    globgrp = GUIGroupCreate(9, 0, 0);
    GUIObjectSetButton(&(globgrp->o[0]), 0, 10,     10, 180, 45, WEBC_GREEN, WEBC_YELLOW, _(SMAIN_MENU), COMMAND_MENU, 0, 0);
    GUIObjectSetButton(&(globgrp->o[1]), 1, 10, y2-105, 180, 45, WEBC_GREEN, WEBC_YELLOW, "A", COMMAND_A, 0, 0);
    GUIObjectSetButton(&(globgrp->o[2]), 2, 10,  y2-55, 180, 45, WEBC_GREEN, WEBC_YELLOW, "B", COMMAND_B, 0, 0);
    GUIObjectSetButton(&(globgrp->o[3]), 3, 10, y2-105,  60, 45, WEBC_GREEN, WEBC_YELLOW, "\x0f\x10", COMMAND_MD, 0, 0);
    GUIObjectSetButton(&(globgrp->o[4]), 4, 70, y2-105,  60, 45, WEBC_GREEN, WEBC_YELLOW, "\x11\x12", COMMAND_MD, 0, 0);
    GUIObjectSetButton(&(globgrp->o[5]), 5,130, y2-105,  60, 45, WEBC_GREEN, WEBC_YELLOW, "\x13\x14", COMMAND_MD, 0, 0);
    GUIObjectSetButton(&(globgrp->o[6]), 6, 10,  y2-55,  60, 45, WEBC_GREEN, WEBC_YELLOW, "\x15\x16", COMMAND_MD, 0, 0);
    GUIObjectSetButton(&(globgrp->o[7]), 7, 70,  y2-55,  60, 45, WEBC_GREEN, WEBC_YELLOW, "\x17\x18", COMMAND_MD, 0, 0);
    GUIObjectSetButton(&(globgrp->o[8]), 8,130,  y2-55,  60, 45, WEBC_GREEN, WEBC_YELLOW, "\x19\x1a", COMMAND_MD, 0, 0);
    GUIGroupSetSelected(globgrp, 1, 0);
    GUIGroupSetVisible(globgrp, 3, 0);
    GUIGroupSetVisible(globgrp, 4, 0);
    GUIGroupSetVisible(globgrp, 5, 0);
    GUIGroupSetVisible(globgrp, 6, 0);
    GUIGroupSetVisible(globgrp, 7, 0);
    GUIGroupSetVisible(globgrp, 8, 0);
}

/***********************/

void setup_gparchis(void)
{
    playercolor[0] = GrAllocColor(0xa9, 0xa9, 0x38);
    playercolor[1] = GrAllocColor(0x00, 0x00, 0xaa);
    playercolor[2] = GrAllocColor(0xaa, 0x00, 0x00);
    playercolor[3] = GrAllocColor(0x00, 0x75, 0x00);
    playerlcolor[0] = GrAllocColor(0xff, 0xff, 0x55);
    playerlcolor[1] = GrAllocColor(0x55, 0x55, 0xff);
    playerlcolor[2] = GrAllocColor(0xff, 0x55, 0x55);
    playerlcolor[3] = GrAllocColor(0x00, 0xaa, 0x00);
    cintcolor[0] = WEBC_WHITE;
    cintcolor[1] = WEBC_YELLOW;
    cintcolor[2] = WEBC_ORANGE;
    //cintcolor[3] = GrAllocColor(0x00, 0xd9, 0x00);
    cintcolor[3] = WEBC_LIMEGREEN;
    cintcolor[4] = WEBC_DODGERBLUE;

    genera_munecos(&(globpt.pp.dp));
}

/***********************/

void ini_globgstatus(void)
{
    globgstatus.workingdice = 0;
    globgstatus.robotpassing = 0;
    globgstatus.robotpawnshow = 0;
    globgstatus.blinkpawn = 0;
    globgstatus.oncarrera = 0;
    globgstatus.rotating = 0;
    globgstatus.mousingpawn = 0;
    globgstatus.pawnoverdest = 0;
    globgstatus.pawnfingermoved = 0;
    globgstatus.handrotating = 0;
    if (globgstatus.waitmanualdice)
        show_manual_dice(0);
}

/***********************/

void set_speed(int speed)
{
    if (speed == 10) {
        globcfg.speed = 10;
        GUIMenuSetUniqueTag(ID_SPEEDMENU, COMMAND_SPEED1, 2);
    } else if (speed == 7) {
        globcfg.speed = 7;
        GUIMenuSetUniqueTag(ID_SPEEDMENU, COMMAND_SPEED2, 2);
    } else if (speed == 3) {
        globcfg.speed = 3;
        GUIMenuSetUniqueTag(ID_SPEEDMENU, COMMAND_SPEED3, 2);
    } else if (speed == 1) {
        globcfg.speed = 1;
        GUIMenuSetUniqueTag(ID_SPEEDMENU, COMMAND_SPEED4, 2);
    } else if (speed == 0) {
        globcfg.speed = 0;
        GUIMenuSetUniqueTag(ID_SPEEDMENU, COMMAND_SPEED5, 2);
    }
}

/***********************/

void check_testoptions(void)
{
    if (globcfg.testopt) {
        GUIMenuSetEnable(ID_SPEEDMENU, GUI_MI_OPER, COMMAND_SPEED5, 1);
        GUIMenuSetEnable(ID_MAINMENU, GUI_MI_MENU, ID_TESTMENU, 1);
    } else {
        GUIMenuSetEnable(ID_SPEEDMENU, GUI_MI_OPER, COMMAND_SPEED5, 0);
        GUIMenuSetEnable(ID_MAINMENU, GUI_MI_MENU, ID_TESTMENU, 0);
        if (globcfg.speed == 0) set_speed(1);
        if (globcfg.testingfichas) {
            globcfg.testingfichas = 0;
            GUIMenuSetTag(ID_TESTMENU, TEST_FICHAS, globcfg.testingfichas);
            paint_board();
        }
        globcfg.autorepeat = 0;
        GUIMenuSetTag(ID_TESTMENU, TEST_AUTOREPEAT, globcfg.autorepeat);
        globcfg.printanal = 0;
        GUIMenuSetTag(ID_TESTMENU, TEST_PRINTANAL, globcfg.printanal);
    }
}

/***********************/

void clean_up(void)
{
    GrDestroyContext(globvar.tbl);
    GrDestroyContext(globvar.tblpaint);
    GrDestroyContext(globvar.ctl);
    GrDestroyContext(globvar.pgb);
    GrDestroyContext(globvar.ins);
    GrDestroyContext(globvar.dce);

    GUIGroupDestroy(globgrp);

    NewGameDlgDestroy();
    GameStatDlgDestroy();
}

/***********************/

void settings_changed(GlobCfg *savedcfg)
{
    int paint = 0;

    if (savedcfg->gwidth != globcfg.gwidth ||
        savedcfg->gheight != globcfg.gheight ||
        (globcfg.gwidth > 1150 &&
         savedcfg->maxrsz != globcfg.maxrsz)) {
        clean_up();
        GUIEnd();
        GrSetMode(GR_default_text);
        GrI18nSetLang(globcfg.lang);
        mgrx_setup(0, NULL);
        grgui_setup();
        setup_globvar();
        req_speed = globcfg.speed;
        setup_menus();
        setup_globgrp();
        load_images();
        inicia_globgpos();
        setup_gparchis();
        GrSetContext(globctx);
        paint_main_screen();
        return;
    }

    if (savedcfg->lang != globcfg.lang) {
        clean_up();
        GUIEnd();
        GrI18nSetLang(globcfg.lang);
        grgui_setup();
        setup_globvar();
        req_speed = globcfg.speed;
        setup_menus();
        setup_globgrp();
        load_images();
        inicia_globgpos();
        setup_gparchis();
        GrSetContext(globctx);
        paint_main_screen();
        return;
    }

    if (savedcfg->nboard != globcfg.nboard) {
        load_board();
        paint = 1;
    }

    if (savedcfg->showbarrs != globcfg.showbarrs) {
        paint = 1;
    }

    if (savedcfg->testopt != globcfg.testopt) {
        check_testoptions();
    }

    if (paint) paint_board();
}
    
/***********************/

void window_resized(int width, int height)
{
    globcfg.gwidth = width;
    globcfg.gheight = height;
    clean_up();
    GUIEnd();
    GrI18nSetLang(globcfg.lang);
    mgrx_setup(0, NULL);
    grgui_setup();
    setup_globvar();
    req_speed = globcfg.speed;
    setup_menus();
    setup_globgrp();
    load_images();
    inicia_globgpos();
    setup_gparchis();
    GrSetContext(globctx);
    paint_main_screen();
}

/***********************/

void paint_main_screen(void)
{
    GUIDBPauseBltsToScreen();

    GrClearContext(WEBC_SEAGREEN);
    if (globvar.xorg > 8 && globvar.yorg > 8) {
        GrBox(globvar.xorg-4, globvar.yorg-4,
              globvar.xorg+globvar.tbldim+CTLDIMW+7,
              globvar.yorg+globvar.tbldim+3, WEBC_BLACK);
        GrBox(globvar.xorg-5, globvar.yorg-5,
              globvar.xorg+globvar.tbldim+CTLDIMW+8,
              globvar.yorg+globvar.tbldim+4, WEBC_BLACK);
    }

    GrSetContext(globvar.ctl);
    GrFilledBox(0, 0, GrMaxX(), GrMaxY(), WEBC_LEMONCHIFFON);
    GrBitBlt(NULL, 0, 60, imgtitle, 0, 0,
             imgtitle->gc_xmax, imgtitle->gc_ymax, GrWRITE);
    GUIGroupPaint(globgrp);
    GrBox(0, 0, GrMaxX(), GrMaxY(), WEBC_BLACK);
    GrSetContext(globctx);

    paint_board();
    paint_progres();
    if (globpt.status == PST_WAITPASSACK || globpt.status == PST_WAITJG)
        paint_dice(globpt.vdice, playercolor[globpt.pp.turno]);
    else
        paint_dice(0, playercolor[globpt.pp.turno]);
    calc_playable_pawn_positions();
    set_instructions();

    GUIDBRestartBltsToScreen();
    GUIDBCurCtxBltToScreen();
}

/***********************/

void show_manual_dice(int show)
{
    GrContext grcaux;

    GrSaveContext(&grcaux);

    if (show) {
        globgstatus.waitmanualdice = 1;
        GUIGroupSetVisible(globgrp, 1, 0);
        GUIGroupSetVisible(globgrp, 2, 0);
        GUIGroupSetVisible(globgrp, 3, 1);
        GUIGroupSetVisible(globgrp, 4, 1);
        GUIGroupSetVisible(globgrp, 5, 1);
        GUIGroupSetVisible(globgrp, 6, 1);
        GUIGroupSetVisible(globgrp, 7, 1);
        GUIGroupSetVisible(globgrp, 8, 1);
    } else {
        globgstatus.waitmanualdice = 0;
        GUIGroupSetVisible(globgrp, 1, 1);
        GUIGroupSetVisible(globgrp, 2, 1);
        GUIGroupSetVisible(globgrp, 3, 0);
        GUIGroupSetVisible(globgrp, 4, 0);
        GUIGroupSetVisible(globgrp, 5, 0);
        GUIGroupSetVisible(globgrp, 6, 0);
        GUIGroupSetVisible(globgrp, 7, 0);
        GUIGroupSetVisible(globgrp, 8, 0);
    }

    GrSetContext(globvar.ctl);
    GUIGroupRestartAfterVisibleChanges(globgrp);
    GUIDBCurCtxBltToScreen();

    GrSetContext(&grcaux);
}

/***********************/

int check_saved_game(char *fname, int ask)
{
    static char *info[2];

    FILE *f;
    int saved_exist = 0;
    int ret;

    f = fopen(fname, "r");
    if (f) {
        saved_exist = 1;
        fclose(f);
    }

    if (!saved_exist) return 0;

    if (ask) {
        obscure();
        info[0] = _(SDSV_MES1);
        info[1] = _(SDSV_MES2);
        ret = GUICDialogYesNo(_(SDSV_TITLE), (void **)info, 2,
                              _(SGN_YES), _(SGN_NO));
        desobscure();
    } else {
        ret = 1;
    }

    if (ret == 1) {
        if (load_game(fname)) {
            AnimationsStopAll();
            genera_munecos(&(globpt.pp.dp));
            ini_globgstatus();
            paint_main_screen();
        } else {
            ret = 0;
        }
    }

    remove(fname);
    
    return (ret == 1) ? 1 : 0;
}

/***********************/

static int rota_coord(int *x, int *y)
{
    int angle, w, xs, ys;
    
    angle = globcfg.rotang % 360;
    if (angle < 0) angle = 360 + angle;
    w = globvar.tbldim;
    xs = *x;
    ys = *y;

    if (angle == 0) {
        return 1;
    } else if (angle == 90) {
        *x = ys;
        *y = w - 1 - xs;
        return 1;
    } else if (angle == 180) {
        *x = w - 1 - xs;
        *y = w - 1 - ys;
        return 1;
    } else if (angle == 270) {
        *x = w - 1 -ys;
        *y = xs;
        return 1;
    }
    return 0;
}

/***********************/

int process_board_event(GrEvent *ev)
{
    static long lasttime = 0;
    static int indjg = 0;
    int i, x, y;
    static int oldrotang;
    static double oldangle, preangle, acuangle;
    double newangle, salto;
    int rotfinal, rotdist, actrotang;
    //int rotincr;

    if (globgstatus.rotating) return 0;

    // finger pawn move

    if ((!globgstatus.handrotating && globpt.status == PST_WAITJG) &&
        (globpt.pp.dp.playertype[globpt.pp.turno] == PERSON)) {
        x = ev->p2;
        y = ev->p3;
        if (!rota_coord(&x, &y)) return 0;
        if (ev->type == GREV_MOUSE && ev->p1 == GRMOUSE_LB_PRESSED) {
            for (i=0; i<globpt.gjg.njg; i++) {
                indjg++;
                if (indjg >= globpt.gjg.njg) indjg = 0;
                if (GrCheckCoordInto(x, y, globgpos.jgorig[indjg].x,
                    globgpos.jgorig[indjg].y, globvar.pawndim, globvar.pawndim) ||
                    (globpt.gjg.jg[indjg].entra2 &&
                    GrCheckCoordInto(x, y, globgpos.jgorig2[indjg].x,
                    globgpos.jgorig2[indjg].y, globvar.pawndim, globvar.pawndim))) {
                    PTSetJg(&globpt,indjg);
                    globgpos.mouse.x = x - globvar.pawndim / 2;
                    globgpos.mouse.y = y - globvar.pawndim / 2;
                    globgstatus.mousingpawn = 1;
                    GrMouseEraseCursor();
                    paint_board();
                    return 1;
                }
            }
        } else if (ev->type == GREV_MOUSE && ev->p1 == GRMOUSE_LB_RELEASED) {
            if (globgstatus.mousingpawn) {
                globgstatus.mousingpawn = 0;
                if (globgstatus.pawnoverdest) {
                    globgstatus.pawnoverdest = 0;
                    globgstatus.pawnfingermoved = 1;
                    GrEventParEnqueue(GREV_COMMAND, COMMAND_A, 0, 0, 0);
                } else {
                    globgstatus.pawnfingermoved = 0;
                    paint_board();
                }
                GrMouseDisplayCursor();
                return 1;
            }
        } else if (ev->type == GREV_MMOVE) {
            if (globgstatus.mousingpawn) {
                if (ev->time > lasttime + 20) {
                    globgstatus.pawnoverdest = 0;
                    if (GrCheckCoordInto(x, y,
                            globgpos.jgdest[globpt.njgselec].x-globvar.pawndim/2,
                            globgpos.jgdest[globpt.njgselec].y-globvar.pawndim/2,
                            globvar.pawndim*2, globvar.pawndim*2)) {
                        globgstatus.pawnoverdest = 1;
                    }
                    globgpos.mouse.x = x - globvar.pawndim / 2;
                    globgpos.mouse.y = y - globvar.pawndim / 2;
                    paint_board();
                    lasttime = ev->time;
                }
                return 1;
            }
        }
    }

    // rotation

    if (ev->type == GREV_MOUSE && ev->p1 == GRMOUSE_LB_PRESSED) {
        oldrotang = globcfg.rotang;
        oldangle = atan2(ev->p3-globvar.tbldim/2, ev->p2-globvar.tbldim/2);
        preangle = oldangle;
        acuangle = 0;
        globgstatus.handrotating = 1;
        return 1;
    } else if (ev->type == GREV_MOUSE && ev->p1 == GRMOUSE_LB_RELEASED) {
        if (globgstatus.handrotating) {
            globcfg.rotang %= 360;
            if (globcfg.rotang < 0) globcfg.rotang += 360;
            if (globcfg.rotang >= 45 && globcfg.rotang < 135) rotfinal = 90;
            else if (globcfg.rotang >= 135 && globcfg.rotang < 225) rotfinal = 180;
            else if (globcfg.rotang >= 225 && globcfg.rotang < 315) rotfinal = 270;
            else rotfinal = 0;
            rotdist = rotfinal - globcfg.rotang;
            if (globcfg.rotang >= 315) rotdist = 360 - globcfg.rotang;
            if (abs(rotdist) > 2 ) {
                //rotincr = (rotdist > 0) ? 1 : -1;
                //StartRoTblAnimation(abs(rotdist), rotincr);
                StartRoTblAnimation2(rotfinal);
            } else {
                globcfg.rotang = rotfinal;
                paint_board();
            }
            globgstatus.handrotating = 0;
            return 1;
        }
    } else if (ev->type == GREV_MMOVE) {
        if (globgstatus.handrotating) {
            if (ev->time > lasttime + 20) {
                newangle = atan2(ev->p3-globvar.tbldim/2, ev->p2-globvar.tbldim/2);
                salto = newangle - preangle;
                if (salto > PI) salto -= PI * 2;
                if (salto < PI) salto += PI * 2;
                acuangle += salto;
                preangle = newangle;
                actrotang = globcfg.rotang;
                globcfg.rotang = oldrotang + (newangle - oldangle) / 0.0174532925199;
                if (actrotang != globcfg.rotang) {
                    paint_board();
                    //printf("%d \n", globcfg.rotang);
                }
                lasttime = ev->time;
            }
            return 1;
        }
    }

    return 0;
}

/***********************/

void set_buttons_text(char *tb1, char *tb2)
{
    GrContext grcaux;

    GrSaveContext(&grcaux);
    GrSetContext(globvar.ctl);

    GUIGroupSetText(globgrp, 1, tb1);
    GUIGroupRePaintObject(globgrp, 1);
    GUIGroupSetText(globgrp, 2, tb2);
    GUIGroupRePaintObject(globgrp, 2);
   
    GrSetContext(&grcaux);
}
   
/***********************/

void set_instructions(void)
{
    if (globonpause) {
        if (globonmoviola) {
            paint_instruction(_(SMAIN_CONTMOVL), NULL);
            set_buttons_text(_(SMAIN_STOPMOVL), _(SMAIN_CONT));
        } else {
            paint_instruction(_(SMAIN_GPAUSED), NULL);
            set_buttons_text("", _(SMAIN_CONT));
        }
        return;
    }

    if (globonmoviola) {
        paint_instruction(_(SMAIN_ONMOVL), NULL);
        set_buttons_text(_(SMAIN_STOPMOVL), _(SMAIN_PAUSE));
        return;
    }

    if (globpt.status == PST_ENDGAME) {
        paint_instruction(_(SMAIN_GOVER), NULL);
        set_buttons_text(_(SMAIN_REPLAY), _(SMAIN_REPGAME));
        return;
    }

    if (globpt.pp.dp.playertype[globpt.pp.turno] == PERSON) {
        switch (globpt.status) {
            case PST_WAITDICE :
                paint_instruction(_(SMAIN_YROLL), NULL);
                set_buttons_text(_(SMAIN_ROLL), "");
                break;
            case PST_WAITPASSACK :
                paint_instruction(_(SMAIN_YPASS), NULL);
                set_buttons_text(_(SMAIN_PASS), "");
                break;
            case PST_WAITJG :
                if (globpt.gjg.njg > 1) {
                    paint_instruction(_(SMAIN_YSELECT), _(SMAIN_ANDMOVE));
                    set_buttons_text(_(SMAIN_MOVE), _(SMAIN_SELECT));
                } else {
                    if (globpt.gjg.jg[0].acasa)
                        paint_instruction(_(SMAIN_Y3SIX), _(SMAIN_YGOH));
                    else
                        paint_instruction(_(SMAIN_YMOVE), NULL);
                    set_buttons_text(_(SMAIN_MOVE), "");
                }
                break;
        }
    } else {
        switch (globpt.status) {
            case PST_WAITDICE :
                paint_instruction(_(SMAIN_IROLL), NULL);
                break;
            case PST_WAITPASSACK :
                paint_instruction(_(SMAIN_IPASS), NULL);
                break;
            case PST_WAITJG :
                if (globpt.gjg.jg[0].acasa)
                    paint_instruction(_(SMAIN_I3SIX), _(SMAIN_IGOH));
                else
                    paint_instruction(_(SMAIN_IMOVE), NULL);
                break;
        }
        set_buttons_text("", _(SMAIN_PAUSE));
    }

    if (globpt.pp.dp.manualdice && globpt.status == PST_WAITDICE) {
        paint_instruction(_(SMAIN_MDICE1), _(SMAIN_MDICE2));
        show_manual_dice(1);
    }
}

/***********************/

void do_next_step(void)
{
    PTNextStep(&globpt);
    if (globpt.status == PST_ENDGAME) {
        HistPartAcuData(&globpt);
        if (globcfg.autorepeat)
            GrEventParEnqueue(GREV_COMMAND, COMMAND_REPEATGAME, 0, 0, 0);
    }
    set_instructions();
    calc_playable_pawn_positions();
}

/***********************/

void play_partida_command(int command, int id)
{
    int vdice, njg;
    DefPartida dpaux;

    if (globonmoviola) {
        if (command == COMMAND_A) {
            globpt = globptsaved;
            globonpause = 0;
            AnimationsStopAll();
            ini_globgstatus();
            globonmoviola = 0;
            set_instructions();
            paint_main_screen();
        } else if (command == COMMAND_B) {
            if (globonpause) {
                globonpause = 0;
                set_instructions();
            } else {
                globonpause = 1;
                set_instructions();
            }
        }
        return;
    }

    if (globpt.status == PST_ENDGAME) {
        if (command == COMMAND_A) {
            globptsaved = globpt;
            MPResetMovl(&globmovpt);
            AnimationsStopAll();
            dpaux = globpt.pp.dp;
            PTInit(&globpt, &dpaux);
            ini_globgstatus();
            globonmoviola = 1;
            set_instructions();
            paint_main_screen();
        } else if (command == COMMAND_B) {
            GrEventParEnqueue(GREV_COMMAND, COMMAND_REPEATGAME, 0, 0, 0);
        }
        return;
    }

    if (globpt.pp.dp.manualdice && globpt.status == PST_WAITDICE) {
        if (command == COMMAND_MD) {
            vdice = id - 2; // this is the object id!!
            if (vdice < 1 || vdice > 6) return; // not likely
            PTSetDice(&globpt, vdice);
            paint_dice(globpt.vdice, playercolor[globpt.pp.turno]);
            show_manual_dice(0);
            do_next_step();
            paint_board();
        }
        return;
    }

    if (globpt.pp.dp.playertype[globpt.pp.turno] == PERSON) {
        globonpause = 0;
        switch (globpt.status) {
            case PST_WAITDICE :
                if (command == COMMAND_A) {
                    if (globgstatus.workingdice == 0) {
                        vdice = get_diceval();
                        PTSetDice(&globpt, vdice);
                        StartDiceAnimation(2*globcfg.speed, globpt.vdice,
                                           playercolor[globpt.pp.turno]);
                        globgstatus.workingdice = 1;
                    }
                }
                if (command == COMMAND_B) {
                    //MPPrint(&globmovpt, stdout);
                    //StartRoTblAnimation(90);
                }
                break;
            case PST_WAITPASSACK :
                if (command == COMMAND_A) {
                    MPAddPass(&globmovpt, globpt.pp.turno, globpt.vdice);
                    do_next_step();
                    paint_board();
                    paint_dice(0, playercolor[globpt.pp.turno]);
                }
                break;
            case PST_WAITJG :
                if (command == COMMAND_A) {
                    if (globgstatus.oncarrera == 0) {
                        set_globcarr(&(globpt.gjg.jg[globpt.njgselec]));
                        globgstatus.pawnfingermoved = 0;
                        globgstatus.oncarrera = 1;
                        StartCarreraAnimation(globcfg.speed);
                    }
                }
                if (command == COMMAND_B) {
                    njg = globpt.njgselec + 1;
                    if (njg >= globpt.gjg.njg) njg = 0;
                    PTSetJg(&globpt,njg);
                    paint_board();
                }
                break;
        }
    } else {
        if (command == COMMAND_B) {
            if (globonpause) {
                globonpause = 0;
                set_instructions();
            } else {
                globonpause = 1;
                set_instructions();
            }
            //StartRoTblAnimation(-90);
        }
        if (command == COMMAND_A) {
            //StartRoTblAnimation(90);
        }
    }
}

/***********************/

void play_partida_auto(void)
{
    int vdice;

    if (globonpause) return;

    if (globpt.pp.dp.manualdice && globpt.status == PST_WAITDICE) return;

    if (globpt.pp.dp.playertype[globpt.pp.turno] == PERSON) {
        switch (globpt.status) {
            case PST_WAITDICE :
                if (globgstatus.workingdice == 2) {
                    do_next_step();
                    paint_board();
                    globgstatus.workingdice = 0;
                }
                break;
            case PST_WAITPASSACK :
                break;
            case PST_WAITJG :
                if (globgstatus.oncarrera == 2) {
                    globgstatus.oncarrera = 0;
                    MPAddJg(&globmovpt, &(globpt.gjg.jg[globpt.njgselec]));
                    do_next_step();
                    paint_progres();
                    paint_board();
                    if (globpt.status == PST_WAITPASSACK ||
                        globpt.status == PST_WAITJG)
                        paint_dice(globpt.vdice, playercolor[globpt.pp.turno]);
                    else
                        paint_dice(0, playercolor[globpt.pp.turno]);
                }
                break;
        }
    } else {
        switch (globpt.status) {
            case PST_WAITDICE :
                if (globgstatus.workingdice == 0) {
                    vdice = get_diceval();
                    PTSetDice(&globpt, vdice);
                    StartDiceAnimation(2*globcfg.speed, globpt.vdice,
                                       playercolor[globpt.pp.turno]);
                    globgstatus.workingdice = 1;
                } else if (globgstatus.workingdice == 2) {
                    do_next_step();
                    //paint_board();
                    globgstatus.workingdice = 0;
                }
                break;
            case PST_WAITPASSACK :
                if (globgstatus.robotpassing == 0) {
                    StartRPassAnimation(1*globcfg.speed);
                    globgstatus.robotpassing = 1;
                } else if (globgstatus.robotpassing == 2) {
                    MPAddPass(&globmovpt, globpt.pp.turno, globpt.vdice);
                    do_next_step();
                    paint_board();
                    paint_dice(0, playercolor[globpt.pp.turno]);
                    globgstatus.robotpassing = 0;
                }
                break;
            case PST_WAITJG :
                if (globgstatus.oncarrera == 0) {
                    if (globgstatus.robotpawnshow == 0) {
                        PTSelectRobotJg(&globpt);
                        if (globcfg.printanal &&
                            globpt.pp.dp.playertype[globpt.pp.turno] == ROBOT &&
                            globpt.status == PST_WAITJG) {
                            GJGPrintAnal(&(globpt.gjg), stdout);
                        }
                        paint_board();
                        StartRShowAnimation(2*globcfg.speed);
                        globgstatus.robotpawnshow = 1;
                    } else if (globgstatus.robotpawnshow == 2) {
                        set_globcarr(&(globpt.gjg.jg[globpt.njgselec]));
                        globgstatus.oncarrera = 1;
                        StartCarreraAnimation(globcfg.speed);
                        globgstatus.robotpawnshow = 0;
                    }
                } else if (globgstatus.oncarrera == 2) {
                    globgstatus.oncarrera = 0;
                    MPAddJg(&globmovpt, &(globpt.gjg.jg[globpt.njgselec]));
                    do_next_step();
                    paint_progres();
                    paint_board();
                    if (globpt.status == PST_WAITPASSACK ||
                        globpt.status == PST_WAITJG)
                        paint_dice(globpt.vdice, playercolor[globpt.pp.turno]);
                    else
                        paint_dice(0, playercolor[globpt.pp.turno]);
                }
                break;
        }
    }
}

/***********************/

void play_partida_moviola(void)
{
    RegMov *rmovact;
    int vdice;

    if (globonpause) return;

    if (globpt.status == PST_ENDGAME) {
        GrEventParEnqueue(GREV_COMMAND, COMMAND_A, 0, 0, 0);
    }

    rmovact = MPGetJgMovl(&globmovpt);

    switch (globpt.status) {
        case PST_WAITDICE :
            if (globgstatus.workingdice == 0) {
                vdice = (rmovact->vdice == 7) ? 6 : rmovact->vdice;
                PTSetDice(&globpt, vdice);
                StartDiceAnimation(2*globcfg.speed, globpt.vdice,
                                   playercolor[globpt.pp.turno]);
                globgstatus.workingdice = 1;
            } else if (globgstatus.workingdice == 2) {
                PTNextStep(&globpt);
                //paint_board();
                globgstatus.workingdice = 0;
            }
            break;
        case PST_WAITPASSACK :
            if (globgstatus.robotpassing == 0) {
                StartRPassAnimation(1*globcfg.speed);
                globgstatus.robotpassing = 1;
            } else if (globgstatus.robotpassing == 2) {
                PTNextStep(&globpt);
                paint_board();
                paint_dice(0, playercolor[globpt.pp.turno]);
                globgstatus.robotpassing = 0;
                MPForwardMovl(&globmovpt);
            }
            break;
        case PST_WAITJG :
            if (globgstatus.oncarrera == 0) {
                if (globgstatus.robotpawnshow == 0) {
                    int njg = 0;
                    for (int i=0; i<globpt.gjg.njg; i++) {
                        Jugada *jg = &(globpt.gjg.jg[i]);
                        if (jg->nficha == rmovact->nf && jg->hptep == rmovact->hpc) {
                            njg = i;
                            break;
                        }
                    }
                    PTSetJg(&globpt,njg);
                    paint_board();
                    StartRShowAnimation(2*globcfg.speed);
                    globgstatus.robotpawnshow = 1;
                } else if (globgstatus.robotpawnshow == 2) {
                    set_globcarr(&(globpt.gjg.jg[globpt.njgselec]));
                    globgstatus.oncarrera = 1;
                    StartCarreraAnimation(globcfg.speed);
                    globgstatus.robotpawnshow = 0;
                }
            } else if (globgstatus.oncarrera == 2) {
                globgstatus.oncarrera = 0;
                PTNextStep(&globpt);
                paint_progres();
                paint_board();
                MPForwardMovl(&globmovpt);
                if (globpt.status == PST_WAITPASSACK ||
                    globpt.status == PST_WAITJG)
                    paint_dice(globpt.vdice, playercolor[globpt.pp.turno]);
                else
                    paint_dice(0, playercolor[globpt.pp.turno]);
            }
            break;
    }
}

/***********************/

void dlg_about(void)
{
    char *atext[5];
    
    atext[0] = _(SDAB_LINE1);
    atext[1] = _(SDAB_LINE2);
    atext[2] = _(SDAB_LINE3);
    atext[3] = _(SDAB_LINE4);
    atext[4] = _(SDAB_LINE5);

    GUICDialogInfo(_(SDAB_TITLE), (void **)atext, 5, _(SGN_OK));
}

/***********************/

int get_diceval(void)
{
    return (int)(((6LL * random()) / (1LL + RAND_MAX)) + 1);
}

/***********************/

static GrContext *obscure_ctx = NULL;

/***********************/

void obscure(void)
{
    unsigned char *p;
    int i, nbytes;

    obscure_ctx = GrCreateContext(GrScreenX(), GrScreenY(), NULL, NULL);
    if (obscure_ctx == NULL) return;
    GrBitBlt(obscure_ctx, 0, 0, globctx, 0, 0, GrScreenX()-1, GrScreenY()-1, GrWRITE);
    nbytes = globctx->gc_lineoffset * (globctx->gc_ymax + 1);
    p = (unsigned char *)globctx->gc_baseaddr[0];
    //p = ctx->gc_baseaddr[0];
    for (i=0; i<nbytes; i++) {
        *p = *p / 2;
        p++;
    }
    GrBitBlt(GrScreenContext(), 0, 0, globctx, 0, 0,
             GrScreenX()-1, GrScreenY()-1, GrWRITE);
}

/***********************/

void desobscure(void)
{
    if (obscure_ctx == NULL) return;

    GrBitBlt(globctx, 0, 0, obscure_ctx, 0, 0,
             GrScreenX()-1, GrScreenY()-1, GrWRITE);
    GrBitBlt(GrScreenContext(), 0, 0, globctx, 0, 0,
             GrScreenX()-1, GrScreenY()-1, GrWRITE);
    GrDestroyContext(obscure_ctx);
    obscure_ctx = NULL;
}

/***********************/
/*
void obscure(void)
{
    GrContext *ctx;
    unsigned char *p;
    int i, nbytes;

    ctx = GrCreateContext(GrScreenX(), GrScreenY(), NULL, NULL);
    if (ctx == NULL) return;
    GrBitBlt(ctx, 0, 0, globctx, 0, 0, GrScreenX()-1, GrScreenY()-1, GrWRITE);
    nbytes = ctx->gc_lineoffset * (ctx->gc_ymax + 1);
    p = (unsigned char *)ctx->gc_baseaddr[0];
    //p = ctx->gc_baseaddr[0];
    for (i=0; i<nbytes; i++) {
        *p = *p / 2;
        p++;
    }
    GrBitBlt(GrScreenContext(), 0, 0, ctx, 0, 0,
             GrScreenX()-1, GrScreenY()-1, GrWRITE);
    GrDestroyContext(ctx);
}
*/
/***********************/
/*
void desobscure(void)
{
    GrBitBlt(GrScreenContext(), 0, 0, globctx, 0, 0,
             GrScreenX()-1, GrScreenY()-1, GrWRITE);
}
*/
/***********************/

#define FILECFGSIGN "P500CFG"   // to identify the file

/***********************/

void load_globcfg(char *fname)
{
    FILE *f;
    char s[201];
    int l;

    globcfg.lang = 0;
    globcfg.gwidth = 1150;
    globcfg.gheight = 950;
    globcfg.maxrsz = 0;
    globcfg.nboard = 1;
    globcfg.showbarrs = 1;
    globcfg.speed = 7;
    req_speed = 7;
    globcfg.rotang = 0;
    globcfg.testopt = 0;
    globcfg.testingfichas = 0;
    globcfg.autorepeat = 0;
    globcfg.printanal = 0;

    DPSet(&(globcfg.defaultdp), GT_SINGLE, PERSON, ROBOT, ROBOT, ROBOT,
          LEVEL0, LEVEL1, LEVEL1, LEVEL1);

    f = fopen(fname, "rt");
    if (f == NULL) return;

    cfgfile_exist = 1;

    fgets(s, 200, f);
    l = strlen(FILECFGSIGN);
    if (strncmp(s, FILECFGSIGN, l) != 0) return;

    while (fgets(s, 200, f) != NULL) {
        if (strncmp(s, "LAN=", 4) == 0) {
            globcfg.lang = atoi(&(s[4]));
            if (globcfg.lang < 0 || globcfg.lang >= NUM_LANGUAGES)
                globcfg.lang = 0;
        } else if (strncmp(s, "DFP=", 4) == 0) {
            DPSetFromString(&(globcfg.defaultdp), &(s[4]));
        } else if (strncmp(s, "GBW=", 4) == 0) {
            globcfg.gwidth = atoi(&(s[4]));
            if (globcfg.gwidth < 640) globcfg.gwidth = 640;
        } else if (strncmp(s, "GBH=", 4) == 0) {
            globcfg.gheight = atoi(&(s[4]));
            if (globcfg.gheight < 480) globcfg.gheight = 480;
        } else if (strncmp(s, "BRD=", 4) == 0) {
            globcfg.nboard = atoi(&(s[4]));
            if (globcfg.nboard < 0 || globcfg.nboard > LASTNBOARD)
                globcfg.nboard = 0;
        } else if (strncmp(s, "SHB=", 4) == 0) {
            globcfg.showbarrs = (atoi(&(s[4])) != 0) ? 1 : 0;
        } else if (strncmp(s, "SPD=", 4) == 0) {
            req_speed = atoi(&(s[4]));
        } else if (strncmp(s, "ROT=", 4) == 0) {
            globcfg.rotang = atoi(&(s[4]));
        } else if (strncmp(s, "MRZ=", 4) == 0) {
            globcfg.maxrsz = (atoi(&(s[4])) != 0) ? 1 : 0;
        } else if (strncmp(s, "TOP=", 4) == 0) {
            globcfg.testopt = (atoi(&(s[4])) != 0) ? 1 : 0;
        }
    }

    fclose(f);
}

/***********************/

void save_globcfg(char *fname)
{
    FILE *f;
    char s[201];

    f = fopen(fname, "wt");
    if (f == NULL) return;

    fprintf(f, "%s\n", FILECFGSIGN);

    fprintf(f, "LAN=%d\n", globcfg.lang);
    fprintf(f, "GBW=%d\n", globcfg.gwidth);
    fprintf(f, "GBH=%d\n", globcfg.gheight);
    DPToString(&(globcfg.defaultdp), s);
    fprintf(f, "DFP=%s\n", s);
    fprintf(f, "BRD=%d\n", globcfg.nboard);
    fprintf(f, "SHB=%d\n", globcfg.showbarrs);
    fprintf(f, "SPD=%d\n", globcfg.speed);
    fprintf(f, "MRZ=%d\n", globcfg.maxrsz);
    fprintf(f, "TOP=%d\n", globcfg.testopt);
    fclose(f);
}

/***********************/

static char SAVE_SIGNATURE[4] = {'P','v','5','1'};

/***********************/

int save_game(char *fname)
{
    FILE *f;

    f = fopen(fname, "wb");
    if (f == NULL) return 0;

    fwrite((void *)SAVE_SIGNATURE, 4, 1, f);
    fwrite((void *)&globonpause, sizeof(int), 1, f);
    fwrite((void *)&globonmoviola, sizeof(int), 1, f);
    PTSaveToFile(&globpt, f);
    PTSaveToFile(&globptsaved, f);
    MPSaveToFile(&globmovpt, f);
    fclose(f);

    return 1;
}

/***********************/

int load_game(char *fname)
{
    FILE *f;
    char buf[4];
    int len, onpause, onmoviola;
    Partida pt2, pt2saved;
    MovPartida mp2 = {0, 0, 0, NULL};

    f = fopen(fname, "rb");
    if (f == NULL) return 0;

    len = fread(buf, 4, 1, f);
    if (len != 1 || memcmp(buf, SAVE_SIGNATURE, 4) != 0) goto error;
    len = fread(&onpause, sizeof(int), 1, f);
    if (len != 1) goto error;
    len = fread(&onmoviola, sizeof(int), 1, f);
    if (len != 1) goto error;
    if (!PTLoadFromFile(&pt2, f)) goto error;
    if (!PTLoadFromFile(&pt2saved, f)) goto error;
    if (!MPLoadFromFile(&mp2, f)) goto error;

    globonpause = onpause;
    globonmoviola = onmoviola;
    globpt = pt2;
    globptsaved = pt2saved;
    free(globmovpt.r);
    globmovpt = mp2;
    fclose(f);

    return 1;

error:
    fclose(f);
    return 0;
}

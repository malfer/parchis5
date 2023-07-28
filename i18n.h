/*
 * i18n.h --- strings ids
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

#define NUM_LANGUAGES 2
 
#define _(sid) ((char *)GrI18nGetString(sid))

enum SID {
    SGN_OK = 0,              // generic strings
    SGN_CANCEL,
    SGN_YES,
    SGN_NO,
    SGN_ON,
    SGN_OFF,

    SMP_TITLE,               // main menu
    SMP_NEWGAME,
    SMP_GAMESTATS,
    SMP_SPEED,
    SMP_SETTINGS,
    SMP_GLBSTATS,
    SMP_HELP,
    SMP_TESTING,
    SMP_EXIT,

    SMV_SLOW,                // speed menu
    SMV_NORMAL,
    SMV_FAST,
    SMV_VFAST,
    SMV_INSTANT,

    SMT_TPAWN,               // test menu
    SMT_TLEFT,
    SMT_TRIGHT,
    SMT_SAVEG,
    SMT_LOADG,
    SMT_AUTOR,
    SMT_PANAL,
    SMT_PMOVL,

    SMH_SRULES,              // help menu
    SMH_PRULES,              // help menu
    SMH_INSTR,
    SMH_ABOUT,

    SMAIN_MENU,              // main program
    SMAIN_GPAUSED,
    SMAIN_CONT,
    SMAIN_YROLL,
    SMAIN_ROLL,
    SMAIN_YPASS,
    SMAIN_PASS,
    SMAIN_YSELECT,
    SMAIN_ANDMOVE,
    SMAIN_MOVE,
    SMAIN_SELECT,
    SMAIN_Y3SIX,
    SMAIN_YGOH,
    SMAIN_YMOVE,
    SMAIN_GOVER,
    SMAIN_IROLL,
    SMAIN_IPASS,
    SMAIN_I3SIX,
    SMAIN_IGOH,
    SMAIN_IMOVE,
    SMAIN_PAUSE,
    SMAIN_MDICE1,
    SMAIN_MDICE2,
    SMAIN_REPLAY,
    SMAIN_REPGAME,
    SMAIN_ONMOVL,
    SMAIN_CONTMOVL,
    SMAIN_STOPMOVL,

    SDEX_TITLE,              // exit dialog
    SDEX_MESSAGE,

    SDSV_TITLE,              // save dialog
    SDSV_MES1,
    SDSV_MES2,

    SDST_TITLE,              // settings dialog
    SDST_RES1,
    SDST_RES2,
    SDST_RES3,
    SDST_RES4,
    SDST_RES5,
    SDST_TBL1,
    SDST_TBL2,
    SDST_TBL3,
    SDST_GOGDIM,
    SDST_GOLANG,
    SDST_GOTBL,
    SDST_ADV,

    SDAST_TITLE,             // advanced settings dialog
    SDAST_MAXRSZ,
    SDAST_TOPT,

    SDAB_TITLE,              // about dialog
    SDAB_LINE1,
    SDAB_LINE2,
    SDAB_LINE3,
    SDAB_LINE4,
    SDAB_LINE5,

    SDNG_TITLE,              // new game dialog
    SDNG_SGAME,
    SDNG_PGAME,
    SDNG_H,
    SDNG_R0,
    SDNG_R1,
    SDNG_R2,
    SDNG_R3,
    SDNG_R4,
    SDNG_NP,
    SDNG_EXPL,
    SDNG_OPT,

    SDGO_TITLE,              // game options dialog
    SDGO_WHOB,
    SDGO_FEND,
    SDGO_FREND,
    SDGO_SFEND,
    SDGO_MDICE,

    SDES_TITLE,              // game statistics dialog
    SDES_CAB,

    SDGE_TITLE,              // global statistics dialog
    SDGE_SING,
    SDGE_PAIR,
    SDGE_GPLY,
    SDGE_LAST,
    SDGE_MDICE,
    SDGE_ADICE,
    SDGE_H,
    SDGE_R,

    SDHL_TSRUL,              // help dialog
    SDHL_TPRUL,
    SDHL_TGINS,

    SID_LAST
};

void AddEngStrings(int lid);
void AddEspStrings(int lid);

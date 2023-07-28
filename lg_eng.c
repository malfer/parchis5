/*
 * lg_eng.c --- English strings
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
#include <stdio.h>
#include <string.h>
#include <grgui.h>
#include "i18n.h"

static char *sgn[6] = {
    "Ok",
    "Cancel",
    "Yes",
    "No",
    "On",
    "Off"};

static char *smp[9] = {
    "    PARCHIS v5.1, MAIN MENU",
    "\x01\x02 &New game",
    "\x03\x04 &Game statistics",
    "\x05\x06 Game spee&d",
    "\x07\x08 Se&ttings",
    "\x09\x0a G&lobal statistics",
    "\x0b\x0c &Help",
    "   Te&sting",
    "\x0d\x0e E&xit"};

static char *smv[5] = {
    "&Slow",
    "&Normal",
    "&Fast",
    "&Very fast",
    "&Instantaneous"};

static char *smt[8] = {
    "&Test pawns",
    "Turn le&ft 90º slow",
    "Turn ri&ght 90º fast",
    "&Save game",
    "&Load game",
    "&Auto repeat game",
    "&Print analysis",
    "P&rint moves"};

static char *smh[4] = {
    "&Single game rules",
    "&Pairs game rules",
    "&Game instructions",
    "&About"};

static char *smain[28] = {
    "Menu",
    "Game paused",
    "Continue",
    "You roll the dice",
    "ROLL",
    "You can't move",
    "PASS",
    "Please select pawn",
    "and move",
    "MOVE",
    "SELECT",
    "Sorry, three six",
    "you go home",
    "You move",
    "Game Over",
    "I roll the dice",
    "I can't move",
    "Ops!, three six",
    "I go home",
    "I move",
    "Pause",
    "Please give me",
    "a dice value",
    "Replay",
    "Repeat game",
    "Replaying game",
    "Replay paused",
    "Stop"};

static char *sdex[2] = {
    "Exit",
    "Do you want to save the game before exit?"};

static char *sdsv[3] = {
    "Restore game",
    "There is a saved game,",
    "do you want to resore it?"};

static char *sdst[13] = {
    "\x07\x08 Settings",
    "Small 640x480",
    "Medium1 800x600",
    "Medium2 1024x768",
    "Default 1150x950",
    "Full Screen",
    "Basic",
    "Enhanced",
    "Marble",
    "Graphic dimensions",
    "Language",
    "Board Image",
    "Advanced"};

static char *sdast[3] = {
    "\x07\x08 Advanced Settings",
    "No resize over Default Gdims",
    "Active test options"};

static char *sdab[6] = {
    "About Parchís v5.1",
    "(C)2020,2023 Mariano Alvarez",
    "The parchís is a very popular board game in Spain",
    "Parchís v5.1 is Free Software,",
    "it makes use of the MRGX+GrGUI graphics library",
    "see readme.txt file for details"};

static char *sdng[12] = {
    "\x01\x02 New Game",
    "Single game",
    "Pairs game",
    "H",
    "R0",
    "R1",
    "R2",
    "R3",
    "R4",
    "NP",
    "   H=Human, Rx=Robot level x, NP=No Play",
    "Options"};

static char *sdgo[6] = {
    "\x01\x02 Game options",
    "Who begins",
    "Fast Ending",
    "Fast Robot Ending",
    "Super Fast Ending",
    "Manual dice"};

static char *sdes[2] = {
    "\x03\x04 Game Statistics",
    "   \x0f\x10   \x11\x12   \x13\x14   \x15\x16   \x17\x18"
                   "   \x19\x1a    Pass    Eat  Eaten GoHome"};

static char *sdge[9] = {
    "\x09\x0a Global Statistics",
    "Single",
    "Pairs",
    "games played",
    " last",
    "Manual dice",
    "Automatic dice",
    "H",
    "R"};

static char *sdhl[3] = {
    "\x0b\x0c Single game rules",
    "\x0b\x0c Pairs game rules",
    "\x0b\x0c Game instructions"};

void AddEngStrings(int lid)
{
    GrI18nSetLabel(lid, "English");
    GrI18nAddStrings(lid, SGN_OK, 6, (void **)sgn);
    GrI18nAddStrings(lid, SMP_TITLE, 9, (void **)smp);
    GrI18nAddStrings(lid, SMV_SLOW, 5, (void **)smv);
    GrI18nAddStrings(lid, SMT_TPAWN, 8, (void **)smt);
    GrI18nAddStrings(lid, SMH_SRULES, 4, (void **)smh);
    GrI18nAddStrings(lid, SMAIN_MENU, 28, (void **)smain);
    GrI18nAddStrings(lid, SDEX_TITLE, 2, (void **)sdex);
    GrI18nAddStrings(lid, SDSV_TITLE, 3, (void **)sdsv);
    GrI18nAddStrings(lid, SDST_TITLE, 13, (void **)sdst);
    GrI18nAddStrings(lid, SDAST_TITLE, 3, (void **)sdast);
    GrI18nAddStrings(lid, SDAB_TITLE, 6, (void **)sdab);
    GrI18nAddStrings(lid, SDNG_TITLE, 12, (void **)sdng);
    GrI18nAddStrings(lid, SDGO_TITLE, 6, (void **)sdgo);
    GrI18nAddStrings(lid, SDES_TITLE, 2, (void **)sdes);
    GrI18nAddStrings(lid, SDGE_TITLE, 9, (void **)sdge);
    GrI18nAddStrings(lid, SDHL_TSRUL, 3, (void **)sdhl);
}

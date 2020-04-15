/*
 * addons.h --- parchis addons include file
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

typedef struct {
    int gametype;       // GT_SINGLE or GT_PAIRS
    int playertype[4];  // PERSON, ROBOT or NOPLAY
    int robotlevel[4];  // LEVEL0 to LEVEL3 (by now), only if ROBOT
    int manualdice;     // used manual dice
    int stats[4][4];    // tines every color has finsidhed 1, 2, 3 or 4
    int totgames;       // total games played
    int lastdate;       // aaaammdd of last game
} RHistPart;

// on histpart.c

int HistPartInit(char *fname);
int HistPartSave(char *fname);
int HistPartAcuData(Partida *pt);

int HistPartGetNReg();
RHistPart * HistPartGetReg(int n);
int HistPartGetStringReg(int n, char *s);

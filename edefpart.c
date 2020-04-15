/*
 * edefpart.c --- Parchis engine, DefPartida functions
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
#include "engine.h"

void DPSet(DefPartida *dp, int gt, int pt1, int pt2, int pt3, int pt4,
           int rl1, int rl2, int rl3, int rl4)
{
    dp->gametype = gt;
    dp->playertype[AMAR] = pt1;
    dp->playertype[AZUL] = pt2;
    dp->playertype[ROJO] = pt3;
    dp->playertype[VERD] = pt4;

    dp->robotlevel[AMAR] = rl1;
    dp->robotlevel[AZUL] = rl2;
    dp->robotlevel[ROJO] = rl3;
    dp->robotlevel[VERD] = rl4;

    dp->whobegin = AMAR;
    dp->fastend = 0;
    dp->superfastend = 0;
    dp->fastrobotend = 0;
    dp->manualdice = 0;

    DPCountPlayers(dp);
}

void DPToString(DefPartida *dp, char *s)
{
    int c;

    s[0] = (dp->gametype == GT_SINGLE) ? 'S' : 'P';
    for (c=AMAR; c<=VERD; c++) {
        s[c+1] = (dp->playertype[c] == PERSON) ? 'P' :
                 ((dp->playertype[c] == ROBOT) ? 'R' : 'N');
        s[c+5] = dp->robotlevel[c] + '0';
    }
    s[9] = dp->whobegin + '0';
    s[10] = dp->fastend ? '1' : '0';
    s[11] = dp->superfastend ? '1' : '0';
    s[12] = dp->fastrobotend ? '1' : '0';
    s[13] = dp->manualdice ? '1' : '0';
    s[14] = '\0';
}

void DPSetFromString(DefPartida *dp, char *s)
{
    int c;

    dp->gametype = (s[0] == 'S') ? GT_SINGLE : GT_PAIRS;
    for (c=AMAR; c<=VERD; c++) {
        dp->playertype[c] = (s[c+1] == 'P') ? PERSON :
                            ((s[c+1] == 'R') ? ROBOT : NOPLAY);
        dp->robotlevel[c] = s[c+5] - '0';
        if (dp->robotlevel[c] < 0 || dp->robotlevel[c] > MAXRLEVEL)
            dp->robotlevel[c] = 0;
    }
    dp->whobegin = s[9] - '0';
    if (dp->whobegin < AMAR || dp->whobegin > VERD) dp->whobegin = AMAR;
    dp->fastend = (s[10] == '1') ? 1 : 0;
    dp->superfastend = (s[11] == '1') ? 1 : 0;
    dp->fastrobotend = (s[12] == '1') ? 1 : 0;
    dp->manualdice = (s[13] == '1') ? 1 : 0;

    DPCountPlayers(dp);
}

void DPCountPlayers(DefPartida *dp)
{
    int i;

    dp->nplayers = 0;
    dp->nhumplayers = 0;
    dp->nrobplayers = 0;

    for (i=AMAR; i<=VERD; i++) {
        if (dp->playertype[i] == PERSON) {
            dp->nplayers++;
            dp->nhumplayers++;
        } else if (dp->playertype[i] == ROBOT) {
            dp->nplayers++;
            dp->nrobplayers++;
        }
    }
}

int DPmatch(DefPartida *dp1, DefPartida *dp2)
{
    int i;

    if (dp1->gametype != dp2->gametype) return 0;

    for (i=AMAR; i<=VERD; i++) {
        if (dp1->playertype[i] != dp2->playertype[i]) return 0;
        if ((dp1->playertype[i] == ROBOT) &&
            (dp1->robotlevel[i] != dp2->robotlevel[i])) return 0;
    }

    return 1;
}

int DPSameTeam(int p1, int p2)
{
    if (p1 == AMAR && p2 == ROJO) return 1;
    if (p1 == ROJO && p2 == AMAR) return 1;
    if (p1 == AZUL && p2 == VERD) return 1;
    if (p1 == VERD && p2 == AZUL) return 1;
    return 0;
}

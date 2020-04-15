/*
 * histpart.c --- HistPart functions
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
#include <time.h>
#include "engine.h"
#include "addons.h"

#define FILESIGN "P500RHP"   // to identify the file

#define HPTMAXREGINCR 10   // the increment to exted the history when needed

typedef struct {
    int nreg;           // number of registers
    int maxreg;         // max number of registerss (extendible)
    RHistPart r[1];     // registers (not actual size!!)
} HistPart;

static HistPart *globhp = NULL;

static HistPart * HPCreate(int maxreg);
static HistPart * HPExtend(HistPart *hp);
static HistPart * HPLoadFromFile(char *fname);
static void regtostring(RHistPart *r, char *s);
static void stringtoreg(char *s, RHistPart *r);
static int HPSaveToFile(HistPart *hp, char *fname);
static int HPFindDP(HistPart *hp, DefPartida *dp);
static int HPAddReg(HistPart *hp, Partida *pt);
static void HPAcuReg(HistPart *hp, int reg, Partida *pt);
static void HPUpReg(HistPart *hp, int reg);

/***********************/

int HistPartInit(char *fname)
{
    globhp = HPLoadFromFile(fname);
    if (globhp) return 1;

    globhp = HPCreate(HPTMAXREGINCR);
    if (globhp) return 1;

    return 0;
}

/***********************/

int HistPartSave(char *fname)
{
    if (globhp == NULL) return 0;

    return HPSaveToFile(globhp, fname);
}

/***********************/

int HistPartAcuData(Partida *pt)
{
    int reg;
    HistPart *newhp;

    if (globhp == NULL) return 0;
    if (pt->pp.dp.nplayers < 2) return 0;

    reg = HPFindDP(globhp, &(pt->pp.dp));
    if (reg >= 0) {
        HPAcuReg(globhp, reg, pt);
        return 1;
    }

    if (globhp->nreg >= globhp->maxreg) {
        newhp = HPExtend(globhp);
        if (newhp)
            globhp = newhp;
        else
            return 0;
    }

    if (HPAddReg(globhp, pt) < 0) return 0;
    
    return 1;
}

/***********************/

int HistPartGetNReg()
{
    if (globhp == NULL) return 0;

    return globhp->nreg;
}

/***********************/

RHistPart * HistPartGetReg(int n)
{
    if (globhp == NULL) return NULL;
    if (n < 0 || n >= globhp->nreg) return NULL;

    return &(globhp->r[n]);
}

/***********************/

int HistPartGetStringReg(int n, char *s)
{
    if (globhp == NULL) return 0;
    if (n < 0 || n >= globhp->nreg) return 0;

    regtostring(&(globhp->r[n]), s);
    return 1;
}

/***********************/

static HistPart * HPCreate(int maxreg)
{
    size_t size;
    HistPart *hp;

    size = sizeof(HistPart) + ((maxreg - 1) * sizeof(RHistPart));
    hp = malloc(size);
    if (hp == NULL) return NULL;
    hp->nreg = 0;
    hp->maxreg = maxreg;
    return hp;
}

/***********************/

static HistPart * HPExtend(HistPart *hp)
{
    size_t newsize;
    int newmaxreg;
    HistPart *hpnew;

    newmaxreg = hp->maxreg + HPTMAXREGINCR;
    newsize = sizeof(HistPart) + ((newmaxreg - 1) * sizeof(RHistPart));
    hpnew = realloc(hp, newsize);
    if (hpnew == NULL) return NULL;
    hpnew->maxreg = newmaxreg;
    return hpnew;
}

/***********************/

static char *sgt = "SP";
static char *pyt = "PRN";
static char *rlv = "0123456";
static char *mdc = "NY";

static void regtostring(RHistPart *r, char *s)
{
    int c, j, ind;
    char *p;

    ind = (r->gametype == GT_SINGLE) ? 0 : 1;
    s[0] = sgt[ind];
    for (c=AMAR; c<=VERD; c++) {
        ind = (r->playertype[c] == PERSON) ? 0 :
              ((r->playertype[c] == ROBOT) ? 1 :2);
        s[c+1] = pyt[ind];
        ind = (r->playertype[c] == ROBOT) ? r->robotlevel[c] : 0;
        if (ind < 0 || ind > MAXRLEVEL) ind = 0;
        s[c+5] = rlv[ind];
    }
    ind = (r->manualdice) ? 1 : 0;
    s[9] = mdc[ind];
    p = &(s[10]);
    sprintf(p, " %08d %08d", r->totgames, r->lastdate);
    p = &(s[28]);
    for (c=AMAR; c<=VERD; c++) {
        for (j=0; j<4; j++) {
            sprintf(p, " %08d", r->stats[c][j]);
            p += 9;
        }
    }
    p[172] = '\0';
}

static void stringtoreg(char *s, RHistPart *r)
{
    int c, j;
    char *p;

    r->gametype = (s[0] == sgt[0]) ? GT_SINGLE : GT_PAIRS;
    for (c=AMAR; c<=VERD; c++) {
        r->playertype[c] = (s[c+1] == pyt[0]) ? PERSON :
                           ((s[c+1] == pyt[1]) ? ROBOT : NOPLAY);
        r->robotlevel[c] = s[c+5] - '0';
        if (r->robotlevel[c] < 0 || r->robotlevel[c] > MAXRLEVEL)
            r->robotlevel[c] = 0;
    }
    r->manualdice = (s[9] == mdc[0]) ? 0 : 1;
    p = &(s[10]);
    r->totgames = atoi(p);
    p = &(s[19]);
    r->lastdate = atoi(p);
    p = &(s[28]);
    for (c=AMAR; c<=VERD; c++) {
        for (j=0; j<4; j++) {
            r->stats[c][j] = atoi(p);
            p += 9;
        }
    }
}

/***********************/

static HistPart * HPLoadFromFile(char *fname)
{
    FILE *f = NULL;
    HistPart *hp = NULL;
    int i, l, nreg;
    char s[201];

    f = fopen(fname, "rt");
    if (f == NULL) goto error;

    fgets(s, 200, f);
    l = strlen(FILESIGN);
    if (strncmp(s, FILESIGN, l) != 0) goto error;
    nreg = atoi(&(s[l]));
    hp = HPCreate(nreg + HPTMAXREGINCR);
    if (hp == NULL) goto error;
    hp->nreg = nreg;
    hp->maxreg = nreg + HPTMAXREGINCR;
    for (i=0; i<hp->nreg; i++) {
        if (fgets(s, 200, f) == NULL) goto error;
        stringtoreg(s, &(hp->r[i]));
    }
    fclose(f);
    return hp;

error:
    if (f) fclose(f);
    if (hp) free(hp);
    return NULL;
}

/***********************/

static int HPSaveToFile(HistPart *hp, char *fname)
{
    int i;
    FILE *f;
    char s[201];

    f = fopen(fname, "wt");
    if (f == NULL) return 0;

    fprintf(f, "%s %d\n", FILESIGN, hp->nreg);

    for (i=0; i<hp->nreg; i++) {
        regtostring(&(hp->r[i]), s);
        fputs(s, f);
        fputc('\n', f);
    }

    fclose(f);

    return 1;
}

/***********************/

static int HPFindDP(HistPart *hp, DefPartida *dp)
{
    int i, j, found;

    for (i=0; i<hp->nreg; i++) {
        if (dp->gametype != hp->r[i].gametype) continue;
        if (dp->manualdice != hp->r[i].manualdice) continue;
        found = 1;
        for (j=AMAR; j<=VERD; j++) {
            if (dp->playertype[j] != hp->r[i].playertype[j]) {
                found = 0;
                break;
            }
            if (dp->playertype[j] == ROBOT &&
                dp->robotlevel[j] != hp->r[i].robotlevel[j]) {
                found = 0;
                break;
            }
        }
        if (found) return i;
    }

    return -1;
}

/***********************/

static int HPAddReg(HistPart *hp, Partida *pt)
{
    int c, j, nreg;
    RHistPart *r;
    time_t t;
    struct tm *lt;

    nreg = hp->nreg;
    if (nreg >= hp->maxreg) return -1;

    r = &(hp->r[nreg]);
    r->gametype = pt->pp.dp.gametype;
    for (c=AMAR; c<=VERD; c++) {
        r->playertype[c] = pt->pp.dp.playertype[c];
        r->robotlevel[c] = pt->pp.dp.robotlevel[c];
        for (j=0; j<4; j++) {
            r->stats[c][j] = 0;
        }
        if (pt->pp.dp.playertype[c] != NOPLAY)
            r->stats[c][pt->pp.endingpos[c]] = 1;
    }
    r->manualdice = pt->pp.dp.manualdice;
    r->totgames = 1;
    t = time(NULL);
    lt = localtime(&t);
    r->lastdate = (lt->tm_year + 1900) * 10000 +
                  (lt->tm_mon + 1) * 100 + lt->tm_mday;

    hp->nreg++;

    HPUpReg(hp, nreg);

    return 0;
}

/***********************/

static void HPAcuReg(HistPart *hp, int reg, Partida *pt)
{
    int c;
    RHistPart *r;
    time_t t;
    struct tm *lt;

    r = &(hp->r[reg]);
    for (c=AMAR; c<=VERD; c++) {
        if (pt->pp.dp.playertype[c] != NOPLAY)
            r->stats[c][pt->pp.endingpos[c]] += 1;
    }
    r->totgames += 1;
    t = time(NULL);
    lt = localtime(&t);
    r->lastdate = (lt->tm_year + 1900) * 10000 +
                  (lt->tm_mon + 1) * 100 + lt->tm_mday;

    HPUpReg(hp, reg);
}

/***********************/

static void HPUpReg(HistPart *hp, int reg)
{
    RHistPart raux;
    int i;

    raux = hp->r[reg];
    for (i=reg; i>0; i--) {
        hp->r[i] = hp->r[i-1];
    }
    hp->r[0] = raux;
}


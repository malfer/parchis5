/*
 * engine.h --- parchis engine include file
 * 
 * Copyright (c) 2020,2034 Mariano Alvarez Fernandez
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

#ifdef  __WIN32__
void srandom(int seed);
long random(void);
#endif

// game types

#define GT_SINGLE  0
#define GT_PAIRS   1

// player colors

#define AMAR       0
#define AZUL       1
#define ROJO       2
#define VERD       3

// player types

#define PERSON     0
#define ROBOT      1
#define NOPLAY     2

#define LEVEL0     0
#define LEVEL1     1
#define LEVEL2     2
#define LEVEL3     3
#define LEVEL4     4
#define MAXRLEVEL  4

typedef struct {
    int gametype;       // GT_SINGLE or GT_PAIRS
    int playertype[4];  // PERSON, ROBOT or NOPLAY
    int robotlevel[4];  // LEVEL0 to LEVEL3 (by now), only if ROBOT
    int whobegin;       // first color to play: AMAR, AZUL, ROJO or VERD
    int fastend;        // End game if only one player rest
    int superfastend;   // End game when finish the first player
    int fastrobotend;   // End game when only robots rest
    int manualdice;     // Use manual dice
    int nplayers;       // calculated player number
    int nhumplayers;    // calculated human player number
    int nrobplayers;    // calculated robot player number
} DefPartida;

// in edefpart.c

void DPSet(DefPartida *dp, int gt, int pt1, int pt2, int pt3, int pt4,
           int rl1, int rl2, int rl3, int rl4);
void DPToString(DefPartida *dp, char *s);
void DPSetFromString(DefPartida *dp, char *s);
void DPCountPlayers(DefPartida *dp);
int DPmatch(DefPartida *dp1, DefPartida *dp2);
int DPSameTeam(int p1, int p2);
int DPTeamColor(int c);

// places

#define CASA      0
#define CORR      1
#define LLEG      2
#define META      3

typedef struct {
    int place;          // CASA, CORR, LLEG or META
    int pos;            // 0     0-67  0-6     0
    int ind;            // 0-3   0-1   0-1     0-3
} PosFicha;

extern int corrent[4];    // enter corridor square for each color
extern int corrsal[4];    // exit corridor square for each color

// in eposfich.c

void PFSetPosition(PosFicha *pf, int place, int pos, int ind);
void PFCopyPosition(PosFicha *pfd, PosFicha *pfs);

//

typedef struct {
    DefPartida dp;         //
    PosFicha pf[4][4];     // pawn positions [color][nficha]
    int hasbegun[4];       // color has exit from CASA at least once
    int endingpos[4];      // 0 to 3 if color has finished, else -1
    int nfcasa[4];         // pawns in CASA
    int nfmeta[4];         // pawns in META
    int nextending;        // ending order of next color finsihing (0-3)
    int turno;             // color who play now
    int nsix;              // consecutive six obtained (3 -> go home)
    int lastmoved;         // last pawn moved by color turno
    int stats_dice[4][6];  // times dice value obtained [color][dice]
    int stats_totdice[4];  // total times a dice was thrown
    int stats_pass[4];     // times passed (can not make a move)
    int stats_eat[4];      // times color has eat an oponent pawn
    int stats_eaten[4];    // times color has been eaten
    int stats_gocasa[4];   // times color has gone CASA
    int distance[4];       // color distance to finish
} PosPartida;

typedef struct st_jugada Jugada;

// in epospart.c

void PPSetInitialPosition(PosPartida *pp, DefPartida *dp);
int PPHasBegun(PosPartida *pp);
int PPHasFinished(PosPartida *pp);
void PPCalcDistances(PosPartida *pp);
int PPAdvanceTurno(PosPartida *pp);
int PPCheckEndPairs(PosPartida *pp);
int PPCheckFastEnd(PosPartida *pp);
int PPDoJg(PosPartida *pp, Jugada *jg);

// No pawn inside

#define VACIO   -1

// CORR square types

#define NORMAL   0
#define SEGURO   1
#define SALIDA   2
#define ENTRADA  3
#define ENTRADA_AMAR  3
#define ENTRADA_AZUL  4
#define ENTRADA_ROJO  5
#define ENTRADA_VERD  6
 
typedef struct {          // CORR squares
    int cf[2];            // pawn color (or VACIO)
    int nf[2];            // pawn number
    int type;             // square type
} CasillaC;

typedef struct {          // LLEG squares
    int cf[2];            // pawn color (or VACIO)
    int nf[2];            // pawn number
} CasillaL;

typedef struct {          // CASA or META squares
    int cf[4];            // pawn color (or VACIO)
    int nf[4];            // pawn number
} Casilla4;

typedef struct {
    Casilla4 casa[4];
    CasillaC corr[68];
    CasillaL lleg[4][7];
    Casilla4 meta[4];
} Tablero;

// in etablero.c

void TBLInit(Tablero *tbl);
void TBLDelPawns(Tablero *tbl);
void TBLSetPawns(Tablero *tbl, PosPartida *pp);

//

struct st_jugada {
    int cjugador;       // Player color who moves
    int nficha;         // first pawn number to move
    int nficha2;        // second pawn number to move (only for first 5)
    int vdado;          // dice value
    int cjugdcom;       // Eaten player color (if any)
    int nfichcom;       // pawn number eaten
    int cjugdcom2;      // Second eaten player color (a rare case for first 5)
    int nfichcom2;      // seconf pawn number eaten
    int cjugddpl;       // color of player displaced (if any)
    int nfichdpl;       // pawn number displaced
    PosFicha origen;    // First pawn moved origin
    PosFicha destino;   // First pawn moved destination
    PosFicha origen2;   // Second pawn moved origin
    PosFicha destino2;  // Second pawn moved destination
    PosFicha destfcom;  // First pawn eaten destination
    PosFicha destfcom2; // Second pawn eaten destination
    // Normal analysis, filled at the same time that the move is generated
    int entra;          // entra en el corredor 
    int entra2;         // entra en el corredor primer 5 
    int sale;           // sale del corredor 
    int meta;           // entra en la meta 
    int acasa;          // a casa por tres seises 
    int come;           // come ficha 
    int comep;          // come ficha pero es pareja
    int come2;          // come dos fichas en primer 5
    int hpte;           // hace puente 
    int hptep;          // hace puente con pareja 
    int rpte;           // rompe puente 
    int dseg;           // deja seguro 
    int aseg;           // va a seguro 
    int hpcor;          // hace puente en corredor 
    int cor01;          // mueve en llegada de casillas 0,1 a mas internas 
    int rpte5;          // rompe puente en salida para facilitar sacar ficha 
    // Extended analysis, filled later
    int npc;            // come o meta pero no puede contar 
    int segun6;         // es el segundo seis 
    int nficom;         // n. de fichas comibles en la posicion inicial 
    int nfipel;         // n. de fichas peligrosas en la posicion inicial 
    int nffcom;         // n. de fichas comibles en la posicion final 
    int nffpel;         // n. de fichas peligrosas en la posicion final 
    int ncfin;          // inverso de n. de casillas que faltan para llegar 
    int nfi6com;        // n. de fichas comibles si 6+n en la posicion inicial 
    int nfi6pel;        // n. de fichas peligrosas si 6+n en la posicion inicial 
    int nff6com;        // n. de fichas comibles si 6+n en la posicion final 
    int nff6pel;        // n. de fichas peligrosas si 6+n en la posicion final 
    int njgliza;        // n. de jugadores que quedan en liza 
    int difdpri;        // diferencia de distancia con el primero, >= 0
    int difdpar;        // diferencia de distancia con la pareja, + voy antes, - detras
    // calculated weight and random value added
    int weight;
    int randw;
};

// in ejugada.c

void JGClean(Jugada *jg);
int JGGen(Jugada *jg, PosPartida *pp, Tablero *tbl,
          int cjg, int nf, int vdice, int eatmate);
int JGGenGoHome(Jugada *jg, PosPartida *pp, Tablero *tbl,
                int cjg, int nf, int vdice);
int JGGenFirst5(Jugada *jg, PosPartida *pp, Tablero *tbl, int cjg);
int JGCalcWeight(Jugada *jg, int level, int gametype);

//

typedef struct {
    Jugada jg[8];
    int njg;
} GrupoJugadas;

// in egrpjg.c

int GJGGen(GrupoJugadas *gjg, PosPartida *pp, int cjg, int vdice);
void GJGFillInfoAnl(GrupoJugadas *gjg, PosPartida *pp);
void GJGPrintAnal(GrupoJugadas *gjg, FILE *f);

//

typedef struct {
    int nficom[4][4];     // pawns that can be eaten for each
    int nfipel[4][4];     // pawns that can eat each
    int nfi6com[4][4];    // if 6+n
    int nfi6pel[4][4];    // if 6+n
} AnlGlobal;

// in eanlglb.c

void AGlobalN2(AnlGlobal *ag, PosPartida *pp);
void AGlobalN4(AnlGlobal *ag, PosPartida *pp);

// partida status

#define PST_WAITDICE     0
#define PST_WAITPASSACK  1
#define PST_WAITJG       2
#define PST_ENDGAME      3

typedef struct {
    PosPartida pp;
    int status;
    int vdice;
    int njgselec;
    int nextvdice;
    int lastfselec[4];
    GrupoJugadas gjg;
} Partida;

// in epartida.c

void PTInit(Partida *pt, DefPartida *dp);
void PTSetDice(Partida *pt, int vdice);
void PTSetJg(Partida *pt, int njg);
void PTSelectRobotJg(Partida *pt);
void PTNextStep(Partida *pt);
int PTSaveToFile(Partida *pt, FILE *f);
int PTLoadFromFile(Partida *pt, FILE *f);

// partida moves

typedef struct {
    char color;    // player color
    char vdice;    // dice value
    char nf;       // pawn number (-1 if pass)
    char hpc;      // special case, do bridge with mate, 0=no, 1=yes
} RegMov;

typedef struct {
    int maxnjg;         // max num of jugadas, can be extended
    int njg;            // num of jugadas
    int posmoviola;     // moviola position;
    RegMov *r;          // registers
} MovPartida;

// in emovpart.c

int MPExtend(MovPartida *mp);
void MPClear(MovPartida *mp);
void MPClean(MovPartida *mp);
int MPAddReg(MovPartida *mp, int color, int vdice, int nf, int hpc);
int MPAddPass(MovPartida *mp, int color, int vdice);
int MPAddJg(MovPartida *mp, Jugada *jg);
RegMov *MPGetJg(MovPartida *mp, int n);
void MPResetMovl(MovPartida *mp);
int MPForwardMovl(MovPartida *mp);
RegMov *MPGetJgMovl(MovPartida *mp);
void MPPrint(MovPartida *mp, FILE *f);
void MPSaveToFile(MovPartida *mp, FILE *f);
int MPLoadFromFile(MovPartida *mp, FILE *f);

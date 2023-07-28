/*
 * gparchis.h --- graphics related stuff include file
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

#define SMALLTBL 600  // if < 600 use small dice and other things
#define LASTNBOARD 2  // number of last board image

typedef struct {
    double scale;         // scale factor;
    int tbldim;           // board dimension
    int pawndim;          // pawn dimension
    int medaldim;         // medal dimension
    int munecodim;        // muñeco dimension
    int podiumdim;        // single podium dimension
    int podiumpdim;       // pairs podium dimension
    int movioladim;       // moviola dimension
    int xorg, yorg;       // x,y offset
    int xmenu, ymenu;     // menu position
    GrContext *tbl;       // board subcontext
    GrContext *tblpaint;  // memory board painting
    GrContext *ctl;       // control subcontext
    GrContext *pgb;       // progress bars subcontext
    GrContext *dce;       // dice subcontext
    GrContext *ins;       // instructions subcontext
} GlobVars;

typedef struct {
    int lang;             // language index
    int gwidth;           // global width
    int gheight;          // global height
    int maxrsz;           // if =1 don't resize if dims>default
    DefPartida defaultdp; // default DefPartida
    int nboard;           // board number to use
    int speed;            // actual speed of animations 10, 7, 3, 1, 0
    int rotang;           // actual angle the board is rotated
    int testopt;          // activate menu test options
    // testing
    int testingfichas;    // if 1 show pawns in every gposition
    int autorepeat;       // auto repeat games
    int printanal;        // print analisis values for robots
} GlobCfg;

typedef struct {
    int workingdice;      // 0=no, 1=yes, 2=just finsihed
    int robotpassing;     // 0=no, 1=yes, 2=just finsihed
    int robotpawnshow;    // 0=no, 1=yes, 2=just finsihed
    int blinkpawn;        // 0=no, 1=yes
    int oncarrera;        // 0=no, 1=yes
    int rotating;         // 0=no, 1=yes
    int mousingpawn;      // 0=no, 1=yes
    int pawnoverdest;     // 0=no, 1=yes
    int pawnfingermoved;  // 0=no, 1=yes
    int handrotating;     // 0=no, 1=yes
    int waitmanualdice;   // 0=no, 1=yes
} GStatus;

// in parchis5.c

extern GrContext *globctx;      // global context
extern GrColor TRANSPARENT;     // will be defined later as RGB(10,10,10) | GrIMAGE
extern GrColor playercolor[4];
extern GrColor playerlcolor[4];
extern GrColor cintcolor[5];

extern GlobVars globvar;
extern GlobCfg globcfg;
extern GStatus globgstatus;

extern int globonpause;
extern int globonmoviola;
extern Partida globpt;

void disaster(char *message);

// in gloadimg.c

extern GrContext *imgtbl;
extern GrContext *imgtitle;
extern GrContext *imgdice[10];
extern GrContext *imgpawn[4];
extern GrContext *imgpawncruz[4];
extern GrContext *imgpawndot[4];
extern GrContext *imgpawndest[2];
extern GrContext *imgpawnhapy[4];
extern GrContext *imgpawnsad[4];
extern GrContext *imgmedal[4];
extern GrContext *imgmuneco[4];
extern GrContext *imgmunecobl[4];
extern GrContext *imgpodium;
extern GrContext *imgpodiump;
extern GrContext *imgmoviola;

void load_board(void);
void load_images(void);
void genera_munecos(DefPartida *dp);

//

typedef struct {
  int x, y;
} PosG;

typedef struct {
    PosG casa[4][4];
    PosG corr[68][2];
    PosG lleg[4][7][2];
    PosG meta[4][4];
    PosG medalC[4];
    PosG medal[4];
    PosG muneco[4];
    // playable pawn position
    PosG jgorig[8];
    PosG jgorig2[8];
    PosG jgdest[8];
    PosG jgdest2[8];
    PosG mouse;
    // podiums
    PosG podium;
    PosG podpos[4];
    PosG podiump;
    PosG podppos[4];
    // others
    PosG moviola;
} GPositions;

typedef struct {
    int xorg, yorg;
    int deltax, deltay;
} GPosSlide;

// in gposgdef.c

extern GPositions globgpos;

void set_posg(PosG *pg, int x, int y);
void inicia_globgpos(void);
void get_xy_from_posficha(int *x, int *y, PosFicha *pf, int c);
void set_gposslide_from_posfichas(GPosSlide *gps, PosFicha *pfi,
                                  PosFicha *pff, int c, int steps);
void set_gposslide_for_medals(GPosSlide *gps, int c, int steps);
void get_xy_from_gposslide(int *x, int *y, GPosSlide *gps, int step);
void calc_playable_pawn_positions(void);
void calc_gcoord_rottbl(int *x, int *y);
void test_fichas(void);

// in gpaint.c

void paint_board(void);
void paint_moviola(void);
void paint_munecos(void);
void paint_munecos2(void);
void paint_fichas(void);
void paint_carrera(void);
void paint_medals(void);
void paint_medals2(void);
void paint_podium(void);
void paint_progres(void);
void paint_dice(int val, GrColor bg);
void paint_instruction(char *l1, char *l2);

//

typedef struct {
    Jugada jg;
    int stepsentra;       // != 0 if entra
    GPosSlide gpsentra;   //
    GPosSlide gpsentra2;  //
    int stepspasos;       // != 0 if normal move with dice>1
    int stepscome;        // != 0 if come
    GPosSlide gpscome;    //
    GPosSlide gpscome2;   //
    int stepsacasa;       // != 0 if a casa
    GPosSlide gpsacasa;   //
    int stepsmeta;        // != 0 if meta
    int stepsfin;         // != 0 if player ends
    GPosSlide gpsfin;     //
    GPosSlide gpsfin2;    //
    int stepstotal;       // total steps
    PosFicha paso[20];    // pasos of normal move
    int mode;             // 0=entra, 1=pasos, 2=come, 3=acasa, 4=meta, 5=fin
    int step;             // mode step
} GCarrera;

// in gcarrera.c

extern GCarrera globcar;

void set_globcarr(Jugada *jg);

//

typedef struct st_animation Animation;

struct st_animation {
    int running;                 // 1=yes, 0=no                   \  handled
    long lasttime;               // lasttime executes (0=first)    } by the
    int stepnumber;              // step number                   /  engine
    int steptime;                // interval time to run
    int numsteps;                // number of steps to run
    int (*doNextStep)(Animation *ani, int stop);  // return -1 when it finishs
    void *data;                  // private data
};
    
// in ganimat.c

void AnimationsInit(void);
int AnimationsAdd(Animation *ani);
void AnimationsRun(long time);
void AnimationsStopAll(void);

void StartDiceAnimation(int nsteps, int vdado, GrColor bg);
void StartRoTblAnimation(int nsteps, int incr);
void StartRoTblAnimation2(int rotfinal);
void StartRPassAnimation(int nsteps);
void StartRShowAnimation(int nsteps);
void StartCarreraAnimation(int speed);

// Dialogs

int NewGameDlgCreate(void);
void NewGameDlgDestroy(void);
void NewGameDlgSetData(DefPartida *dp);
void NewGameDlgGetData(DefPartida *dp);
int NewGameDlgRun(void);

int GameStatDlgCreate(void);
void GameStatDlgDestroy(void);
void GameStatDlgSetData(PosPartida *pp);
int GameStatDlgRun(void);

int GlbStatDlgRun(void);

int SettingsDlgRun(void);

#define HELP_SRULES 0
#define HELP_PRULES 1
#define HELP_GINSTR 2

int HelpDlgRun(int hfile);

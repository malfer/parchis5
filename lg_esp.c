/*
 * lg_esp.c --- Spanish strings
 * 
 * Copyright (c) 2020-2024 Mariano Alvarez Fernandez
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
    "Vale",
    "Cancelar",
    "Si",
    "No",
    "On",
    "Off"};

static char *smp[9] = {
    "     PARCHIS v5.2, MENÚ PRINCIPAL",
    "\x01\x02 &Nueva partida",
    "\x03\x04 &Estadísticas de la partida",
    "\x05\x06 &Velocidad de juego",
    "\x07\x08 &Ajustes globales",
    "\x09\x0a Es&tadísticas globales",
    "\x0b\x0c Ay&uda",
    "   &Pruebas",
    "\x0d\x0e &Salir"};

static char *smv[5] = {
    "&Lenta",
    "&Normal",
    "&Rápida",
    "&Muy rápida",
    "&Instantánea"};

static char *smt[8] = {
    "&Test fichas",
    "Gira &izquierda 90º lento",
    "Gira &derecha 90º rápido",
    "&Guarda partida",
    "&Carga partida",
    "&Autorepetir partida",
    "I&mprime análisis",
    "Im&prime movimientos"};

static char *smh[4] = {
    "&Reglas juego individual",
    "R&eglas juego por parejas",
    "&Instrucciones del juego",
    "&Acerca de"};

static char *smain[28] = {
    "Menú",
    "Partida en pausa",
    "Continuar",
    "Lanza el dado",
    "LANZAR",
    "No puedes mover",
    "PASAR",
    "Selecciona la ficha",
    "y mueve",
    "MOVER",
    "SELECCIONAR",
    "Vaya, tres seises",
    "vas a casa",
    "Mueves",
    "Fin de partida",
    "Lanzo el dado",
    "No puedo mover",
    "Ops!, tres seises",
    "voy a casa",
    "Muevo",
    "Pausa",
    "Por favor, dame",
    "un valor de dado",
    "Moviola",
    "Rep. Partida",
    "Reprod. moviola",
    "Moviola en pausa",
    "Parar"};

static char *sdex[2] = {
    "Salir",
    "¿Quieres guardar la partida antes de salir?"};

static char *sdsv[3] = {
    "Continuar partida",
    "Hay una partida guardada,",
    "¿quieres continuarla?"};

static char *sdst[14] = {
    "\x07\x08 Ajustes globales",
    "Pequeño 640x480",
    "Mediano1 800x600",
    "Mediano2 1024x768",
    "Defecto 1150x950",
    "Pantalla completa",
    "Básico",
    "Mejorado",
    "Mármol",
    "Dimensiones gráficas",
    "Idioma",
    "Imagen del tablero",
    "Mostrar barreras",
    "Avanzado"};

static char *sdast[3] = {
    "\x07\x08 Ajustes globales avanzados",
    "No redmnsnr si Dgra > Defecto",
    "Activar opciones de pruebas"};

static char *sdab[6] = {
    "Acerca de Parchís v5.2",
    "(C)2020-2024 Mariano Alvarez",
    "El parchís es un juego de mesa muy popular en España",
    "Parchís v5.2 es Software Libre,",
    "utiliza la librería gráfica MGRX y GrGUI",
    "lea el fichero readme.txt para mas detalles"};

static char *sdng[12] = {
    "\x01\x02 Nueva partida",
    "Individual",
    "Por parejas",
    "H",
    "R0",
    "R1",
    "R2",
    "R3",
    "R4",
    "NJ",
    "   H=Humano, Rx=Robot nivel x, NJ=No Juega",
    "Opciones"};

static char *sdgo[6] = {
    "\x01\x02 Opciones de partida",
    "Quién empieza",
    "Final rápido",
    "Final rápido de robots",
    "Final súbito",
    "Dado manual"};
    
static char *sdes[2] = {
    "\x03\x04 Estadísticas de la partida",
    "   \x0f\x10   \x11\x12   \x13\x14   \x15\x16   \x17\x18"
    "   \x19\x1a    Pasa   Come Comida  ACasa"};

static char *sdge[9] = {
    "\x09\x0a Estadísticas globales",
    "Individual",
    "Parejas",
    "partidas jugadas",
    " última",
    "Dado manual",
    "Dado automático",
    "H",
    "R"};

static char *sdhl[3] = {
    "\x0b\x0c Reglas juego individual",
    "\x0b\x0c Reglas juego por parejas",
    "\x0b\x0c Instrucciones del juego"};

void AddEspStrings(int lid)
{
    GrI18nSetLabel(lid, "Español");
    GrI18nAddStrings(lid, SGN_OK, 6, (void **)sgn);
    GrI18nAddStrings(lid, SMP_TITLE, 9, (void **)smp);
    GrI18nAddStrings(lid, SMV_SLOW, 5, (void **)smv);
    GrI18nAddStrings(lid, SMT_TPAWN, 8, (void **)smt);
    GrI18nAddStrings(lid, SMH_SRULES, 4, (void **)smh);
    GrI18nAddStrings(lid, SMAIN_MENU, 28, (void **)smain);
    GrI18nAddStrings(lid, SDEX_TITLE, 2, (void **)sdex);
    GrI18nAddStrings(lid, SDSV_TITLE, 3, (void **)sdsv);
    GrI18nAddStrings(lid, SDST_TITLE, 14, (void **)sdst);
    GrI18nAddStrings(lid, SDAST_TITLE, 3, (void **)sdast);
    GrI18nAddStrings(lid, SDAB_TITLE, 6, (void **)sdab);
    GrI18nAddStrings(lid, SDNG_TITLE, 12, (void **)sdng);
    GrI18nAddStrings(lid, SDGO_TITLE, 6, (void **)sdgo);
    GrI18nAddStrings(lid, SDES_TITLE, 2, (void **)sdes);
    GrI18nAddStrings(lid, SDGE_TITLE, 9, (void **)sdge);
    GrI18nAddStrings(lid, SDHL_TSRUL, 3, (void **)sdhl);
}

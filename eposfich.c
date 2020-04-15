/*
 * eposfich.c --- Parchis engine, PosFicha functions
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

int corrent[4] = { 4,21,38,55 };
int corrsal[4] = { 67,16,33,50 };

void PFSetPosition(PosFicha *pf, int place, int pos, int ind)
{
    pf->place = place;
    pf->pos = pos;
    pf->ind = ind;
}

void PFCopyPosition(PosFicha *pfd, PosFicha *pfs)
{
    *pfd = *pfs;
}


/*
 * Symbol-cache (binary file) handling
 *
 *              _______          _____ ___        ______
 *                 |      ||    |         |    | |      |
 *                 |      ||    |         |    | |      |
 *                 |   ___||___ |         |___ | |______|
 *
 *            Copyright (C) 2020-2022 AiO Secure Teletronics
 *
 * Tarsio is able to store parsed information originating from the design
 * under test into a binary file. It's basically the simplest form of
 * serialization, to be able to reuse and pass that informatino betweeen the
 * diffrent Tarsio tools.
 *
 *  This file is part of Tarsio.
 *
 *  Tarsio is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  Tarsio is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with Tarsio.  If not, see <https://www.gnu.org/licenses/>.
 *
 */

#ifndef _SYMBOL_CACHE_H_
#define _SYMBOL_CACHE_H_

#include "prototype.h"

unsigned char* reload_symbol_cache(prototype_list_t* list, const char* file);

#endif

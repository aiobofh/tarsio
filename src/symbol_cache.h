/*
 * Symbol-cache (binary file) handling
 *
 *              _______          _____ ___        ______
 *                 |      ||    |         |    | |      |
 *                 |      ||    |         |    | |      |
 *                 |   ___||___ |         |___ | |______|
 *
 *                   Copyleft AiO Secure Teletronics
 *
 * Tarsio is able to store parsed information originating from the design
 * under test into a binary file. It's basically the simplest form of
 * serialization, to be able to reuse and pass that informatino betweeen the
 * diffrent Tarsio tools.
 */

#ifndef _SYMBOL_CACHE_H_
#define _SYMBOL_CACHE_H_

#include "prototype.h"

int reload_symbol_cache(prototype_list_t* list, const char* file);

#endif

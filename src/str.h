/*
 * String helpers
 *
 *              _______          _____ ___        ______
 *                 |      ||    |         |    | |      |
 *                 |      ||    |         |    | |      |
 *                 |   ___||___ |         |___ | |______|
 *
 *                   Copyleft AiO Secure Teletronics
 *
 * These string helpers are mainly written to make Tarsio a bit more portable,
 * since some string operations of the Posix standard (however convenient) are
 * not available on other systems.
 */

#ifndef _STR_H_
#define _STR_H_

char* strclone(const char* src);

#endif

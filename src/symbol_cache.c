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

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "debug.h"
#include "error.h"

#include "symbol_cache.h"

#include "prototype.h"

/****************************************************************************
 * Deseriliazation
 */
static size_t fsize(FILE *fd) {
  size_t file_size;
  (void)fseek(fd, 0L, SEEK_END);
  file_size = ftell(fd);
  fseek(fd, 0L, SEEK_SET);
  return file_size;
}

static void transform_structs(prototype_list_t* list, char* buf) {
  prototype_list_t* l;
  prototype_node_t* n;
  prototype_node_t* pn = NULL;
  size_t i;
  size_t j;
  size_t k;

  if (('T' != buf[0]) || ('C' != buf[1]) || ('H' != buf[2]) || ('1' != buf[3])) {
    error0("Wrong symbol cache file format");
    return;
  }

  debug0("Found valid header TCH1 in first 4 bytes of the file");

  buf += 4;

  l = (prototype_list_t*)buf;

  /* Move offset into the first node */
  buf += sizeof(*l);

  if (0 == l->cnt) {
    l->first = l->last = NULL;
  }
  else {
    l->first = (prototype_node_t*)buf;
  }

  debug1("Transforming prototype list pointer to first node %lu nodes available", l->cnt);

  for (i = 0; i < l->cnt; i++) {

    argument_list_t* al;
    symbol_usage_list_t* sl;
    argument_node_t* an = NULL;
    symbol_usage_node_t* sn = NULL;

    pn = (prototype_node_t*)buf;

    if (NULL == pn) {
      error0("Prototype node was NULL in file, regardles of the number of nodes in list");
    }

    al = &pn->info.argument_list;
    sl = &pn->info.symbol_usage_list;

    buf += sizeof(*pn);

    /*
    debug4(" Transforming argument list pointer to first node %s %lu %lu arguments %p", pn->info.symbol, pn->info.is_function_implementation, pn->info.argument_list.cnt, al);
    */
    if (0 == pn->info.argument_list.cnt) {
      al->first = al->last = NULL;
    }
    else {
      al->first = (argument_node_t*)buf;
    }
    an = NULL;
    debug2("  first: %p last: %p", al->first, al->last);
    for (j = 0; j < pn->info.argument_list.cnt; j++) {
      debug1("  Handling argument %lu", j);
      an = (argument_node_t*)buf;
      /*
      debug4("  %p %s %s %d", an, an->info.datatype.name, an->info.name, an->info.datatype.datatype_definition.is_pointer);
      */
      buf += sizeof(*an);
      an->next = (argument_node_t*)buf;
      al->last = an;
    }
    if (NULL != an) {
      an->next = NULL;
    }

    debug0(" DONE");

    /*
    debug1(" Transforming symbol usage list pointer to first node %lu usages", pn->info.symbol_usage_list.cnt);
    */
    if (0 == pn->info.symbol_usage_list.cnt) {
      sl->first = sl->last = NULL;
    }
    else {
      sl->first = (symbol_usage_node_t*)buf;
    }
    sn = NULL;
    for (k = 0; k < pn->info.symbol_usage_list.cnt; k++) {
      sn = (symbol_usage_node_t*)buf;
      /*
      debug4("  %p line: %lu col: %lu offset: %lu", sn, sn->info.line, sn->info.col, sn->info.offset);
      */
      /*
      fprintf(stderr, "DEBUG: REREAD %s usage %p line: %lu col: %lu offset: %lu must be extern before %lu\n", pn->info.symbol, sn, sn->info.line, sn->info.col, sn->info.offset, sn->info.last_function_start);
      */
      buf += sizeof(*sn);
      sn->next = (symbol_usage_node_t*)buf;
      sn->info.prototype_node = pn; /* IS THIS WORKING???? */

      sl->last = sn;
    }
    if (NULL != sn) {
      sn->next = NULL;
    }

    debug0(" DONE");

    pn->next = (prototype_node_t*)buf;
    l->last = pn;

    debug0("Fetching next symbol");
  }

  debug0("All symbols processed");

  if (NULL == pn) {
    return;
  }

  pn->next = NULL;

  *list = *l;

  debug0("Setting up all string pointers to strings, located at the end");

  for (n = list->first; NULL != n; n = n->next) {
    n->info.datatype.name = buf;
    buf += strlen(n->info.datatype.name) + 1;
    n->info.symbol = buf;
    buf += strlen(n->info.symbol) + 1;
    debug2(" '%s' '%s'", n->info.datatype.name, n->info.symbol);
  }

  for (n = list->first; NULL != n; n = n->next) {
    argument_node_t* an;
    debug2("Finding usages for '%s' %li", n->info.symbol, n->info.argument_list.cnt);
    if (0 == n->info.argument_list.cnt) {
      n->info.argument_list.first = NULL;
      n->info.argument_list.last = NULL;
    }
    for (an = n->info.argument_list.first; NULL != an; an = an->next) {
      an->info.name = buf;
      buf += strlen(an->info.name) + 1;
      an->info.datatype.name = buf;
      buf += strlen(an->info.datatype.name) + 1;
      if (0 == strlen(an->info.name)) {
        an->info.name = NULL;
      }
      if (0 == strlen(an->info.datatype.name)) {
        an->info.datatype.name = NULL;
      }
      debug2(" '%s' '%s'", an->info.datatype.name, an->info.name);
    }
  }
}

int reload_symbol_cache(prototype_list_t* list, const char* file) {
  int retval = 0;
  FILE* fd;
  size_t len;
  char* buf;

  if (NULL == (fd = fopen(file, "rb"))) {
    error1("Unable to open '%s' for reading", file);
    retval = -1;
    goto fopen_failed;
  }

  len = fsize(fd);

  if (NULL == (buf = malloc(len))) {
    error1("Out of memory while allocating '%s'", file);
    retval = -1;
    goto malloc_failed;
  }

  if (1 != fread(buf, len, 1, fd)) {
    error1("Unable to read '%s'", file);
    retval = -2;
    goto fread_failed;
  }

  fclose(fd);

  debug2("Loaded %s (%lu bytes)", file, len);

  transform_structs(list, buf);

  goto normal_exit;

 fread_failed:
  free(buf);
 malloc_failed:
  fclose(fd);
 fopen_failed:
 normal_exit:
  return retval;
}

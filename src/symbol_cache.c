#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "error.h"

#include "symbol_cache.h"

#include "prototype.h"

/****************************************************************************
 * Deseriliazation
 */
static size_t fsize(FILE *fd) {
  size_t file_size = 0;
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

  if (('T' != buf[0]) || ('C' != buf[1]) || ('H' != buf[2]) || ('1' != buf[3])) {
    error0("Wrong symbol cache file format");
    return;
  }

  buf += 4;

  l = (prototype_list_t*)buf;
  l->first = NULL;
  l->last = NULL;

  /* Move offset into the first node */
  buf += sizeof(*l);
  l->first = (prototype_node_t*)buf;


  for (i = 0; i < l->cnt; i++) {
    argument_list_t* al;
    symbol_usage_list_t* sl;
    argument_node_t* an = NULL;
    symbol_usage_node_t* sn = NULL;

    pn = (prototype_node_t*)buf;
    al = &pn->info.argument_list;
    sl = &pn->info.symbol_usage_list;

    buf += sizeof(*pn);

    al->first = (argument_node_t*)buf;
    for (j = 0; j < pn->info.argument_list.cnt; j++) {
      an = (argument_node_t*)buf;
      buf += sizeof(*an);
      an->next = (argument_node_t*)buf;
      al->last = an;
    }
    an->next = NULL;

    sl->first = (symbol_usage_node_t*)buf;
    for (j = 0; j < pn->info.symbol_usage_list.cnt; j++) {
      sn = (symbol_usage_node_t*)buf;
      buf += sizeof(*sn);
      sn->next = (symbol_usage_node_t*)buf;
      sl->last = sn;
    }
    sn->next = NULL;

    pn->next = (prototype_node_t*)buf;
    l->last = pn;
  }

  pn->next = NULL;

  *list = *l;

  for (n = list->first; NULL != n; n = n->next) {
    n->info.datatype.name = buf;
    buf += strlen(n->info.datatype.name) + 1;
    n->info.symbol = buf;
    buf += strlen(n->info.symbol) + 1;
  }

  for (n = list->first; NULL != n; n = n->next) {
    argument_node_t* an;
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

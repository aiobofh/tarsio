#ifndef _CPP_H_
#define _CPP_H_

struct cpp_s {
  char* directive;
};
typedef struct cpp_s cpp_t;

#define CPP_EMPTY {NULL}

struct cpp_node_s {
  struct cpp_node_s* next;
  cpp_t info;
};
typedef struct cpp_node_s cpp_node_t;

#define CPP_NODE_EMPTY {NULL, CPP_EMPTY}

struct cpp_list_s {
  size_t cnt;
  cpp_node_t* first;
  cpp_node_t* last;
};
typedef struct cpp_list_s cpp_list_t;

#define CPP_LIST_EMPTY {0, NULL, NULL}

int cpp_list_init(cpp_list_t* list, const file_t* file);
void cpp_list_cleanup(cpp_list_t* list);

#endif

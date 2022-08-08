#include "node.h"
#include "helpers.h"

__node_t* _node_malloc(size_t size) {
  enum {
    ok = 0,
    malloc_failed = -1
  } retval = ok;
  static const __node_t default_node = {NODE_INIT};
  __node_t* node;

  assert(0 != size && "Can not allocate zero size");

  node = smalloc(size) else ret(malloc_failed);

  *node = default_node;
 malloc_failed:
  return (ok == retval) ? node : NULL;
}

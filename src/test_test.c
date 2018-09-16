#include <tarsio.h>

test(foo_should_output_n_times) {
  const int n = 10;
  foo(n);
  assert_eq(n, tarsio_mock.fprintf.call_count);
}

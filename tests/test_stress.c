/*
 * libusbx stress test program to perform simple stress tests
 * Copyright © 2012 Toby Gray <toby.gray@realvnc.com>
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA
 */

#include <stdio.h>
#include <sys/types.h>

#include "libusb.h"

#include "libusbx_testlib.h"

/** Test that creates and destroys a single concurrent context
 * 10000 times. */
static libusbx_testlib_result test_init_and_exit(libusbx_testlib_ctx * tctx)
{
  libusb_context * ctx = NULL;
  int i;
  for (i = 0; i < 10000; ++i) {
    int r = libusb_init(&ctx);
    if (r != LIBUSB_SUCCESS) {
      libusbx_testlib_logf(tctx,
                           "Failed to init libusb on iteration %d: %d",
                           i, r);
      return TEST_STATUS_FAILURE;
    }
    libusb_exit(ctx);
    ctx = NULL;
  }

  return TEST_STATUS_SUCCESS;
}

/* Fill in the list of tests. */
static const libusbx_testlib_test tests[] = {
  {"init_and_exit", &test_init_and_exit},
  LIBUSBX_NULL_TEST
};

int main (int argc, char ** argv)
{
  return libusbx_testlib_run_tests(argc, argv, tests);
}

/*
 * libusbx test library helper functions
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

#include "libusbx_testlib.h"

#include <stdio.h>
#include <stdarg.h>
#include <string.h>

/**
 * Converts a test result code into a human readable string.
 */
static const char* test_result_to_str(libusbx_testlib_result result)
{
  switch (result) {
  case TEST_STATUS_SUCCESS:
    return "Success";
  case TEST_STATUS_FAILURE:
    return "Failure";
  case TEST_STATUS_ERROR:
    return "Error";
  case TEST_STATUS_SKIP:
    return "Skip";
  default:
    return "Unknown";
  }
}

static void print_usage(int argc, char ** argv)
{
	printf("Usage: %s [-l] [<test_name> ...]\n",
		argc > 0 ? argv[0] : "test_*");
	printf("   -l   List available tests\n");
}

void libusbx_testlib_logf(libusbx_testlib_ctx * ctx,
                          const char* fmt, ...)
{
  va_list va;
  (void)ctx;
  va_start(va, fmt);
  vprintf(fmt, va);
  va_end(va);
  printf("\n");
}

int libusbx_testlib_run_tests(int argc,
                              char ** argv,
                              const libusbx_testlib_test * tests)
{
  int run_count = 0;
  int idx = 0;
  int pass_count = 0;
  int fail_count = 0;
  int error_count = 0;
  int skip_count = 0;
  int j;
  size_t arglen;
  libusbx_testlib_result test_result;
  libusbx_testlib_ctx ctx;

  /* Setup default mode of operation */
  ctx.test_names = NULL;
  ctx.test_count = 0;
  ctx.list_tests = false;

  /* Parse command line options */
  if (argc >= 2) {
    for (j = 1; j < argc; j++) {
      arglen = strlen(argv[j]);
      if ( ((argv[j][0] == '-') || (argv[j][0] == '/')) &&
	   arglen >=2 ) {
	switch (argv[j][1]) {
	case 'l':
	  ctx.list_tests = true;
	  break;
	default:
	  printf("Unknown option: '%s'\n", argv[j]);
	  print_usage(argc, argv);
	  return 1;
	}
      } else {
	/* End of command line options, remaining must be list of tests to run */
	ctx.test_names = argv + j;
	ctx.test_count = argc - j;
	break;
      }
    }
  }

  /* Act on any options not related to running tests */
  if (ctx.list_tests) {
    if (ctx.test_names) {
      printf("List of tests requested but test list provided\n");
      print_usage(argc, argv);
      return 1;
    }
    while (tests[idx].function != NULL) {
      libusbx_testlib_logf(&ctx, tests[idx].name);
      ++idx;
    }
    return 0;
  }

  /* Run any requested tests */
  while (tests[idx].function != NULL) {
    const libusbx_testlib_test * test = &tests[idx];
    ++idx;
    if (ctx.test_count > 0) {
      /* Filtering tests to run, check if this is one of them */
      int i;
      for (i = 0; i < ctx.test_count; ++i) {
	if (strcmp(ctx.test_names[i], test->name) == 0)
	  /* Matches a requested test name */
	  break;
      }
      if (i >= ctx.test_count) {
	/* Failed to find a test match, so do the next loop iteration */
	continue;
      }
    }
    libusbx_testlib_logf(&ctx,
                         "Starting test run: %s...", test->name);
    test_result = test->function(&ctx);
    libusbx_testlib_logf(&ctx,
                         "%s (%d)",
                         test_result_to_str(test_result), test_result);
    switch (test_result) {
    case TEST_STATUS_SUCCESS: pass_count++; break;
    case TEST_STATUS_FAILURE: fail_count++; break;
    case TEST_STATUS_ERROR: error_count++; break;
    case TEST_STATUS_SKIP: skip_count++; break;
    }
    ++run_count;
  }
  libusbx_testlib_logf(&ctx, "---");
  libusbx_testlib_logf(&ctx, "Ran %d tests", run_count);
  libusbx_testlib_logf(&ctx, "Passed %d tests", pass_count);
  libusbx_testlib_logf(&ctx, "Failed %d tests", fail_count);
  libusbx_testlib_logf(&ctx, "Error in %d tests", error_count);
  libusbx_testlib_logf(&ctx, "Skipped %d tests", skip_count);
  return pass_count != run_count;
}

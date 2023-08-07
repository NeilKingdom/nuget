#include <stdio.h>
#include <stdlib.h>
#include <check.h>

#include "display.h"

START_TEST(test1)
{

}
END_TEST

int run_testsuite()
{
   int fail_nr;
   Suite *s;
   SRunner *sr;

   s = test_suite();
   sr = srunner_create(s);

   srunner_run_all(sr, CK_NORMAL);
   fail_nr = srunner_ntests_failed(sr);
   srunner_free(sr);

   printf("%s\n", fail_nr ? "At least one test failed" : "All tests passed");

   return(!fail_nr ? EXIT_SUCCESS : EXIT_FAILURE);
}

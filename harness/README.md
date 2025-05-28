Just a quick tweak of the public test handler. All rights reserved to whomever wrote it.
Installation:
1) (recommended) Rename the folder something less unwieldy (e.g. "harness").
2) Run the install.sh script (e.g. ./harness/install.sh) in order to chmod all of the Perl tests so you have execute rights.
3) Everything should be all set to go - use the run_tests.pl command to run all tests (e.g. "./harness/run_tests.pl") or run one test
by running, for example, "./harness/customTests/t01".
After adding a test, 
1) Run the install.sh script to ensure you have execute rights on the new test.
2) Add the test and the description to the main runner (run_tests.pl). 
3) 
-Shlomi

#!/bin/bash
chmod +x ./harness/runner.pl
chmod +x ./harness/run_tests.pl

BASE_DIR="./harness/customTests"
find "$BASE_DIR" -type f \( -name 't[0-9][0-9]' -o -name 't[0-9][0-9].pl' \) -exec chmod +x {} \;

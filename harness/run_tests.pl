#!/usr/bin/perl -w
use strict;
use Cwd qw();

my $path = Cwd::cwd();

our $debug = 0;
our $CLASS  = "323";
our $PROJ   = "1";
our $NAME   = "proj1";           # Name of program
our $LANG   = "C";               # Language (C, Perl, ...)
our $CLASS_DIR = "/c/cs323";
our $PROJ_DIR        = "$CLASS_DIR/proj$PROJ";
our $TESTS_DIR       = "$path/harness/customTests";
our $CLASS_BIN_DIR   = "$CLASS_DIR/bin";

# Common test functions and actions.
do "$path/harness/runner.pl";

&limitCpuTime (2, 4);                           # Limit CPU-time per process
&limitWallClock (10);                           # Limit wall-clock per process
&limitFileSize (100000);                        # Limit size of files created
&limitHeapSize (1000000);                       # Limit size of heap
&limitProcesses (1000);                         # Limit #processes

my $total = 0;
my $amt = 0;

&header ("Errors");
$amt += &runTest ("01", "Missing 'then' after 'if'");
$total++;
$amt += &runTest ("02", "Missing 'else' after 'if'");
$total++;
$amt += &runTest ("03", "Errors: Macro name not alphanumeric");
$total++;
$amt += &runTest ("04", "Errors: Empty def");
$total++;
$amt += &runTest ("05", "Errors: Undef without def");
$total++;
$amt += &runTest ("06", "Errors: Unbalanced if condition");
$total++;
$amt += &runTest ("07", "Errors: Undef without def");
$total++;
$amt += &runTest ("08", "Errors: Redefine macro");
$total++;
$amt += &runTest ("09", "Def: No replacement");
$total++;
$amt += &runTest ("10", "Expandafter: escape, fragment");
$total++;
$amt += &runTest ("11", "Comment in middle of value");
$total++;
$amt += &runTest ("12", "Escape followed by non-alphanumeric");
$total++;
$amt += &runTest ("13", "Circular macro def");
$total++;
#$amt += &runTest ("14", "Really long with a macro inside");
#$total++;
$amt += &runTest ("15", "Just a backspace");
$total++;

print "\nEnd custom tests!\n";

printf ("\n%3d out of %3d points Total for $NAME\n", $amt, $total);


exit $amt;

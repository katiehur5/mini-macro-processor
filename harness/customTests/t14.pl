#!/usr/bin/perl
#
#   
#
#  Very long input with (almost!) no macros

$length = (defined $ARGV[0]) ? $ARGV[0] : 1;

$length /= 2;

$longInput = "a" x $length . "\def{macro}{hello}" . "a" x $length . "\macro{}";
print "$longInput\n";
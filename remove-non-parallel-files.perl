#!/usr/bin/perl -w

use strict;
use Encode;

binmode(STDIN, ":utf8");
binmode(STDOUT, ":utf8");
binmode(STDERR, ":utf8");

my ($l1,$l2) = @ARGV;
die unless -e "./$l1";
die unless -e "./$l2";

my ($dayfile,$s1); 
open(LS,"ls ./$l1|");
while($dayfile = <LS>) {
  chop($dayfile);
  if (! -e "./$l2/$dayfile") {
    print "$dayfile only for $l1, not $l2, removing\n";
    system("rm ./$l1/$dayfile"); 
  }
}

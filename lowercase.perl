#!/usr/bin/perl -w

use strict;
use Term::ReadLine;

binmode(STDIN, ":utf8");
binmode(STDOUT, ":utf8");

while(<STDIN>) {
	if (/^<.+>$/ || /^\s*$/) {#don't lowercase XML/HTML tag lines

		print $_;
	}
	else {
		print lc($_);
	}
}


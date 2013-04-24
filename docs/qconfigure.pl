#!/usr/bin/perl
#
# Copyright (C) 2013 VATSIM community / contributors
# This Source Code Form is subject to the terms of the Mozilla Public
# License, v. 2.0. If a copy of the MPL was not distributed with this
# file, You can obtain one at http://mozilla.org/MPL/2.0/
#
# Script used by the qmake project file doxygen.pro to process the file
# Doxyfile.cmake.in

use strict;
use warnings;

die "not enough arguments\n" if @ARGV < 2;

my ($infile, $outfile, @vars) = @ARGV;

my %vars;
for my $line (@vars)
{
	my ($var, $val) = split '=', $line;
	$vars{$var} = $val;
	
	die "missing '=' in parameter\n" unless defined $val;
}

$infile  =~ s.\\./.g;
$outfile =~ s.\\./.g;

open my $in, '<', $infile or die "couldn't read from $infile\n";
open my $out, '>', $outfile or die "couldn't write to $outfile\n";

while (defined(my $line = <$in>))
{
	for my $var (keys %vars)
	{
		$line =~ s.\@$var\@.$vars{$var}.g;
	}
	print $out $line;
}

close $in;
close $out;
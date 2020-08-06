#!/usr/bin/perl
#
# Convert C++ compiler warnings into Github Actions workflow commands
#
# Copyright (C) 2020 swift Project Community / contributors
#
# This program is free software: you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation, either version 3 of the License, or
# (at your option) any later version.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.

use strict;
use warnings;
use Path::Class;

my %cmds;
while (<<>>)
{
    chomp;
    my %result = extractWarning($_);
    next if not %result;
    $result{file} = toRelativePath($result{file})->as_foreign('Unix');
    if (exists $result{line}) {
        ++$cmds{"::warning file=$result{file},line=$result{line}::$result{msg}"};
    } else {
        ++$cmds{"::warning file=$result{file}::$result{msg}"};
    }
}
my $bool = %cmds ? 'true' : 'false';
print "$_\n" for sort keys %cmds;
print "::set-output name=warnings::$bool\n";

sub extractWarning
{
    my $line = shift;
    my %result = extractGccWarning($line);
    return %result if %result;
    return extractMsvcWarning($line);
}

sub extractGccWarning
{
    my $line = shift;
    $line =~ m(^\s*(?<loc>.*?): warning: (?<msg>.*)$) or return ();
    my ($loc, $msg) = @+{ qw(loc msg) };
    $loc =~ m(^(?<file>.*?):(?<loc>[0-9:]+)$) or return (file => $loc, msg => $msg);
    my ($file, $line) = ($+{file}, split ':', $+{loc});
    return (file => $file, line => $line, msg => $msg);
}

sub extractMsvcWarning
{
    my $line = shift;
    $line =~ m(^\s*(?<loc>.*?): warning (?<msg>[A-Z]+[0-9]+: .*)$) or return ();
    my ($loc, $msg) = @+{ qw(loc msg) };
    $loc =~ m(^(?<file>.*)\((?<loc>[0-9,]+)\)$) or return (file => $loc, msg => $msg);
    my ($file, $line) = ($+{file}, split ',', $+{loc});
    return (file => $file, line => $line, msg => $msg);
}

sub toRelativePath
{
    my $path = shift;
    my $file = file($path);
    return $file if $file->is_relative;
    return $file->relative if dir()->absolute()->subsumes($file);
    return $file;
}

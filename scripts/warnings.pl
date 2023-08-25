#!/usr/bin/perl
#
# Convert C++ compiler warnings into Github Actions workflow commands
#
# SPDX-FileCopyrightText: Copyright (C) 2020 swift Project Community / contributors
# SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

use strict;
use warnings;
use File::Spec;
use Cwd;

my %cmds;
while (<<>>)
{
    chomp;
    my %result = extractWarning($_);
    next if not %result;
    $result{file} = toRelativePath($result{file});
    $result{file} =~ s(\\)(/)g if $^O eq 'MSWin32';
    if (exists $result{line}) {
        ++$cmds{"::warning file=$result{file},line=$result{line}::$result{msg}"};
    } else {
        ++$cmds{"::warning file=$result{file}::$result{msg}"};
    }
}
print "$_\n" for sort keys %cmds;

my $bool = %cmds ? 'true' : 'false';
open my $GITHUB_OUTPUT, ">> $ENV{GITHUB_OUTPUT}" or die "open: $!";
print $GITHUB_OUTPUT "warnings=$bool\n";

sub extractWarning
{
    my $text = shift;
    my %result = extractGccWarning($text);
    return %result if %result;
    return extractMsvcWarning($text);
}

sub extractGccWarning
{
    my $text = shift;
    $text =~ m(^\s*(?<loc>.*?): warning: (?<msg>.*)$) or return ();
    my ($loc, $msg) = @+{ qw(loc msg) };
    $loc =~ m(^(?<file>.*?):(?<loc>[0-9:]+)$) or return (file => $loc, msg => $msg);
    my ($file, $line) = ($+{file}, split ':', $+{loc});
    return (file => $file, line => $line, msg => $msg);
}

sub extractMsvcWarning
{
    my $text = shift;
    $text =~ m(^\s*(?<loc>.*?): warning (?<msg>[A-Z]+[0-9]+: .*)$) or return ();
    my ($loc, $msg) = @+{ qw(loc msg) };
    $loc =~ m(^(?<file>.*)\((?<loc>[0-9,]+)\)$) or return (file => $loc, msg => $msg);
    my ($file, $line) = ($+{file}, split ',', $+{loc});
    return (file => $file, line => $line, msg => $msg);
}

sub toRelativePath
{
    my $path = shift;
    return $path if not File::Spec->file_name_is_absolute($path);
    return File::Spec->rel2abs($path) if index($path, getcwd()) == 0;
    return $path;
}

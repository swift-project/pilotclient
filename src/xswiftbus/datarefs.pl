#!/usr/bin/perl
#
# Script to generate C++ traits classes for all X-Plane datarefs.
#
# Copyright (C) 2014 swift Project Community / contributors
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

sub usage
{
    print STDERR <<"EOF";
This program generates C++ traits classes that describe the data refs of the
X-Plane Plugin SDK.
Usage:
    $0 <XPLANEPATH>
Arguments:
    <XPLANEPATH> - directory in which X-Plane is installed
EOF
    exit 1;
}

@ARGV == 1 or usage();

my $filename = "$ARGV[0]/Resources/plugins/DataRefs.txt";

my $fh;
unless (open $fh, '<', $filename)
{
    print STDERR "$filename: $!\n";
    usage();
}

our @keywords = qw(
    alignas alignof and and_eq asm atomic_cancel atomic_commit atomic_noexcept
    auto bitand bitor bool break case catch char char8_t char16_t char32_t class
    compl concept const consteval constexpr constinit const_cast continue
    co_await co_return co_yield decltype default delete do double dynamic_cast
    else enum explicit export extern false float for friend goto if inline int
    long mutable namespace new noexcept not not_eq nullptr operator or or_eq
    private protected public reflexpr register reinterpret_cast requires return
    short signed sizeof static static_assert static_cast struct switch
    synchronized template this thread_local throw true try typedef typeid
    typename union unsigned using virtual void volatile wchar_t while xor xor_eq
);

my %hierarchy;

while (<$fh>)
{
    chomp;
    next unless m"\t";

    my @fields = split "\t";
    next unless @fields >= 3;
  
    my($dataref, $type, $writable, $extra, $comment) = @fields;
    $writable = $writable eq 'y' ? 'true' : 'false';
    $comment ||= 'Undocumented dataref';
    my $size;
    $type =~ m"(\w+)\[(\d+)\]" and ($type, $size) = ($1, $2);
  
    my @namespaces = map sanitize($_), split '/', $dataref;
    my $class = pop @namespaces;
    $class =~ s(\])()g;
    $class =~ s(\[)(_)g;
    $class =~ s((plane\d+)_sla1_ratio)(${1}_slat_ratio);
    $class eq 'type' and $class .= '_';
  
    my $namespace = \%hierarchy;
    $namespace = ($namespace->{$_} ||= {}) foreach @namespaces;
  
    $namespace->{$class} = [$dataref, $type, $size, $writable, $extra, $comment];
}

close $fh;

our $indent = '    ';
print <<"EOF";
// -*- c++ -*-

// DO NOT EDIT
// This file automatically generated from DataRefs.txt by $0

//! X-Plane Plugin SDK C++ API
namespace xplane
{
${indent}//! X-Plane datarefs
${indent}namespace data
${indent}{
EOF

recurse(\%hierarchy, 2);

print <<"EOF";
${indent}}
}
EOF

sub recurse
{
    my($hash, $indentLevel) = @_;
  
    my $in = $indent x $indentLevel;
    foreach my $key (sort keys %$hash)
    {
        if (ref $hash->{$key} eq 'HASH')
        {
            print <<"EOF";
${in}//! $key datarefs
${in}namespace $key
${in}{
EOF

            recurse($hash->{$key}, $indentLevel + 1);
      
            print <<"EOF";
${in}}

EOF
        }
        else
        {
            my($name, $type, $size, $writable, $extra, $comment) = @{ $hash->{$key} };
      
            defined $extra and $extra ne '???' and $comment .= " ($extra)";
      
            print <<"EOF";
${in}//! $comment
${in}struct $key
${in}{
${in}${indent}//! Dataref name
${in}${indent}static constexpr const char *name() { return "$name"; }
${in}${indent}//! Can be written to?
${in}${indent}static constexpr bool writable = $writable;
EOF

            print <<"EOF" if $type =~ m(int|float|double);
${in}${indent}//! Dataref type
${in}${indent}using type = $type;
EOF

            print <<"EOF" if defined $size;
${in}${indent}//! Is an array dataref
${in}${indent}static constexpr bool is_array = true;
${in}${indent}//! Size of array dataref
${in}${indent}static constexpr size_t size = $size;
EOF

            print <<"EOF" unless defined $size;
${in}${indent}//! Not an array dataref
${in}${indent}static constexpr bool is_array = false;
EOF

            print <<"EOF";
${in}};

EOF
        }
    }
}

sub sanitize
{
    my ($name) = @_;
    $name .= '_' if grep $_ eq $name, @keywords;
    return $name;
}

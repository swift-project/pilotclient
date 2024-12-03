# SPDX-FileCopyrightText: Copyright (C) 2013 VATSIM Community / authors
# SPDX-License-Identifier: MPL-2.0
#
# Copyright (C) 2013 VATSIM Community / authors
# This Source Code Form is subject to the terms of the Mozilla Public License,
# version 2.0. If a copy of the MPL was not distributed with this file, you can
# obtain one at http://mozilla.org/MPL/2.0
#
# This program is also separately licensed under the same terms as Perl itself.
#
##############################################################################
#
# This package offers a procedural interface for specifying and running simple
# unit tests of interactive commandline programs, implemented using Expect.pm.
#

##
## All commandline arguments of the script are passed to Expect::spawn().
## Therefore the first argument must be the name of the program to be tested.
##
## Expect.pm depends on IO::Pty, which only supports true Unix operating
## systems. On Windows, this means Cygwin only. MinGW, ActiveState or
## Strawberry Perl will not work.
##
## put($lines...)
##   Prints $lines to stdin of the program under test.
##
## set_timeout($seconds)
##   Set the number of seconds to wait for expected responses before returning
##   an error.
##
## add_test($text, $patterns...)
##   Add a test.
##   $text : The text to print to stdin of the program under test.
##   $patterns : One or more patterns which are expected to sequentially match
##     the responses received from stdout of the program under test.
##
## add_ok($pattern)
##   Add a pattern which will always be ignored if it matches while waiting
##   for a response during a test.
##
## add_fail($pattern)
##   Add a pattern which will always cause a test to fail if it matches while
##   waiting for a response.
##
## run_tests()
##   Run all defined tests in sequence, and print the results.
##
## Patterns can be simple strings, or regex objects.
##

package SwiftCore::Expect::Test;
use strict;
use warnings;
use SwiftCore::Expect::Dict;
use Expect;
use IO::String;
use base 'Exporter';
our @EXPORT = qw(add_test add_ok add_fail run_tests put set_timeout);

@ARGV > 0 or _usage();
$ARGV[0] =~ m'^-' and _usage();

our $timeout = 10;
our $expect = Expect->new;
our $log = IO::String->new;
$expect->raw_pty(1);
$expect->log_stdout(0);
$expect->log_file($log);
$expect->spawn(@ARGV) or die "$!";
our $dict = SwiftCore::Expect::Dict->new($expect);

sub put
{
  print $log "$_\n" for @_;
  $expect->print("$_\n") for @_;
}

sub set_timeout
{
  $timeout = shift;
}

sub add_test
{
  my ($print_test, @pattern_seq) = @_;
  
  my @pattern_keys;
  foreach my $pattern (@pattern_seq) {
    push @pattern_keys, $dict->add($pattern, \&_test_fail, \&_test_ok);
  }
  
  chomp $print_test;
  $print_test =~ m'(\S+)\N*$';
  my $name = $1 || '';
  push our @tests, [ $name, $print_test, @pattern_keys ];
}

sub add_ok
{
  my ($pattern) = @_;
  
  $dict->add_enabled($pattern, (sub { exp_continue_timeout }) x 2);
}

sub add_fail
{
  my ($pattern) = @_;
  
  $dict->add($pattern, \&_test_fail, \&_test_fail);
}

sub run_tests
{
  our $passes = 0;
  our $fails = 0;
  my $total = 0;
  
  foreach my $test (our @tests)
  {
    my ($name, $print_test, @pattern_keys) = @$test;
    
    my $dots = '.' x (40 - length $name);
    
    print $log "$print_test\n";
    $expect->print("$print_test\n");
    foreach my $key (@pattern_keys) {
      $total++;
      print "Testing $name$dots";
      $dict->enable($key);
      my $pos = $dict->expect($timeout);
      $dict->disable($key);
      _check_result();
      defined $pos or print $expect->error . "\n";
    }
  }
  
  print "Log:\n" . ${ $expect->log_file->string_ref };
  
  my $errors = $total - $passes - $fails;
  print "\n$total TESTS, $passes PASSED, $fails FAILED, $errors ERRORS\n";
}

sub _test_fail
{
  our $test_status = 'fail';
  (our $fails)++;
}

sub _test_ok
{
  our $test_status = 'ok';
  (our $passes)++;
}

sub _check_result
{
  if (our $test_status) {
    print "$test_status\n";
    $test_status eq 'ok' or print "Received: " . $expect->match . "\n";
    undef $test_status;
  } else {
    print "error\n";
  }
}

sub _usage
{
  print "Usage:\n  $0 <program>\nRun tests on <program>.\n";
  exit;
}

1;

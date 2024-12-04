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
# This package offers an extra interface on top of Expect.pm, based on a
# dictionary of patterns which can be dynamically enabled and disabled.
#

##
## $dict = Dict->new($expect)
##   Constructor, returns a new Dict object.
##   $expect : The Expect object to be used.
##
## $key = $dict->add($pattern, $disabled_cb, $enabled_cb)
##   Adds a pattern to the dictionary, which will be initially disabled.
##   Returns a key object which can be used to refer to the pattern later.
##   $pattern : A simple string or regex object.
##   $disabled_cb : A callback to be called when the disabled pattern matches.
##   $enabled_cb  : A callback to be called when the enabled pattern matches.
##
## $key = $dict->add_enabled($pattern, $disabled_cb, $enabled_cb)
##   As add(), but the pattern will be initially enabled.
##
## $dict->enable($key)
##   Enable a pattern.
##   $key : As returned by add() or add_enabled().
##
## $dict->disable($key)
##   Disable a pattern.
##   $key : As returned by add() or add_enabled().
##
## $state = $dict->save_state()
##   Returns a representation of the current state of the dictionary, which
##   can be restored later.
##
## $dict->restore_state($state)
##   Revert the dictionary to the state represented by $state.
##
## $dict->expect($timeout)
##   Calls $expect->expect() with the patterns of the dictionary.
##   $timeout : Time to wait in seconds.

package SwiftCore::Expect::Dict;
use strict;
use warnings;

sub new
{
  my ($pkg, $expect) = @_;
  die unless $expect->isa('Expect');

  return bless {
    expect => $expect,
    patterns => {},
    enabled_patterns => {}
  } => $pkg;
}

sub add
{
  my ($self, $pattern, $disabled_cb, $enabled_cb) = @_;

  my $key = keys %{ $self->{patterns} };
  $self->{patterns}{$key} = [ $pattern, $disabled_cb, $enabled_cb ];
  return $key;
}

sub add_enabled
{
  my ($self, @args) = @_;

  my $key = $self->add(@args);
  $self->enable($key);
  return $key;
}

sub enable
{
  my ($self, $key) = @_;

  $self->{enabled_patterns}{$key} = 1;
}

sub disable
{
  my ($self, $key) = @_;

  delete $self->{enabled_patterns}{$key}
    if exists $self->{enabled_patterns}{$key};
}

sub save_state
{
  my ($self) = @_;

  return \%{ $self->{enabled_patterns} };
}

sub restore_state
{
  my ($self, $state) = @_;

  %{ $self->{enabled_patterns} } = %$state;
}

sub expect
{
  my ($self, $timeout) = @_;

  my @args;
  my $last_key = scalar(keys %{ $self->{patterns} }) - 1;
  foreach my $key (0..$last_key)
  {
    my $pattern = $self->{patterns}{$key}[0];
    push @args, [ $pattern, sub { return $self->_matched($key) } ];
  }

  my @result = $self->{expect}->expect($timeout, @args);

  if (wantarray) {
    return @result;
  } else {
    return $result[0];
  }
}

sub _matched
{
  my ($self, $key) = @_;

  if (exists $self->{enabled_patterns}{$key}) {
    return $self->{patterns}{$key}[2]();
  } else {
    return $self->{patterns}{$key}[1]();
  }
}

1;

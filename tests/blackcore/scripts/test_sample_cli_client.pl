#!/usr/bin/perl
#
# SPDX-FileCopyrightText: Copyright (C) 2013 VATSIM Community / authors
# SPDX-License-Identifier: MPL-2.0
#
# This Source Code Form is subject to the terms of the Mozilla Public License,
# version 2.0. If a copy of the MPL was not distributed with this file, you can
# obtain one at http://mozilla.org/MPL/2.0
#

use strict;
use warnings;
use lib::abs 'perllib';
use BlackCore::Expect::Test;

my $callsign = 'TEST01';
set_timeout(10);

add_test(<<"", 'CONN_STATUS_CONNECTING', 'CONN_STATUS_CONNECTED');
setserver vatsim-germany.org 6809
setuser guest guest
setrealname Pilot Client Tester
setcallsign $callsign
initconnect

add_test('ping server', qr'PONG server');

add_test('termconnect', 'CONN_STATUS_DISCONNECTED');

add_ok(qr"PRIV_MSG server -> $callsign");

add_fail('Cannot exec');

run_tests();

put('exit');
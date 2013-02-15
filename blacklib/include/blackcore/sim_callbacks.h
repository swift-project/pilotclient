//! Copyright (C) 2013 Roland Winklmeier
//! This Source Code Form is subject to the terms of the Mozilla Public
//! License, v. 2.0. If a copy of the MPL was not distributed with this
//! file, You can obtain one at http://mozilla.org/MPL/2.0/

#ifndef SIM_CALLBACKS_H
#define SIM_CALLBACKS_H

typedef void (* cbChangedRadioFreq)(const int radioNumber, const QString &frequency);
typedef void (* cbChangedGearPosition)(const int percentage);
typedef void (* cbChangedLights)(const int map);
typedef void (* cbSimStarted)(const bool status);
typedef void (* cbChangedAircraftType)(const QString &type);
typedef void (* cbChangedFlaps)(const int percentage);

#endif // CALLBACKS_H

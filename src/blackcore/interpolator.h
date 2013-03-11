//! Copyright (C) 2013 Roland Winklmeier
//! This Source Code Form is subject to the terms of the Mozilla Public
//! License, v. 2.0. If a copy of the MPL was not distributed with this
//! file, You can obtain one at http://mozilla.org/MPL/2.0/

#ifndef INTERPOLATOR_H
#define INTERPOLATOR_H

#include <QElapsedTimer>

#include "blackcore/vector_geo.h"
#include <blackcore/ecef.h>
#include "blackcore/vector_3d.h"
#include "blackcore/ned.h"
#include "blackcore/ecef.h"
#include "blackcore/constants.h"

namespace BlackCore
{

typedef struct
{
    double heading;
    double pitch;
    double bank;

} TOrientation;

typedef struct
{
    void reset()
    {
    }

    qint64 			timestamp;
    CEcef 			position;
    TOrientation 	orientation;
    double 			groundspeed;
    CVector3D 		velocity;
	CNed			velNED;

} TPlaneState;

class CInterpolator
{
public:
    CInterpolator();
	virtual ~CInterpolator();

    void initialize();

    void pushUpdate(CVectorGeo pos, double groundVelocity, double heading, double pitch, double bank);

    bool isValid();



    bool     stateNow(TPlaneState *state);
private:
    double   normalizeRadians(double radian);


    QElapsedTimer   m_time;
    TPlaneState     *m_state_begin;
    TPlaneState     *m_state_end;

    bool            m_valid;

    CVector3D       m_a;
    CVector3D       m_b;

    double          m_timeEnd;
    double          m_timeBegin;

};

} // namespace BlackCore

#endif // INTERPOLATOR_H

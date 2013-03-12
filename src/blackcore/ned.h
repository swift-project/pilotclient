//! Copyright (C) 2013 Roland Winklmeier
//! This Source Code Form is subject to the terms of the Mozilla Public
//! License, v. 2.0. If a copy of the MPL was not distributed with this
//! file, You can obtain one at http://mozilla.org/MPL/2.0/

#ifndef VECTOR_NED_H
#define VECTOR_NED_H

#include "vector_3d.h"
#include "vector_geo.h"

namespace BlackCore
{

class CEcef;

class CNed : public CVector3D
{
public:
    CNed();
    CNed(CVectorGeo &pos, double N, double E, double D);

    double North() const {return v[0];}

    double East() const {return v[1];}

    double Down() const {return v[2];}

    CVectorGeo position() const { return m_position; }

    void setNorth(const double num) { v[0] = num; }

    void setEast(const double num) { v[1] = num; }

    void setDown(const double num) { v[2] = num; }

    void setPosition(const CVectorGeo &pos ) { m_position = pos; }

    CEcef toECEF();

private:
    CVectorGeo m_position;
};

} // namespace BlackCore

#endif // VECTOR_NED_H

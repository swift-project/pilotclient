/* Copyright (C) 2013 VATSIM Community / contributors
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#include "simconnect_object.h"
#include "blackcore/interpolator_linear.h"

using namespace BlackCore;

namespace BlackSimPlugin
{
    namespace Fsx
    {
        CSimConnectObject::CSimConnectObject() :
            m_interpolator(new CInterpolatorLinear()),
            m_requestId(-1),
            m_objectId(-1)
        {
        }
    }
}

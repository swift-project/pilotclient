/* Copyright (C) 2018
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution. No part of swift project, including this file, may be copied, modified, propagated,
 * or distributed except according to the terms contained in the LICENSE file.
 */

#include "dbuserror.h"

namespace XSwiftBus
{

    CDBusError::CDBusError(const DBusError *error)
        : m_name(error->name), m_message(error->message)
    {}

}

//! Copyright (C) 2013 Roland Winklmeier
//! This Source Code Form is subject to the terms of the Mozilla Public
//! License, v. 2.0. If a copy of the MPL was not distributed with this
//! file, You can obtain one at http://mozilla.org/MPL/2.0/

#ifndef MESSAGE_SYSTEM_H
#define MESSAGE_SYSTEM_H

#include "blackmisc/message_handler.h"
#include "blackmisc/message_dispatcher.h"
#include "blackmisc/message_factory.h"

namespace BlackMisc
{

class CMessageSystem
{
public:
    CMessageSystem();

    static void init();
};

} // namespace BlackMisc

#endif // MESSAGE_SYSTEM_H

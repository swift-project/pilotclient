/* Copyright (C) 2019
 * swift project community / contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution. No part of swift project, including this file, may be copied, modified, propagated,
 * or distributed except according to the terms contained in the LICENSE file.
 */

#include "blackcore/fsd/messagebase.h"

namespace BlackCore::Fsd
{
    MessageBase::MessageBase(const QString &sender)
        : m_sender(sender)
    {}

    MessageBase::MessageBase(const QString &sender, const QString &receiver)
        : m_sender(sender),
          m_receiver(receiver)
    {}
}

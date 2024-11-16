// SPDX-FileCopyrightText: Copyright (C) 2019 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

#include "core/fsd/messagebase.h"

namespace swift::core::fsd
{
    MessageBase::MessageBase(const QString &sender) : m_sender(sender) {}

    MessageBase::MessageBase(const QString &sender, const QString &receiver) : m_sender(sender), m_receiver(receiver) {}
} // namespace swift::core::fsd

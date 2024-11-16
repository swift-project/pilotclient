// SPDX-FileCopyrightText: Copyright (C) 2016 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

#include "registermetadatacomponents.h"

#include <QMetaType>

#include "gui/components/textmessagecomponenttab.h"

namespace swift::gui::components
{
    void registerMetadata()
    {
        qRegisterMetaType<TextMessageTab>();
    }
} // namespace swift::gui::components

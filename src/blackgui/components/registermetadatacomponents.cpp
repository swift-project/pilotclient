// SPDX-FileCopyrightText: Copyright (C) 2016 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

#include "registermetadatacomponents.h"
#include "blackgui/components/textmessagecomponenttab.h"

#include <QMetaType>

namespace BlackGui::Components
{
    void registerMetadata()
    {
        qRegisterMetaType<TextMessageTab>();
    }
} // ns

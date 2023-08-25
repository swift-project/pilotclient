// SPDX-FileCopyrightText: Copyright (C) 2018 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

//! \file

#ifndef BLACKGUI_COMPONENTS_TEXTMESSAGECOMPONENT_TAB_H
#define BLACKGUI_COMPONENTS_TEXTMESSAGECOMPONENT_TAB_H

#include <QMetaType>

namespace BlackGui::Components
{
    //! Tabs
    enum TextMessageTab
    {
        TextMessagesAll,
        TextMessagesUnicom,
        TextMessagesCom1,
        TextMessagesCom2
    };
} // ns

Q_DECLARE_METATYPE(BlackGui::Components::TextMessageTab)

#endif // guard

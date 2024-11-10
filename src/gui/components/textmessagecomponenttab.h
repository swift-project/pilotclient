// SPDX-FileCopyrightText: Copyright (C) 2018 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

//! \file

#ifndef SWIFT_GUI_COMPONENTS_TEXTMESSAGECOMPONENT_TAB_H
#define SWIFT_GUI_COMPONENTS_TEXTMESSAGECOMPONENT_TAB_H

#include <QMetaType>

namespace swift::gui::components
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

Q_DECLARE_METATYPE(swift::gui::components::TextMessageTab)

#endif // guard

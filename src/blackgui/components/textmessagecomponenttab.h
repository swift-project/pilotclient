/* Copyright (C) 2018
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution. No part of swift project, including this file, may be copied, modified, propagated,
 * or distributed except according to the terms contained in the LICENSE file.
 */

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

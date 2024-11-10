// SPDX-FileCopyrightText: Copyright (C) 2016 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

#include "gui/registermetadata.h"
#include "gui/settings/dockwidgetsettings.h"
#include "gui/settings/navigatorsettings.h"
#include "gui/settings/viewupdatesettings.h"
#include "gui/settings/guisettings.h"
#include "gui/settings/textmessagesettings.h"
#include "gui/components/registermetadatacomponents.h"

namespace swift::gui
{
    void registerMetadata()
    {
        swift::gui::settings::CDockWidgetSettings::registerMetadata();
        swift::gui::settings::CNavigatorSettings::registerMetadata();
        swift::gui::settings::CViewUpdateSettings::registerMetadata();
        swift::gui::settings::CGeneralGuiSettings::registerMetadata();
        swift::gui::settings::CTextMessageSettings::registerMetadata();
        swift::gui::components::registerMetadata();
    }
}

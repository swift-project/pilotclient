// SPDX-FileCopyrightText: Copyright (C) 2016 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

#include "blackgui/registermetadata.h"
#include "blackgui/settings/dockwidgetsettings.h"
#include "blackgui/settings/navigatorsettings.h"
#include "blackgui/settings/viewupdatesettings.h"
#include "blackgui/settings/guisettings.h"
#include "blackgui/settings/textmessagesettings.h"
#include "blackgui/settings/atcstationssettings.h"
#include "blackgui/components/registermetadatacomponents.h"

namespace BlackGui
{
    void registerMetadata()
    {
        BlackGui::Settings::CDockWidgetSettings::registerMetadata();
        BlackGui::Settings::CNavigatorSettings::registerMetadata();
        BlackGui::Settings::CViewUpdateSettings::registerMetadata();
        BlackGui::Settings::CGeneralGuiSettings::registerMetadata();
        BlackGui::Settings::CTextMessageSettings::registerMetadata();
        BlackGui::Settings::CAtcStationsSettings::registerMetadata();
        BlackGui::Components::registerMetadata();
    }
}

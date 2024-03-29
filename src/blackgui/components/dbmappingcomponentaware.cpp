// SPDX-FileCopyrightText: Copyright (C) 2015 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

#include "blackgui/components/dbmappingcomponent.h"
#include "blackgui/components/dbmappingcomponentaware.h"

#include <QObject>

using namespace BlackMisc;

namespace BlackGui::Components
{
    void CDbMappingComponentAware::setMappingComponent(CDbMappingComponent *component)
    {
        m_mappingComponent = component;
    }

    CDbMappingComponentAware::CDbMappingComponentAware(QObject *parent)
    {
        // if we get a mapping component we use it
        if (!parent) { return; }
        if (!parent->isWidgetType()) { return; }
        CDbMappingComponent *m = qobject_cast<CDbMappingComponent *>(parent);
        if (!m) { return; }
        m_mappingComponent = m;
    }

    bool CDbMappingComponentAware::showMappingComponentOverlayMessage(const CStatusMessage &message, int timeoutMs)
    {
        if (!m_mappingComponent) { return false; }
        return m_mappingComponent->showOverlayMessage(message, timeoutMs);
    }

    bool CDbMappingComponentAware::showMappingComponentOverlayHtmlMessage(const CStatusMessage &message, int timeoutMs)
    {
        if (!m_mappingComponent) { return false; }
        return m_mappingComponent->showOverlayHTMLMessage(message, timeoutMs);
    }
} // ns

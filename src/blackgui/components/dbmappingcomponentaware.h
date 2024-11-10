// SPDX-FileCopyrightText: Copyright (C) 2015 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

//! \file

#ifndef BLACKGUI_COMPONENTS_DBMAPPINGCOMPONENTAWARE_H
#define BLACKGUI_COMPONENTS_DBMAPPINGCOMPONENTAWARE_H

#include "misc/statusmessage.h"
#include "blackgui/blackguiexport.h"
#include <QObject>

namespace BlackGui::Components
{
    class CDbMappingComponent;

    //! Allows subcomponents to gain access to model component
    class BLACKGUI_EXPORT CDbMappingComponentAware
    {
    public:
        //! Set the corresponding component
        virtual void setMappingComponent(CDbMappingComponent *component);

        //! Get the mapping component
        CDbMappingComponent *getMappingComponent() const { return m_mappingComponent; }

    protected:
        //! Constructor
        CDbMappingComponentAware(QObject *parent);

        //! Destructor
        virtual ~CDbMappingComponentAware() {}

        //! Copy constructor
        CDbMappingComponentAware(const CDbMappingComponentAware &) = default;

        //! Copy assignment operator
        CDbMappingComponentAware &operator=(const CDbMappingComponentAware &) = default;

        //! @{
        //! Overlay messages
        bool showMappingComponentOverlayMessage(const swift::misc::CStatusMessage &message, int timeoutMs = -1);
        bool showMappingComponentOverlayHtmlMessage(const swift::misc::CStatusMessage &message, int timeoutMs = -1);
        //! @}

    private:
        CDbMappingComponent *m_mappingComponent = nullptr; //!< reference to component
    };
} // ns

Q_DECLARE_INTERFACE(BlackGui::Components::CDbMappingComponentAware, "org.swift-project.blackgui.components.dbmappingcomponentaware")

#endif // guard

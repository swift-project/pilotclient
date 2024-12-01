// SPDX-FileCopyrightText: Copyright (C) 2015 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

//! \file

#ifndef SWIFT_GUI_COMPONENTS_DBMAPPINGCOMPONENTAWARE_H
#define SWIFT_GUI_COMPONENTS_DBMAPPINGCOMPONENTAWARE_H

#include <QObject>

#include "gui/swiftguiexport.h"
#include "misc/statusmessage.h"

namespace swift::gui::components
{
    class CDbMappingComponent;

    //! Allows subcomponents to gain access to model component
    class SWIFT_GUI_EXPORT CDbMappingComponentAware
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
        bool showMappingComponentOverlayMessage(const swift::misc::CStatusMessage &message,
                                                std::chrono::milliseconds timeout = std::chrono::milliseconds(0));
        bool showMappingComponentOverlayHtmlMessage(const swift::misc::CStatusMessage &message,
                                                    std::chrono::milliseconds timeout = std::chrono::milliseconds(0));
        //! @}

    private:
        CDbMappingComponent *m_mappingComponent = nullptr; //!< reference to component
    };
} // namespace swift::gui::components

Q_DECLARE_INTERFACE(swift::gui::components::CDbMappingComponentAware,
                    "org.swift-project.swiftgui.components.dbmappingcomponentaware")

#endif // SWIFT_GUI_COMPONENTS_DBMAPPINGCOMPONENTAWARE_H

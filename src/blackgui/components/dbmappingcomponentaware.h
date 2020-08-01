/* Copyright (C) 2015
 * swift project Community / Contributors
 *
 * This file is part of swift Project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution. No part of swift project, including this file, may be copied, modified, propagated,
 * or distributed except according to the terms contained in the LICENSE file.
 */

//! \file

#ifndef BLACKGUI_COMPONENTS_DBMAPPINGCOMPONENTAWARE_H
#define BLACKGUI_COMPONENTS_DBMAPPINGCOMPONENTAWARE_H

#include "blackmisc/statusmessage.h"
#include "blackgui/blackguiexport.h"
#include <QObject>

namespace BlackGui
{
    namespace Components
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
            CDbMappingComponentAware &operator =(const CDbMappingComponentAware &) = default;

            //! Overlay messages
            //! @{
            bool showMappingComponentOverlayMessage(const BlackMisc::CStatusMessage &message, int timeoutMs = -1);
            bool showMappingComponentOverlayHtmlMessage(const BlackMisc::CStatusMessage &message, int timeoutMs = -1);
            //! @}

        private :
            CDbMappingComponent *m_mappingComponent = nullptr; //!< reference to component
        };
    } // ns
} // ns

Q_DECLARE_INTERFACE(BlackGui::Components::CDbMappingComponentAware, "org.swift-project.blackgui.components.dbmappingcomponentaware")

#endif // guard

/* Copyright (C) 2015
 * swift project Community / Contributors
 *
 * This file is part of swift Project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

//! \file

#ifndef BLACKGUI_COMPONENTS_DBMAPPINGCOMPONENTAWARE_H
#define BLACKGUI_COMPONENTS_DBMAPPINGCOMPONENTAWARE_H

#include "blackgui/blackguiexport.h"

class QObject;

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

        protected:
            //! Constructor
            CDbMappingComponentAware(QObject *parent);

            //! Get the mapping component
            CDbMappingComponent *getMappingComponent() const { return m_mappingComponent; }

        private :
            CDbMappingComponent *m_mappingComponent = nullptr; //!< reference to component
        };
    } // ns
} // ns
#endif // guard

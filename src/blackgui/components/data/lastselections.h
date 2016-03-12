/* Copyright (C) 2016
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

//! \file

#ifndef BLACKGUI_COMPONENTS_DATA_LASTSELECTIONS_H
#define BLACKGUI_COMPONENTS_DATA_LASTSELECTIONS_H

#include "blackgui/blackguiexport.h"
#include "blackmisc/valueobject.h"
#include "blackmisc/datacache.h"
#include "blackmisc/simulation/simulatorinfo.h"

namespace BlackGui
{
    namespace Components
    {
        namespace Data
        {
            /*!
             * Store last selections/interactions with this component
             */
            class BLACKGUI_EXPORT CDbMappingComponent : public BlackMisc::CValueObject<CDbMappingComponent>
            {
            public:
                //! Properties by index
                enum ColumnIndex
                {
                    IndexLastSimulator = BlackMisc::CPropertyIndex::GlobalIndexCCountry + 33
                };

                //! Simulator last selected
                const BlackMisc::Simulation::CSimulatorInfo &getLastSimulatorSelection() const { return m_simulator; }

                //! Simulator last selected
                void setLastSimulatorSelection(const BlackMisc::Simulation::CSimulatorInfo &simulator) { m_simulator = simulator; }

                //! \copydoc BlackMisc::Mixin::String::toQString
                QString convertToQString(bool i18n = false) const;

                //! \copydoc BlackMisc::Mixin::Index::propertyByIndex
                BlackMisc::CVariant propertyByIndex(const BlackMisc::CPropertyIndex &index) const;

                //! \copydoc BlackMisc::Mixin::Index::setPropertyByIndex
                void setPropertyByIndex(const BlackMisc::CVariant &variant, const BlackMisc::CPropertyIndex &index);

                //! Compare by index
                int comparePropertyByIndex(const CDbMappingComponent &compareValue, const BlackMisc::CPropertyIndex &index) const;

            private:
                BLACK_ENABLE_TUPLE_CONVERSION(CDbMappingComponent)
                BlackMisc::Simulation::CSimulatorInfo m_simulator; //!< Last simulator selection
            };

            //! Trait for model cache
            struct DbMappingComponent : public BlackMisc::CDataTrait<CDbMappingComponent>
            {
                //! Default value
                static const CDbMappingComponent &defaultValue()
                {
                    static const CDbMappingComponent ls;
                    return ls;
                }

                //! Key in data cache
                static const char *key() { return "dbmappingcomponent"; }
            };
        } // ns
    } // ns
} // ns

Q_DECLARE_METATYPE(BlackGui::Components::Data::CDbMappingComponent)
BLACK_DECLARE_TUPLE_CONVERSION(BlackGui::Components::Data::CDbMappingComponent, (
                                   attr(o.m_simulator)
                               ))

#endif // guard


/* Copyright (C) 2016
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

#include "lastselections.h"

using namespace BlackMisc;

namespace BlackGui
{
    namespace Components
    {
        namespace Data
        {
            QString CDbMappingComponent::convertToQString(bool i18n) const
            {
                QString s(this->m_simulator.toQString(i18n));
                return s;
            }

            CVariant CDbMappingComponent::propertyByIndex(const BlackMisc::CPropertyIndex &index) const
            {
                if (index.isMyself()) { return CVariant::from(*this); }
                ColumnIndex i = index.frontCasted<ColumnIndex>();
                switch (i)
                {
                case IndexLastSimulator:
                    return this->m_simulator.propertyByIndex(index.copyFrontRemoved());
                default:
                    return CValueObject::propertyByIndex(index);
                }
            }

            void CDbMappingComponent::setPropertyByIndex(const CVariant &variant, const BlackMisc::CPropertyIndex &index)
            {
                if (index.isMyself()) { (*this) = variant.to<CDbMappingComponent>(); return; }
                ColumnIndex i = index.frontCasted<ColumnIndex>();
                switch (i)
                {
                case IndexLastSimulator:
                    this->m_simulator.setPropertyByIndex(variant, index.copyFrontRemoved());
                    break;
                default:
                    CValueObject::setPropertyByIndex(variant, index);
                    break;
                }
            }

            int CDbMappingComponent::comparePropertyByIndex(const CDbMappingComponent &compareValue, const CPropertyIndex &index) const
            {
                if (index.isMyself()) { return this->toQString().compare(compareValue.toQString()); }
                ColumnIndex i = index.frontCasted<ColumnIndex>();
                switch (i)
                {
                case IndexLastSimulator:
                    return this->m_simulator.comparePropertyByIndex(compareValue.getLastSimulatorSelection(), index.copyFrontRemoved());
                default:
                    break;
                }
                Q_ASSERT_X(false, Q_FUNC_INFO, "No comparison");
                return 0;
            }

        } // ns
    } // ns
} // ns

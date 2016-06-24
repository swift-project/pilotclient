/* Copyright (C) 2016
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

#include "reader.h"

using namespace BlackMisc;
using namespace BlackMisc::PhysicalQuantities;

namespace BlackCore
{
    namespace Settings
    {
        CSettingsReader::CSettingsReader()
        { }

        CSettingsReader::CSettingsReader(const CTime &initialTime, const CTime &periodicTime, bool neverUpdate) :
            m_initialTime(initialTime), m_periodicTime(periodicTime), m_neverUpdate(neverUpdate)
        { }

        QString CSettingsReader::convertToQString(bool i18n) const
        {
            QString s("CReaderSettings");
            s.append(" ").append(this->m_initialTime.convertToQString(i18n));
            s.append(" ").append(this->m_periodicTime.convertToQString(i18n));
            return s;
        }

        const CSettingsReader &CSettingsReader::neverUpdateSettings()
        {
            static const CSettingsReader s(CTime{ 1.0, CTimeUnit::d()}, CTime{ 1.0, CTimeUnit::d()}, true);
            return s;
        }

        CVariant CSettingsReader::propertyByIndex(const BlackMisc::CPropertyIndex &index) const
        {
            if (index.isMyself()) { return CVariant::from(*this); }
            ColumnIndex i = index.frontCasted<ColumnIndex>();
            switch (i)
            {
            case IndexInitialTime:
                return this->m_initialTime.propertyByIndex(index.copyFrontRemoved());
            case IndexPeriodicTime:
                return this->m_periodicTime.propertyByIndex(index.copyFrontRemoved());
            case IndexNeverUpdate:
                return CVariant::fromValue(this->m_neverUpdate);
            default:
                return CValueObject::propertyByIndex(index);
            }
        }

        void CSettingsReader::setPropertyByIndex(const CPropertyIndex &index, const CVariant &variant)
        {
            if (index.isMyself()) { (*this) = variant.to<CSettingsReader>(); return; }
            ColumnIndex i = index.frontCasted<ColumnIndex>();
            switch (i)
            {
            case IndexInitialTime:
                this->m_initialTime.setPropertyByIndex(index.copyFrontRemoved(), variant);
                break;
            case IndexPeriodicTime:
                this->m_periodicTime.setPropertyByIndex(index.copyFrontRemoved(), variant);
                break;
            case IndexNeverUpdate:
                this->m_neverUpdate = variant.toBool();
                break;
            default:
                CValueObject::setPropertyByIndex(index, variant);
                break;
            }
        }
    } // ns
} // ns

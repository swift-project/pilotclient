/* Copyright (C) 2013
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

#include "blackmisc/aviation/modulator.h"
#include "blackmisc/aviation/comsystem.h"
#include "blackmisc/aviation/navsystem.h"
#include "blackmisc/aviation/adfsystem.h"
#include "blackmisc/propertyindex.h"
#include "blackmisc/variant.h"

using BlackMisc::PhysicalQuantities::CFrequency;
using BlackMisc::PhysicalQuantities::CFrequencyUnit;

namespace BlackMisc
{
    namespace Aviation
    {

        template <class AVIO>
        bool CModulator<AVIO>::isDefaultValue() const
        {
            return (this->m_frequencyActive == FrequencyNotSet());
        }

        template <class AVIO>
        void CModulator<AVIO>::toggleActiveStandby()
        {
            CFrequency a = this->m_frequencyActive;
            this->m_frequencyActive = this->m_frequencyStandby;
            this->m_frequencyStandby = a;
        }

        template <class AVIO>
        BlackMisc::PhysicalQuantities::CFrequency CModulator<AVIO>::getFrequencyActive() const
        {
            return this->m_frequencyActive;
        }

        template <class AVIO>
        BlackMisc::PhysicalQuantities::CFrequency CModulator<AVIO>::getFrequencyStandby() const
        {
            return this->m_frequencyStandby;
        }

        template <class AVIO>
        void CModulator<AVIO>::setFrequencyActive(const BlackMisc::PhysicalQuantities::CFrequency &frequency)
        {
            this->m_frequencyActive = frequency;
        }

        template <class AVIO>
        void CModulator<AVIO>::setFrequencyStandby(const BlackMisc::PhysicalQuantities::CFrequency &frequency)
        {
            this->m_frequencyStandby = frequency;
        }

        template <class AVIO>
        int CModulator<AVIO>::getVolumeOutput() const
        {
            return this->m_volumeOutput;
        }

        template <class AVIO>
        int CModulator<AVIO>::getVolumeInput() const
        {
            return this->m_volumeInput;
        }

        template <class AVIO>
        void CModulator<AVIO>::setVolumeOutput(int volume)
        {
            this->m_volumeOutput = volume;
        }

        template <class AVIO>
        void CModulator<AVIO>::setVolumeInput(int volume)
        {
            this->m_volumeInput = volume;
        }

        template <class AVIO>
        bool CModulator<AVIO>::isEnabled() const
        {
            return this->m_enabled;
        }

        template <class AVIO>
        void CModulator<AVIO>::setEnabled(bool enable)
        {
            this->m_enabled = enable;
        }

        template <class AVIO>
        QString CModulator<AVIO>::getName() const
        {
            return this->m_name;
        }

        template <class AVIO>
        CVariant CModulator<AVIO>::propertyByIndex(const CPropertyIndex &index) const
        {
            if (index.isMyself()) { return CVariant::from(*derived()); }
            ColumnIndex i = index.frontCasted<ColumnIndex>();
            switch (i)
            {
            case IndexActiveFrequency:
                return this->getFrequencyActive().propertyByIndex(index.copyFrontRemoved());
            case IndexStandbyFrequency:
                return this->getFrequencyStandby().propertyByIndex(index.copyFrontRemoved());
            case IndexEnabled:
                return CVariant::from(this->isEnabled());
            case IndexInputVolume:
                return CVariant::from(this->getVolumeInput());
            case IndexOutputVolume:
                return CVariant::from(this->getVolumeOutput());
            default:
                return CValueObject<CModulator<AVIO>>::propertyByIndex(index);
            }
        }

        template <class AVIO>
        void CModulator<AVIO>::setPropertyByIndex(const CVariant &variant, const CPropertyIndex &index)
        {
            if (index.isMyself()) { Q_ASSERT_X(false, Q_FUNC_INFO, "Wrong index to base template"); return; }
            ColumnIndex i = index.frontCasted<ColumnIndex>();
            switch (i)
            {
            case IndexActiveFrequency:
                this->m_frequencyActive.setPropertyByIndex(variant, index.copyFrontRemoved());
                break;
            case IndexStandbyFrequency:
                this->m_frequencyStandby.setPropertyByIndex(variant, index.copyFrontRemoved());
                break;
            case IndexEnabled:
                this->setEnabled(variant.toBool());
                break;
            case IndexInputVolume:
                this->setVolumeInput(variant.toInt());
                break;
            case IndexOutputVolume:
                this->setVolumeOutput(variant.toInt());
                break;
            default:
                CValueObject<CModulator<AVIO>>::setPropertyByIndex(variant, index);
                break;
            }
        }

        template <class AVIO>
        CModulator<AVIO>::CModulator() :
            m_name("default") {}

        template <class AVIO>
        CModulator<AVIO>::CModulator(const QString &name, const BlackMisc::PhysicalQuantities::CFrequency &activeFrequency, const BlackMisc::PhysicalQuantities::CFrequency &standbyFrequency) :
            m_name(name), m_frequencyActive(activeFrequency), m_frequencyStandby(standbyFrequency) {}

        template <class AVIO>
        CModulator<AVIO>::~CModulator() {}

        template <class AVIO>
        QString CModulator<AVIO>::convertToQString(bool i18n) const
        {
            QString s(this->getName());
            s.append(" Active: ").append(this->m_frequencyActive.valueRoundedWithUnit(3, i18n));
            s.append(" Standby: ").append(this->m_frequencyStandby.valueRoundedWithUnit(3, i18n));
            return s;
        }

        template <class AVIO>
        void CModulator<AVIO>::setFrequencyActiveKHz(double frequencyKHz)
        {
            this->m_frequencyActive = BlackMisc::PhysicalQuantities::CFrequency(frequencyKHz, BlackMisc::PhysicalQuantities::CFrequencyUnit::kHz());
        }

        template <class AVIO>
        void CModulator<AVIO>::setFrequencyStandbyKHz(double frequencyKHz)
        {
            this->m_frequencyStandby = BlackMisc::PhysicalQuantities::CFrequency(frequencyKHz, BlackMisc::PhysicalQuantities::CFrequencyUnit::kHz());
        }


        template <class AVIO>
        void CModulator<AVIO>::setFrequencyActiveMHz(double frequencyMHz)
        {
            frequencyMHz = Math::CMathUtils::round(frequencyMHz, 3);
            this->m_frequencyActive = BlackMisc::PhysicalQuantities::CFrequency(frequencyMHz, BlackMisc::PhysicalQuantities::CFrequencyUnit::MHz());
        }

        template <class AVIO>
        void CModulator<AVIO>::setFrequencyStandbyMHz(double frequencyMHz)
        {
            frequencyMHz = Math::CMathUtils::round(frequencyMHz, 3);
            this->m_frequencyStandby = BlackMisc::PhysicalQuantities::CFrequency(frequencyMHz, BlackMisc::PhysicalQuantities::CFrequencyUnit::MHz());
        }

        template <class AVIO>
        const QString &CModulator<AVIO>::NameCom1()
        {
            static QString n("COM1");
            return n;
        }

        template <class AVIO>
        const QString &CModulator<AVIO>::NameCom2()
        {
            static QString n("COM2");
            return n;
        }

        template <class AVIO>
        const QString &CModulator<AVIO>::NameCom3()
        {
            static QString n("COM3");
            return n;
        }

        template <class AVIO>
        const QString &CModulator<AVIO>::NameNav1()
        {
            static QString n("NAV1");
            return n;
        }

        template <class AVIO>
        const QString &CModulator<AVIO>::NameNav2()
        {
            static QString n("NAV2");
            return n;
        }

        template <class AVIO>
        const QString &CModulator<AVIO>::NameNav3()
        {
            static QString n("NAV3");
            return n;
        }

        template <class AVIO>
        const QString &CModulator<AVIO>::NameAdf1()
        {
            static QString n("ADF1");
            return n;
        }

        template <class AVIO>
        const QString &CModulator<AVIO>::NameAdf2()
        {
            static QString n("ADF2");
            return n;
        }

        template <class AVIO>
        const BlackMisc::PhysicalQuantities::CFrequency &CModulator<AVIO>::FrequencyNotSet()
        {
            static BlackMisc::PhysicalQuantities::CFrequency f;
            return f;
        }

        template <class AVIO>
        AVIO const *CModulator<AVIO>::derived() const
        {
            return static_cast<AVIO const *>(this);
        }

        template <class AVIO>
        AVIO *CModulator<AVIO>::derived()
        {
            return static_cast<AVIO *>(this);
        }

        // see here for the reason of thess forward instantiations
        // http://www.parashift.com/c++-faq/separate-template-class-defn-from-decl.html
        template class CModulator<CComSystem>;
        template class CModulator<CNavSystem>;
        template class CModulator<CAdfSystem>;

    } // namespace
} // namespace

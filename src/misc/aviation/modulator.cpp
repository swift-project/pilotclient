// SPDX-FileCopyrightText: Copyright (C) 2013 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

#include "misc/aviation/modulator.h"

#include <type_traits>

#include <QtGlobal>

#include "misc/aviation/comsystem.h"
#include "misc/aviation/navsystem.h"
#include "misc/comparefunctions.h"
#include "misc/pq/units.h"
#include "misc/propertyindexref.h"

using swift::misc::physical_quantities::CFrequency;
using swift::misc::physical_quantities::CFrequencyUnit;

namespace swift::misc::aviation
{
    template <class AVIO>
    void CModulator<AVIO>::toggleActiveStandby()
    {
        const CFrequency a = m_frequencyActive;
        m_frequencyActive = m_frequencyStandby;
        m_frequencyStandby = a;
    }

    template <class AVIO>
    physical_quantities::CFrequency CModulator<AVIO>::getFrequencyActive() const
    {
        return m_frequencyActive;
    }

    template <class AVIO>
    physical_quantities::CFrequency CModulator<AVIO>::getFrequencyStandby() const
    {
        return m_frequencyStandby;
    }

    template <class AVIO>
    void CModulator<AVIO>::setFrequencyActive(const swift::misc::physical_quantities::CFrequency &frequency)
    {
        m_frequencyActive = frequency;
    }

    template <class AVIO>
    void CModulator<AVIO>::setFrequencyStandby(const swift::misc::physical_quantities::CFrequency &frequency)
    {
        m_frequencyStandby = frequency;
    }

    template <class AVIO>
    int CModulator<AVIO>::getVolumeReceive() const
    {
        return m_volumeReceive;
    }

    template <class AVIO>
    int CModulator<AVIO>::getVolumeTransmit() const
    {
        return m_volumeTransmit;
    }

    template <class AVIO>
    void CModulator<AVIO>::setVolumeReceive(int volume)
    {
        if (volume >= 100) { m_volumeReceive = 100; }
        else if (volume <= 0) { m_volumeReceive = 0; }
        else { m_volumeReceive = volume; }
    }

    template <class AVIO>
    void CModulator<AVIO>::setVolumeTransmit(int volume)
    {
        if (volume >= 100) { m_volumeTransmit = 100; }
        else if (volume <= 0) { m_volumeTransmit = 0; }
        else { m_volumeTransmit = volume; }
    }

    template <class AVIO>
    bool CModulator<AVIO>::isTransmitEnabled() const
    {
        return m_transmitEnabled;
    }

    template <class AVIO>
    bool CModulator<AVIO>::isReceiveEnabled() const
    {
        return m_receiveEnabled;
    }

    template <class AVIO>
    void CModulator<AVIO>::setTransmitEnabled(bool enable)
    {
        m_transmitEnabled = enable;
    }

    template <class AVIO>
    void CModulator<AVIO>::setReceiveEnabled(bool enable)
    {
        m_receiveEnabled = enable;
    }

    template <class AVIO>
    QString CModulator<AVIO>::getName() const
    {
        return m_name;
    }

    template <class AVIO>
    QVariant CModulator<AVIO>::propertyByIndex(CPropertyIndexRef index) const
    {
        if (index.isMyself()) { return QVariant::fromValue(*derived()); }
        const auto i = index.frontCasted<ColumnIndex>();
        switch (i)
        {
        case IndexActiveFrequency: return this->getFrequencyActive().propertyByIndex(index.copyFrontRemoved());
        case IndexStandbyFrequency: return this->getFrequencyStandby().propertyByIndex(index.copyFrontRemoved());
        case IndexEnabledTransmit: return QVariant::fromValue(this->isTransmitEnabled());
        case IndexEnabledReceive: return QVariant::fromValue(this->isReceiveEnabled());
        case IndexTransmitVolume: return QVariant::fromValue(this->getVolumeTransmit());
        case IndexReceiveVolume: return QVariant::fromValue(this->getVolumeReceive());
        default: return CValueObject<CModulator<AVIO>>::propertyByIndex(index);
        }
    }

    template <class AVIO>
    void CModulator<AVIO>::setPropertyByIndex(CPropertyIndexRef index, const QVariant &variant)
    {
        if (index.isMyself())
        {
            Q_ASSERT_X(false, Q_FUNC_INFO, "Wrong index to base template");
            return;
        }
        const auto i = index.frontCasted<ColumnIndex>();
        switch (i)
        {
        case IndexActiveFrequency: m_frequencyActive.setPropertyByIndex(index.copyFrontRemoved(), variant); break;
        case IndexStandbyFrequency: m_frequencyStandby.setPropertyByIndex(index.copyFrontRemoved(), variant); break;
        case IndexEnabledTransmit: this->setTransmitEnabled(variant.toBool()); break;
        case IndexEnabledReceive: this->setReceiveEnabled(variant.toBool()); break;
        case IndexTransmitVolume: this->setVolumeTransmit(variant.toInt()); break;
        case IndexReceiveVolume: this->setVolumeReceive(variant.toInt()); break;
        default: CValueObject<CModulator<AVIO>>::setPropertyByIndex(index, variant); break;
        }
    }

    template <class AVIO>
    int CModulator<AVIO>::comparePropertyByIndex(CPropertyIndexRef index, const AVIO &compareValue) const
    {
        if (index.isMyself())
        {
            return m_frequencyActive.comparePropertyByIndex(index.copyFrontRemoved(), compareValue.m_frequencyActive);
        }
        const auto i = index.frontCasted<ColumnIndex>();
        switch (i)
        {
        case IndexActiveFrequency:
            return m_frequencyActive.comparePropertyByIndex(index.copyFrontRemoved(), compareValue.m_frequencyActive);
        case IndexStandbyFrequency:
            return m_frequencyStandby.comparePropertyByIndex(index.copyFrontRemoved(), compareValue.m_frequencyStandby);
        case IndexEnabledTransmit: return Compare::compare(this->isTransmitEnabled(), compareValue.isTransmitEnabled());
        case IndexEnabledReceive: return Compare::compare(this->isReceiveEnabled(), compareValue.isReceiveEnabled());
        case IndexTransmitVolume: return Compare::compare(this->getVolumeTransmit(), compareValue.getVolumeTransmit());
        case IndexReceiveVolume: return Compare::compare(this->getVolumeReceive(), compareValue.getVolumeReceive());
        default: break;
        }
        Q_ASSERT_X(false, Q_FUNC_INFO, "Compare failed");
        return 0;
    }

    template <class AVIO>
    CModulator<AVIO>::CModulator() : m_name("default")
    {
        static_assert(!std::is_polymorphic_v<AVIO>, "Must not use virtual functions for value classes");
    }

    template <class AVIO>
    CModulator<AVIO>::CModulator(const QString &name,
                                 const swift::misc::physical_quantities::CFrequency &activeFrequency,
                                 const swift::misc::physical_quantities::CFrequency &standbyFrequency)
        : m_name(name), m_frequencyActive(activeFrequency), m_frequencyStandby(standbyFrequency)
    {
        static_assert(!std::is_polymorphic_v<AVIO>, "Must not use virtual functions for value classes");
    }

    template <class AVIO>
    QString CModulator<AVIO>::convertToQString(bool i18n) const
    {
        QString s(this->getName());
        s.append(" Active: ").append(m_frequencyActive.valueRoundedWithUnit(3, i18n));
        s.append(" Standby: ").append(m_frequencyStandby.valueRoundedWithUnit(3, i18n));
        return s;
    }

    template <class AVIO>
    void CModulator<AVIO>::setFrequencyActiveKHz(double frequencyKHz)
    {
        m_frequencyActive = swift::misc::physical_quantities::CFrequency(
            frequencyKHz, swift::misc::physical_quantities::CFrequencyUnit::kHz());
    }

    template <class AVIO>
    void CModulator<AVIO>::setFrequencyStandbyKHz(double frequencyKHz)
    {
        m_frequencyStandby = swift::misc::physical_quantities::CFrequency(
            frequencyKHz, swift::misc::physical_quantities::CFrequencyUnit::kHz());
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

    // see here for the reason of the forward instantiations
    // https://isocpp.org/wiki/faq/templates#separate-template-fn-defn-from-decl
    //! \cond PRIVATE
    template class CModulator<CComSystem>;
    template class CModulator<CNavSystem>;
    //! \endcond

} // namespace swift::misc::aviation

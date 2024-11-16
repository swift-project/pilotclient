// SPDX-FileCopyrightText: Copyright (C) 2013 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

//! \file

#ifndef SWIFT_MISC_AVIATION_MODULATORUNIT_H
#define SWIFT_MISC_AVIATION_MODULATORUNIT_H

#include "misc/pq/frequency.h"
#include "misc/propertyindexref.h"

namespace swift::misc::aviation
{
    //! Base class for COM, NAV, Squawk units.
    template <class AVIO>
    class CModulator : public CValueObject<CModulator<AVIO>>
    {
    public:
        //! Column indexes
        enum ColumnIndex
        {
            IndexActiveFrequency = CPropertyIndexRef::GlobalIndexCModulator,
            IndexStandbyFrequency,
            IndexReceiveVolume,
            IndexTransmitVolume,
            IndexEnabledTransmit,
            IndexEnabledReceive
        };

        //! Toggle active and standby frequencies
        void toggleActiveStandby();

        //! Active frequency
        swift::misc::physical_quantities::CFrequency getFrequencyActive() const;

        //! Standby frequency
        swift::misc::physical_quantities::CFrequency getFrequencyStandby() const;

        //! Output volume 0..100
        int getVolumeReceive() const;

        //! Input volume 0..100
        int getVolumeTransmit() const;

        //! Output volume 0.100
        void setVolumeReceive(int volume);

        //! Input volume 0..100
        void setVolumeTransmit(int volume);

        //! Name
        QString getName() const;

        //! Enabled?
        bool isTransmitEnabled() const;

        //! Enabled?
        bool isReceiveEnabled() const;

        //! Enabled?
        void setTransmitEnabled(bool enable);

        //! Enabled?
        void setReceiveEnabled(bool enable);

        //! \copydoc swift::misc::mixin::Index::propertyByIndex
        QVariant propertyByIndex(swift::misc::CPropertyIndexRef index) const;

        //! \copydoc swift::misc::mixin::Index::setPropertyByIndex
        void setPropertyByIndex(swift::misc::CPropertyIndexRef index, const QVariant &variant);

        //! Compare by property index
        int comparePropertyByIndex(swift::misc::CPropertyIndexRef index, const AVIO &compareValue) const;

        //! \copydoc swift::misc::mixin::String::toQString
        QString convertToQString(bool i18n = false) const;

    protected:
        //! Default constructor
        CModulator();

        //! Constructor
        CModulator(const QString &name, const swift::misc::physical_quantities::CFrequency &activeFrequency,
                   const swift::misc::physical_quantities::CFrequency &standbyFrequency);

        //! Set active frequency
        void setFrequencyActive(const swift::misc::physical_quantities::CFrequency &frequency);

        //! Set standby frequency
        void setFrequencyStandby(const swift::misc::physical_quantities::CFrequency &frequency);

        //! Set active frequency
        void setFrequencyActiveKHz(double frequencyKHz);

        //! Set standby frequency
        void setFrequencyStandbyKHz(double frequencyKHz);

        //! COM1
        static const QString &NameCom1();

        //! COM2
        static const QString &NameCom2();

        //! COM3
        static const QString &NameCom3();

        //! NAV1
        static const QString &NameNav1();

        //! NAV2
        static const QString &NameNav2();

        //! NAV3
        static const QString &NameNav3();

    private:
        QString m_name; //!< name of the unit
        physical_quantities::CFrequency m_frequencyActive; //!< active frequency
        physical_quantities::CFrequency m_frequencyStandby; //!< standby frequency
        int m_volumeTransmit = 100; //!< volume transmit/input 0..100
        int m_volumeReceive = 100; //!< volume receive/output 0..100
        bool m_transmitEnabled = true; //!< is enabled, used e.g. for mute etc.
        bool m_receiveEnabled = true; //!< is enabled, used e.g. for mute etc.

        //! Easy access to derived class (CRTP template parameter)
        AVIO const *derived() const { return static_cast<AVIO const *>(this); }

        //! Easy access to derived class (CRTP template parameter)
        AVIO *derived() { return static_cast<AVIO *>(this); }

        SWIFT_METACLASS(
            CModulator,
            SWIFT_METAMEMBER(name),
            SWIFT_METAMEMBER(frequencyActive),
            SWIFT_METAMEMBER(frequencyStandby),
            SWIFT_METAMEMBER(volumeTransmit),
            SWIFT_METAMEMBER(volumeReceive),
            SWIFT_METAMEMBER(transmitEnabled),
            SWIFT_METAMEMBER(receiveEnabled));
    };

    //! \cond PRIVATE
    class CComSystem;
    class CNavSystem;

    extern template class SWIFT_MISC_EXPORT_DECLARE_TEMPLATE CModulator<CComSystem>;
    extern template class SWIFT_MISC_EXPORT_DECLARE_TEMPLATE CModulator<CNavSystem>;
    //! \endcond

} // namespace swift::misc::aviation

#endif // guard

/*  Copyright (C) 2013 VATSIM Community / contributors
 *  This Source Code Form is subject to the terms of the Mozilla Public
 *  License, v. 2.0. If a copy of the MPL was not distributed with this
 *  file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#ifndef BLACKMISC_AVIOMODULATORUNIT_H
#define BLACKMISC_AVIOMODULATORUNIT_H

#include <QDBusMetaType>
#include "blackmisc/aviobase.h"
#include "blackmisc/mathematics.h"

using namespace BlackMisc::Math;

namespace BlackMisc
{
    namespace Aviation
    {

        /*!
         * \brief Base class for COM, NAV, Squawk units.
         */
        template <class AVIO> class CModulator : public CAvionicsBase
        {
        private:
            BLACK_ENABLE_TUPLE_CONVERSION(CModulator)
            BlackMisc::PhysicalQuantities::CFrequency m_frequencyActive; //!< active frequency
            BlackMisc::PhysicalQuantities::CFrequency m_frequencyStandby; //!< standby frequency
            qint32 m_volumeInput; //!< volume input
            qint32 m_volumeOutput; //!< volume output
            bool m_enabled; //!< is enabled, used e.g. for mute etc.

        protected:
            int m_digits; //!< digits used

        protected:
            //! \brief Default constructor
            CModulator() :
                CAvionicsBase("default"), m_volumeInput(0), m_volumeOutput(0), m_enabled(true), m_digits(2) {}

            //! \brief Constructor
            CModulator(const QString &name, const BlackMisc::PhysicalQuantities::CFrequency &activeFrequency, const BlackMisc::PhysicalQuantities::CFrequency &standbyFrequency, int digits) :
                CAvionicsBase(name), m_frequencyActive(activeFrequency), m_frequencyStandby(standbyFrequency), m_volumeInput(0), m_volumeOutput(0), m_enabled(true), m_digits(digits) {}

            //! \copydoc CValueObject::convertToQString
            virtual QString convertToQString(bool i18n = false) const override
            {
                QString s(this->getName());
                s.append(" Active: ").append(this->m_frequencyActive.valueRoundedWithUnit(3, i18n));
                s.append(" Standby: ").append(this->m_frequencyStandby.valueRoundedWithUnit(3, i18n));
                return s;
            }

            //! \brief Set active frequency
            void setFrequencyActiveKHz(double frequencyKHz)
            {
                this->m_frequencyActive = BlackMisc::PhysicalQuantities::CFrequency(frequencyKHz, BlackMisc::PhysicalQuantities::CFrequencyUnit::kHz());
            }

            //! \brief Set standby frequency
            void setFrequencyStandbyKHz(double frequencyKHz)
            {
                this->m_frequencyStandby = BlackMisc::PhysicalQuantities::CFrequency(frequencyKHz, BlackMisc::PhysicalQuantities::CFrequencyUnit::kHz());
            }

            //! \brief Set active frequency
            void setFrequencyActiveMHz(double frequencyMHz)
            {
                frequencyMHz = CMath::round(frequencyMHz, 3);
                this->m_frequencyActive = BlackMisc::PhysicalQuantities::CFrequency(frequencyMHz, BlackMisc::PhysicalQuantities::CFrequencyUnit::MHz());
            }

            //! \brief Set standby frequency
            void setFrequencyStandbyMHz(double frequencyMHz)
            {
                frequencyMHz = CMath::round(frequencyMHz, 3);
                this->m_frequencyStandby = BlackMisc::PhysicalQuantities::CFrequency(frequencyMHz, BlackMisc::PhysicalQuantities::CFrequencyUnit::MHz());
            }

            //! \brief operator ==
            bool operator ==(const CModulator &other) const;

            //! \brief operator !=
            bool operator !=(const CModulator &other) const;

            //! \copydoc CValueObject::compareImpl(otherBase)
            virtual int compareImpl(const CValueObject &otherBase) const override;

            //! \brief COM1
            static const QString &NameCom1()
            {
                static QString n("COM1");
                return n;
            }

            //! \brief COM2
            static const QString &NameCom2()
            {
                static QString n("COM2");
                return n;
            }

            //! \brief COM3
            static const QString &NameCom3()
            {
                static QString n("COM3");
                return n;
            }

            //! \brief NAV1
            static const QString &NameNav1()
            {
                static QString n("NAV1");
                return n;
            }

            //! \brief NAV2
            static const QString &NameNav2()
            {
                static QString n("NAV2");
                return n;
            }

            //! \brief NAV3
            static const QString &NameNav3()
            {
                static QString n("NAV3");
                return n;
            }

            //! \brief ADF1
            static const QString &NameAdf1()
            {
                static QString n("ADF1");
                return n;
            }

            //! \brief ADF2
            static const QString &NameAdf2()
            {
                static QString n("ADF2");
                return n;
            }

            //! \brief Frequency not set
            static const BlackMisc::PhysicalQuantities::CFrequency &FrequencyNotSet()
            {
                static BlackMisc::PhysicalQuantities::CFrequency f;
                return f;
            }

            //! \copydoc CValueObject::marshallFromDbus()
            virtual void marshallToDbus(QDBusArgument &argument) const override;

            //! \copydoc CValueObject::unmarshallFromDbus()
            virtual void unmarshallFromDbus(const QDBusArgument &argument) override;

            //! \copydoc CValueObject::getValueHash()
            virtual uint getValueHash() const override;

        public:
            //! \brief Virtual destructor
            virtual ~CModulator() {}

            //! \brief Default value?
            virtual bool isDefaultValue() const
            {
                return this->m_frequencyActive == CModulator::FrequencyNotSet();
            }

            //! \brief Toggle active and standby frequencies
            void toggleActiveStandby();

            //! \brief Active frequency
            BlackMisc::PhysicalQuantities::CFrequency getFrequencyActive() const
            {
                return this->m_frequencyActive;
            }

            //! \brief Standby frequency
            BlackMisc::PhysicalQuantities::CFrequency getFrequencyStandby() const
            {
                return this->m_frequencyStandby;
            }

            //! \brief Set active frequency
            void setFrequencyActive(const BlackMisc::PhysicalQuantities::CFrequency &frequency)
            {
                this->m_frequencyActive = frequency;
            }

            //! \brief Set standby frequency
            void setFrequencyStandby(const BlackMisc::PhysicalQuantities::CFrequency &frequency)
            {
                this->m_frequencyStandby = frequency;
            }

            //! \brief Output volume
            qint32 getVolumeOutput() const { return this->m_volumeOutput; }

            //! \brief Input volume
            qint32 getVolumeInput() const { return this->m_volumeInput; }

            //! \brief Output volume
            void setVolumeOutput(qint32 volume) { this->m_volumeOutput = volume; }

            //! \brief Input volume
            void setVolumeInput(qint32 volume) { this->m_volumeInput = volume; }

            //! \brief Enabled?
            bool isEnabled() const { return this->m_enabled;}

            //! \brief Enabled?
            void setEnabled(bool enable) { this->m_enabled = enable;}

            /*!
             * \brief Register metadata
             */
            static void registerMetadata();
        };

    } // namespace
} // namespace

BLACK_DECLARE_TUPLE_CONVERSION_TEMPLATE(BlackMisc::Aviation::CModulator, (o.m_frequencyActive, o.m_frequencyStandby, o.m_volumeInput , o.m_volumeOutput, o.m_enabled))

#endif // guard

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
            BlackMisc::PhysicalQuantities::CFrequency m_frequencyActive; //!< active frequency
            BlackMisc::PhysicalQuantities::CFrequency m_frequencyStandby; //!< standby frequency
            qint32 m_volumeInput; //!< volume input
            qint32 m_volumeOutput; //!< volume output
            bool m_enabled; //!< is enabled, used e.g. for mute etc.

        protected:
            int m_digits; //!< digits used

        protected:
            /*!
             * \brief Default constructor
             */
            CModulator() :
                CAvionicsBase("default"), m_volumeInput(0), m_volumeOutput(0), m_digits(2) {}

            /*!
             * \brief Constructor
             * \param name
             * \param activeFrequency
             * \param standbyFrequency
             * \param digits
             */
            CModulator(const QString &name, const BlackMisc::PhysicalQuantities::CFrequency &activeFrequency, const BlackMisc::PhysicalQuantities::CFrequency &standbyFrequency, int digits) :
                CAvionicsBase(name), m_frequencyActive(activeFrequency), m_frequencyStandby(standbyFrequency), m_volumeInput(0), m_volumeOutput(0), m_digits(digits), m_enabled(true) {}

            /*!
             * \brief String for converter
             * \param i18n
             * \return
             */
            virtual QString convertToQString(bool i18n = false) const
            {
                QString s(this->getName());
                s.append(" Active: ").append(this->m_frequencyActive.valueRoundedWithUnit(3, i18n));
                s.append(" Standby: ").append(this->m_frequencyStandby.valueRoundedWithUnit(3, i18n));
                return s;
            }

            /*!
             * \brief Set active frequency
             * \param frequencyKHz
             */
            void setFrequencyActiveKHz(double frequencyKHz)
            {
                this->m_frequencyActive = BlackMisc::PhysicalQuantities::CFrequency(frequencyKHz, BlackMisc::PhysicalQuantities::CFrequencyUnit::kHz());
            }

            /*!
             * \brief Set standby frequency
             * \param frequencyKHz
             */
            void setFrequencyStandbyKHz(double frequencyKHz)
            {
                this->m_frequencyStandby = BlackMisc::PhysicalQuantities::CFrequency(frequencyKHz, BlackMisc::PhysicalQuantities::CFrequencyUnit::kHz());
            }

            /*!
             * \brief Set active frequency
             * \param frequencyMHz
             */
            void setFrequencyActiveMHz(double frequencyMHz)
            {
                frequencyMHz = CMath::round(frequencyMHz, 3);
                this->m_frequencyActive = BlackMisc::PhysicalQuantities::CFrequency(frequencyMHz, BlackMisc::PhysicalQuantities::CFrequencyUnit::MHz());
            }

            /*!
             * \brief Set standby frequency
             * \param frequencyMHz
             */
            void setFrequencyStandbyMHz(double frequencyMHz)
            {
                frequencyMHz = CMath::round(frequencyMHz, 3);
                this->m_frequencyStandby = BlackMisc::PhysicalQuantities::CFrequency(frequencyMHz, BlackMisc::PhysicalQuantities::CFrequencyUnit::MHz());
            }

            /*!
             * \brief operator ==
             * \param other
             * \return
             */
            bool operator ==(const CModulator &other) const;

            /*!
             * \brief operator !=
             * \param other
             * \return
             */
            bool operator !=(const CModulator &other) const;

            /*!
             * \brief COM1
             * \return
             */
            static const QString &NameCom1()
            {
                static QString n("COM1");
                return n;
            }

            /*!
             * \brief COM2
             * \return
             */
            static const QString &NameCom2()
            {
                static QString n("COM2");
                return n;
            }

            /*!
             * \brief COM3
             * \return
             */
            static const QString &NameCom3()
            {
                static QString n("COM3");
                return n;
            }

            /*!
             * \brief NAV1
             * \return
             */
            static const QString &NameNav1()
            {
                static QString n("NAV1");
                return n;
            }

            /*!
             * \brief NAV2
             * \return
             */
            static const QString &NameNav2()
            {
                static QString n("NAV2");
                return n;
            }

            /*!
             * \brief NAV2
             * \return
             */
            static const QString &NameNav3()
            {
                static QString n("NAV3");
                return n;
            }

            /*!
             * \brief ADF1
             * \return
             */
            static const QString &NameAdf1()
            {
                static QString n("ADF1");
                return n;
            }

            /*!
             * \brief ADF2
             * \return
             */
            static const QString &NameAdf2()
            {
                static QString n("ADF2");
                return n;
            }

            /*!
             * \brief Frequency not set
             * \return
             */
            static const BlackMisc::PhysicalQuantities::CFrequency &FrequencyNotSet()
            {
                static BlackMisc::PhysicalQuantities::CFrequency f;
                return f;
            }

            /*!
             * \brief Stream to DBus <<
             * \param argument
             */
            virtual void marshallToDbus(QDBusArgument &argument) const;

            /*!
             * \brief Stream from DBus >>
             * \param argument
             */
            virtual void unmarshallFromDbus(const QDBusArgument &argument);

            /*!
             * \brief Value hash
             * \return
             */
            virtual uint getValueHash() const;

        public:
            /*!
             * \brief Virtual destructor
             */
            virtual ~CModulator() {}

            /*!
             * \brief Default value
             * \return
             */
            virtual bool isDefaultValue() const
            {
                return this->m_frequencyActive == CModulator::FrequencyNotSet();
            }

            /*!
             * \brief Toggle active and standby frequencies
             */
            void toggleActiveStandby();

            /*!
             * \brief Active frequency
             * \return
             */
            BlackMisc::PhysicalQuantities::CFrequency getFrequencyActive() const
            {
                return this->m_frequencyActive;
            }

            /*!
             * \brief Standby frequency
             * \return
             */
            BlackMisc::PhysicalQuantities::CFrequency getFrequencyStandby() const
            {
                return this->m_frequencyStandby;
            }

            /*!
             * \brief Set active frequency
             * \param frequency
             */
            void setFrequencyActive(const BlackMisc::PhysicalQuantities::CFrequency &frequency)
            {
                this->m_frequencyActive = frequency;
            }

            /*!
             * \brief Set standby frequency
             * \param frequency
             */
            void setFrequencyStandby(const BlackMisc::PhysicalQuantities::CFrequency &frequency)
            {
                this->m_frequencyStandby = frequency;
            }

            /*!
             * \brief Output volume
             * \return
             */
            qint32 getVolumeOutput() const { return this->m_volumeOutput; }

            /*!
             * \brief Input volume
             * \return
             */
            qint32 getVolumeInput() const { return this->m_volumeInput; }

            /*!
             * \brief Output volume
             */
            void setVolumeOutput(qint32 volume) { this->m_volumeOutput = volume; }

            /*!
             * \brief Input volume
             */
            void setVolumeInput(qint32 volume) { this->m_volumeInput = volume; }

            /*!
             * \brief Enabled?
             * \return
             */
            bool isEnabled() const { return this->m_enabled;}

            /*!
             * \brief Enabled?
             * \param
             */
            void setEnabled(bool enable) { this->m_enabled = enable;}

            /*!
             * \brief Register metadata
             */
            static void registerMetadata();
        };

    } // namespace
} // namespace

#endif // guard

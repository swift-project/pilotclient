/* Copyright (C) 2013
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

//! \file

#ifndef BLACKMISC_AVIOMODULATORUNIT_H
#define BLACKMISC_AVIOMODULATORUNIT_H

#include <QDBusMetaType>
#include "blackmisc/propertyindex.h"
#include "blackmisc/aviobase.h"
#include "blackmisc/mathematics.h"

using namespace BlackMisc::Math;

namespace BlackMisc
{
    namespace Aviation
    {

        /*!
         * Base class for COM, NAV, Squawk units.
         */
        template <class AVIO> class CModulator : public CAvionicsBase
        {

        public:

            //! Column indexes
            enum ColumnIndex
            {
                IndexActiveFrequency = BlackMisc::CPropertyIndex::GlobalIndexCModulator,
                IndexStandbyFrequency,
                IndexOutputVolume,
                IndexInputVolume,
                IndexEnabled
            };

            //! Virtual destructor
            virtual ~CModulator() {}

            //! Default value?
            virtual bool isDefaultValue() const
            {
                return this->m_frequencyActive == CModulator::FrequencyNotSet();
            }

            //! Toggle active and standby frequencies
            void toggleActiveStandby();

            //! Active frequency
            BlackMisc::PhysicalQuantities::CFrequency getFrequencyActive() const
            {
                return this->m_frequencyActive;
            }

            //! Standby frequency
            BlackMisc::PhysicalQuantities::CFrequency getFrequencyStandby() const
            {
                return this->m_frequencyStandby;
            }

            //! Set active frequency
            void setFrequencyActive(const BlackMisc::PhysicalQuantities::CFrequency &frequency)
            {
                this->m_frequencyActive = frequency;
            }

            //! Set standby frequency
            void setFrequencyStandby(const BlackMisc::PhysicalQuantities::CFrequency &frequency)
            {
                this->m_frequencyStandby = frequency;
            }

            //! Output volume
            qint32 getVolumeOutput() const { return this->m_volumeOutput; }

            //! Input volume
            qint32 getVolumeInput() const { return this->m_volumeInput; }

            //! Output volume
            void setVolumeOutput(qint32 volume) { this->m_volumeOutput = volume; }

            //! Input volume
            void setVolumeInput(qint32 volume) { this->m_volumeInput = volume; }

            //! Enabled?
            bool isEnabled() const { return this->m_enabled;}

            //! Enabled?
            void setEnabled(bool enable) { this->m_enabled = enable;}

            //! \copydoc CValueObject::toJson
            virtual QJsonObject toJson() const override;

            //! \copydoc CValueObject::fromJson
            virtual void fromJson(const QJsonObject &json) override;

            //! \copydoc CValueObject::propertyByIndex
            virtual QVariant propertyByIndex(const BlackMisc::CPropertyIndex &index) const override;

            //! \copydoc CValueObject::setPropertyByIndex(variant, index)
            virtual void setPropertyByIndex(const QVariant &variant, const BlackMisc::CPropertyIndex &index) override;

            //! Register metadata
            static void registerMetadata();

            //! Members
            static const QStringList &jsonMembers();

        protected:
            int m_digits; //!< digits used

            //! Default constructor
            CModulator() :
                CAvionicsBase("default"), m_volumeInput(0), m_volumeOutput(0), m_enabled(true), m_digits(2) {}

            //! Constructor
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

            //! Set active frequency
            void setFrequencyActiveKHz(double frequencyKHz)
            {
                this->m_frequencyActive = BlackMisc::PhysicalQuantities::CFrequency(frequencyKHz, BlackMisc::PhysicalQuantities::CFrequencyUnit::kHz());
            }

            //! Set standby frequency
            void setFrequencyStandbyKHz(double frequencyKHz)
            {
                this->m_frequencyStandby = BlackMisc::PhysicalQuantities::CFrequency(frequencyKHz, BlackMisc::PhysicalQuantities::CFrequencyUnit::kHz());
            }

            //! Set active frequency
            void setFrequencyActiveMHz(double frequencyMHz)
            {
                frequencyMHz = CMath::round(frequencyMHz, 3);
                this->m_frequencyActive = BlackMisc::PhysicalQuantities::CFrequency(frequencyMHz, BlackMisc::PhysicalQuantities::CFrequencyUnit::MHz());
            }

            //! Set standby frequency
            void setFrequencyStandbyMHz(double frequencyMHz)
            {
                frequencyMHz = CMath::round(frequencyMHz, 3);
                this->m_frequencyStandby = BlackMisc::PhysicalQuantities::CFrequency(frequencyMHz, BlackMisc::PhysicalQuantities::CFrequencyUnit::MHz());
            }

            //! operator ==
            bool operator ==(const CModulator &other) const;

            //! operator !=
            bool operator !=(const CModulator &other) const;

            //! \copydoc CValueObject::compareImpl(otherBase)
            virtual int compareImpl(const CValueObject &otherBase) const override;

            //! COM1
            static const QString &NameCom1()
            {
                static QString n("COM1");
                return n;
            }

            //! COM2
            static const QString &NameCom2()
            {
                static QString n("COM2");
                return n;
            }

            //! COM3
            static const QString &NameCom3()
            {
                static QString n("COM3");
                return n;
            }

            //! NAV1
            static const QString &NameNav1()
            {
                static QString n("NAV1");
                return n;
            }

            //! NAV2
            static const QString &NameNav2()
            {
                static QString n("NAV2");
                return n;
            }

            //! NAV3
            static const QString &NameNav3()
            {
                static QString n("NAV3");
                return n;
            }

            //! ADF1
            static const QString &NameAdf1()
            {
                static QString n("ADF1");
                return n;
            }

            //! ADF2
            static const QString &NameAdf2()
            {
                static QString n("ADF2");
                return n;
            }

            //! Frequency not set
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

        private:
            BLACK_ENABLE_TUPLE_CONVERSION(CModulator)
            BlackMisc::PhysicalQuantities::CFrequency m_frequencyActive;  //!< active frequency
            BlackMisc::PhysicalQuantities::CFrequency m_frequencyStandby; //!< standby frequency
            qint32 m_volumeInput;  //!< volume input
            qint32 m_volumeOutput; //!< volume output
            bool m_enabled; //!< is enabled, used e.g. for mute etc.

        };

    } // namespace
} // namespace

BLACK_DECLARE_TUPLE_CONVERSION_TEMPLATE(BlackMisc::Aviation::CModulator, (o.m_frequencyActive, o.m_frequencyStandby, o.m_volumeInput , o.m_volumeOutput, o.m_enabled))

#endif // guard

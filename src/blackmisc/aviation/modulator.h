/* Copyright (C) 2013
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

//! \file

#ifndef BLACKMISC_AVIATION_MODULATORUNIT_H
#define BLACKMISC_AVIATION_MODULATORUNIT_H

#include "blackmisc/propertyindex.h"
#include "blackmisc/aviation/avionicsbase.h"
#include "blackmisc/mathematics.h"
#include "blackmisc/blackmiscfreefunctions.h"

namespace BlackMisc
{
    namespace Aviation
    {

        //! Base class for COM, NAV, Squawk units.
        template <class AVIO> class CModulator : public CValueObject<CModulator<AVIO>, CAvionicsBase>
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

            //! Default value?
            virtual bool isDefaultValue() const
            {
                return (this->m_frequencyActive == FrequencyNotSet());
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
            virtual void setFrequencyActive(const BlackMisc::PhysicalQuantities::CFrequency &frequency)
            {
                this->m_frequencyActive = frequency;
            }

            //! Set standby frequency
            virtual void setFrequencyStandby(const BlackMisc::PhysicalQuantities::CFrequency &frequency)
            {
                this->m_frequencyStandby = frequency;
            }

            //! Output volume 0..100
            qint32 getVolumeOutput() const { return this->m_volumeOutput; }

            //! Input volume 0..100
            qint32 getVolumeInput() const { return this->m_volumeInput; }

            //! Output volume 0.100
            void setVolumeOutput(qint32 volume) { this->m_volumeOutput = volume; }

            //! Input volume 0..100
            void setVolumeInput(qint32 volume) { this->m_volumeInput = volume; }

            //! Enabled?
            bool isEnabled() const { return this->m_enabled;}

            //! Enabled?
            void setEnabled(bool enable) { this->m_enabled = enable;}

            //! \copydoc CValueObject::propertyByIndex
            virtual CVariant propertyByIndex(const BlackMisc::CPropertyIndex &index) const override;

            //! \copydoc CValueObject::setPropertyByIndex
            virtual void setPropertyByIndex(const CVariant &variant, const BlackMisc::CPropertyIndex &index) override;

        protected:
            //! Default constructor
            CModulator() :
                CModulator::CValueObject("default") {}

            //! Constructor
            CModulator(const QString &name, const BlackMisc::PhysicalQuantities::CFrequency &activeFrequency, const BlackMisc::PhysicalQuantities::CFrequency &standbyFrequency) :
                CModulator::CValueObject(name), m_frequencyActive(activeFrequency), m_frequencyStandby(standbyFrequency) {}

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
            virtual void setFrequencyActiveMHz(double frequencyMHz)
            {
                frequencyMHz = Math::CMath::round(frequencyMHz, 3);
                this->m_frequencyActive = BlackMisc::PhysicalQuantities::CFrequency(frequencyMHz, BlackMisc::PhysicalQuantities::CFrequencyUnit::MHz());
            }

            //! Set standby frequency
            virtual void setFrequencyStandbyMHz(double frequencyMHz)
            {
                frequencyMHz = Math::CMath::round(frequencyMHz, 3);
                this->m_frequencyStandby = BlackMisc::PhysicalQuantities::CFrequency(frequencyMHz, BlackMisc::PhysicalQuantities::CFrequencyUnit::MHz());
            }

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

        private:
            BLACK_ENABLE_TUPLE_CONVERSION(CModulator)
            BlackMisc::PhysicalQuantities::CFrequency m_frequencyActive;  //!< active frequency
            BlackMisc::PhysicalQuantities::CFrequency m_frequencyStandby; //!< standby frequency
            qint32 m_volumeInput = 0;  //!< volume input
            qint32 m_volumeOutput = 0; //!< volume output
            bool m_enabled = true;     //!< is enabled, used e.g. for mute etc.

            //! Easy access to derived class (CRTP template parameter)
            AVIO const *derived() const { return static_cast<AVIO const *>(this); }

            //! Easy access to derived class (CRTP template parameter)
            AVIO *derived() { return static_cast<AVIO *>(this); }
        };

    }
}

BLACK_DECLARE_TUPLE_CONVERSION_TEMPLATE(BlackMisc::Aviation::CModulator, (
                                            o.m_frequencyActive,
                                            o.m_frequencyStandby,
                                            o.m_volumeInput ,
                                            o.m_volumeOutput,
                                            o.m_enabled
                                        ))

#endif // guard

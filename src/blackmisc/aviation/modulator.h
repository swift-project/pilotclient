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
#include "blackmisc/math/mathutils.h"
#include "blackmisc/blackmiscfreefunctions.h"

namespace BlackMisc
{
    namespace Aviation
    {

        class CComSystem;
        class CNavSystem;
        class CAdfSystem;

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
            virtual bool isDefaultValue() const;

            //! Toggle active and standby frequencies
            void toggleActiveStandby();

            //! Active frequency
            BlackMisc::PhysicalQuantities::CFrequency getFrequencyActive() const;

            //! Standby frequency
            BlackMisc::PhysicalQuantities::CFrequency getFrequencyStandby() const;

            //! Set active frequency
            virtual void setFrequencyActive(const BlackMisc::PhysicalQuantities::CFrequency &frequency);

            //! Set standby frequency
            virtual void setFrequencyStandby(const BlackMisc::PhysicalQuantities::CFrequency &frequency);

            //! Output volume 0..100
            qint32 getVolumeOutput() const;

            //! Input volume 0..100
            qint32 getVolumeInput() const;

            //! Output volume 0.100
            void setVolumeOutput(qint32 volume);

            //! Input volume 0..100
            void setVolumeInput(qint32 volume);

            //! Enabled?
            bool isEnabled() const;

            //! Enabled?
            void setEnabled(bool enable);

            //! \copydoc CValueObject::propertyByIndex
            CVariant propertyByIndex(const BlackMisc::CPropertyIndex &index) const;

            //! \copydoc CValueObject::setPropertyByIndex
            void setPropertyByIndex(const CVariant &variant, const BlackMisc::CPropertyIndex &index);

            //! \copydoc CValueObject::convertToQString
            QString convertToQString(bool i18n = false) const;

        protected:
            //! Default constructor
            CModulator();

            //! Constructor
            CModulator(const QString &name, const BlackMisc::PhysicalQuantities::CFrequency &activeFrequency, const BlackMisc::PhysicalQuantities::CFrequency &standbyFrequency);

            //! Set active frequency
            void setFrequencyActiveKHz(double frequencyKHz);

            //! Set standby frequency
            void setFrequencyStandbyKHz(double frequencyKHz);

            //! Set active frequency
            virtual void setFrequencyActiveMHz(double frequencyMHz);

            //! Set standby frequency
            virtual void setFrequencyStandbyMHz(double frequencyMHz);

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

            //! ADF1
            static const QString &NameAdf1();

            //! ADF2
            static const QString &NameAdf2();

            //! Frequency not set
            static const BlackMisc::PhysicalQuantities::CFrequency &FrequencyNotSet();

        private:
            BLACK_ENABLE_TUPLE_CONVERSION(CModulator)
            BlackMisc::PhysicalQuantities::CFrequency m_frequencyActive;  //!< active frequency
            BlackMisc::PhysicalQuantities::CFrequency m_frequencyStandby; //!< standby frequency
            qint32 m_volumeInput = 0;  //!< volume input
            qint32 m_volumeOutput = 0; //!< volume output
            bool m_enabled = true;     //!< is enabled, used e.g. for mute etc.

            //! Easy access to derived class (CRTP template parameter)
            AVIO const *derived() const;

            //! Easy access to derived class (CRTP template parameter)
            AVIO *derived();
        };

        //! \cond PRIVATE
        extern template class BLACKMISC_EXPORT_TEMPLATE CModulator<CComSystem>;
        extern template class BLACKMISC_EXPORT_TEMPLATE CModulator<CNavSystem>;
        extern template class BLACKMISC_EXPORT_TEMPLATE CModulator<CAdfSystem>;
        //! \endcond

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

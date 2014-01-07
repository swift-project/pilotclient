/*  Copyright (C) 2013 VATSIM Community / contributors
 *  This Source Code Form is subject to the terms of the Mozilla Public
 *  License, v. 2.0. If a copy of the MPL was not distributed with this
 *  file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#include "blackmisc/aviomodulator.h"
#include "blackmisc/aviocomsystem.h"
#include "blackmisc/avionavsystem.h"
#include "blackmisc/avioadfsystem.h"
#include "blackmisc/blackmiscfreefunctions.h"

using BlackMisc::PhysicalQuantities::CFrequency;
using BlackMisc::PhysicalQuantities::CFrequencyUnit;

namespace BlackMisc
{
    namespace Aviation
    {
        /*
         * Toggle standby <-> active
         */
        template <class AVIO> void CModulator<AVIO>::toggleActiveStandby()
        {
            CFrequency a = this->m_frequencyActive;
            this->m_frequencyActive = this->m_frequencyStandby;
            this->m_frequencyStandby = a;
        }

        /*
         * Register metadata
         */
        template <class AVIO> void CModulator<AVIO>::registerMetadata()
        {
            qRegisterMetaType<AVIO>();
            qDBusRegisterMetaType<AVIO>();
        }

        /*
         * Equal operator ==
         */
        template <class AVIO> bool CModulator<AVIO>::operator ==(const CModulator<AVIO> &other) const
        {
            if (this == &other) return true;
            return (this->getName() == other.getName() &&
                    this->m_frequencyActive == other.m_frequencyActive &&
                    this->m_frequencyStandby == other.m_frequencyStandby);
        }

        /*
         * Equal operator !=
         */
        template <class AVIO> bool CModulator<AVIO>::operator !=(const CModulator<AVIO> &other) const
        {
            return !(other == (*this));
        }

        /*
         * To DBus
         */
        template <class AVIO> void CModulator<AVIO>::marshallToDbus(QDBusArgument &argument) const
        {
            this->CAvionicsBase::marshallToDbus(argument);
            argument << this->m_frequencyActive;
            argument << this->m_frequencyStandby;
            argument << this->m_digits;
        }

        /*
         * From DBus
         */
        template <class AVIO> void CModulator<AVIO>::unmarshallFromDbus(const QDBusArgument &argument)
        {
            this->CAvionicsBase::unmarshallFromDbus(argument);
            argument >> this->m_frequencyActive;
            argument >> this->m_frequencyStandby;
            argument >> this->m_digits;
        }

        /*
         * Value hash
         */
        template <class AVIO> uint CModulator<AVIO>::getValueHash() const
        {
            QList<uint> hashs;
            hashs << this->m_frequencyActive.getValueHash();
            hashs << this->m_frequencyStandby.getValueHash();
            hashs << qHash(this->m_digits);
            return BlackMisc::calculateHash(hashs, "CModulator");
        }

        // see here for the reason of thess forward instantiations
        // http://www.parashift.com/c++-faq/separate-template-class-defn-from-decl.html
        template class CModulator<CComSystem>;
        template class CModulator<CNavSystem>;
        template class CModulator<CAdfSystem>;

    } // namespace
} // namespace

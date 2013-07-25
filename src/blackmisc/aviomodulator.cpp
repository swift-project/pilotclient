/*  Copyright (C) 2013 VATSIM Community / contributors
 *  This Source Code Form is subject to the terms of the Mozilla Public
 *  License, v. 2.0. If a copy of the MPL was not distributed with this
 *  file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#include "blackmisc/aviomodulator.h"
#include "blackmisc/aviocomsystem.h"
#include "blackmisc/avionavsystem.h"
#include "blackmisc/avioadfsystem.h"

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
    qRegisterMetaType<AVIO>(typeid(AVIO).name());
    qDBusRegisterMetaType<AVIO>();
}

/*
 * Assigment operator =
 */
template <class AVIO> CModulator<AVIO>& CModulator<AVIO>::operator=(const CModulator<AVIO> &otherModulator)
{
    if (this == &otherModulator)  return *this; // Same object?
    this->m_frequencyActive = otherModulator.m_frequencyActive;
    this->m_frequencyStandby = otherModulator.m_frequencyStandby;
    this->m_digits = otherModulator.m_digits;
    this->setName(otherModulator.getName());
    return *this;
}

/*
 * Equal operator ==
 */
template <class AVIO> bool CModulator<AVIO>::operator ==(const CModulator<AVIO> &otherModulator) const
{
    if (this == &otherModulator) return true;
    return (this->getName() == otherModulator.getName() &&
            this->m_frequencyActive == otherModulator.m_frequencyActive &&
            this->m_frequencyStandby == otherModulator.m_frequencyStandby);
}

/*
 * Equal operator !=
 */
template <class AVIO> bool CModulator<AVIO>::operator !=(const CModulator<AVIO> &otherModulator) const
{
    return !(otherModulator == (*this));
}

// see here for the reason of thess forward instantiations
// http://www.parashift.com/c++-faq/separate-template-class-defn-from-decl.html
template class CModulator<CComSystem>;
template class CModulator<CNavSystem>;
template class CModulator<CAdfSystem>;

} // namespace
} // namespace

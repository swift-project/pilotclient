/*  Copyright (C) 2013 VATSIM Community / contributors
 *  This Source Code Form is subject to the terms of the Mozilla Public
 *  License, v. 2.0. If a copy of the MPL was not distributed with this
 *  file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#include "blackmisc/aviomodulator.h"
#include "blackmisc/aviocomsystem.h"
#include "blackmisc/avionavsystem.h"

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
 * String representation
 */
template <class AVIO> QString CModulator<AVIO>::stringForConverter() const
{
    QString s(this->getName());
    s.append(" Active: ").append(this->m_frequencyActive.unitValueRoundedWithUnit(3));
    s.append(" Standby: ").append(this->m_frequencyStandby.unitValueRoundedWithUnit(3));
    return s;
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

} // namespace
} // namespace

/* Copyright (C) 2018
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

#include "aircraftmatchersetup.h"

namespace BlackMisc
{
    namespace Simulation
    {
        QString CAircraftMatcherSetup::convertToQString(bool i18n) const
        {
            Q_UNUSED(i18n);
            return modeToString(this->getMatchingMode());
        }

        CVariant CAircraftMatcherSetup::propertyByIndex(const CPropertyIndex &index) const
        {
            if (index.isMyself()) { return CVariant::from(*this); }
            const ColumnIndex i = index.frontCasted<ColumnIndex>();
            switch (i)
            {
            case IndexMatchingMode: return CVariant::fromValue(m_mode);
            default: break;
            }
            return CValueObject::propertyByIndex(index);
        }

        void CAircraftMatcherSetup::setPropertyByIndex(const CPropertyIndex &index, const CVariant &variant)
        {
            if (index.isMyself()) { (*this) = variant.to<CAircraftMatcherSetup>(); return; }
            const ColumnIndex i = index.frontCasted<ColumnIndex>();
            switch (i)
            {
            case IndexMatchingMode: m_mode = variant.toInt(); break;
            default: break;
            }
            CValueObject::setPropertyByIndex(index, variant);
        }

        const QString &CAircraftMatcherSetup::modeFlagToString(MatchingModeFlag modeFlag)
        {
            static const QString ms("by model string");
            static const QString icao("by ICAO");
            static const QString family("by family");
            static const QString livery("by livery");
            static const QString combined("by combined combined");

            switch (modeFlag)
            {
            case ByModelString:  return ms;
            case ByIcaoData:     return icao;
            case ByFamily:       return family;
            case ByLivery:       return livery;
            case ByCombinedType: return combined;
            default: break;
            }

            static const QString unknown("unknown");
            return unknown;
        }

        QString CAircraftMatcherSetup::modeToString(MatchingMode mode)
        {
            if (mode == ModeAll) { return "all"; }

            QStringList modes;
            if (mode.testFlag(ByModelString))  { modes << modeFlagToString(ByModelString); }
            if (mode.testFlag(ByIcaoData))     { modes << modeFlagToString(ByIcaoData); }
            if (mode.testFlag(ByFamily))       { modes << modeFlagToString(ByFamily); }
            if (mode.testFlag(ByLivery))       { modes << modeFlagToString(ByLivery); }
            if (mode.testFlag(ByCombinedType)) { modes << modeFlagToString(ByCombinedType); }
            return modes.join(", ");
        }

        CAircraftMatcherSetup::MatchingMode CAircraftMatcherSetup::matchingMode(bool byModelString, bool byIcaoData, bool byFamily, bool byLivery, bool byCombinedType)
        {
            MatchingMode mode = byModelString ? ByModelString : ModeNone;
            if (byIcaoData)     { mode |= ByIcaoData; }
            if (byFamily)       { mode |= ByFamily; }
            if (byLivery)       { mode |= ByLivery; }
            if (byCombinedType) { mode |= ByCombinedType; }
            return mode;
        }
    } // namespace
} // namespace

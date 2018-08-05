/* Copyright (C) 2018
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

//! \file

#ifndef BLACKMISC_SIMULATION_AIRCRAFTMATCHERSETUP_H
#define BLACKMISC_SIMULATION_AIRCRAFTMATCHERSETUP_H

#include "blackmisc/valueobject.h"
#include "blackmisc/blackmiscexport.h"


namespace BlackMisc
{
    namespace Simulation
    {
        //! Matcher settings
        class BLACKMISC_EXPORT CAircraftMatcherSetup : public CValueObject<CAircraftMatcherSetup>
        {
        public:
            //! Enabled matching mode flags
            enum MatchingModeFlag
            {
                ByModelString    = 1 << 0,
                ByIcaoData       = 1 << 1,
                ByFamily         = 1 << 2,
                ByLivery         = 1 << 3,
                ByCombinedType   = 1 << 4,
                ModeAll          = ByModelString | ByIcaoData | ByFamily | ByLivery | ByCombinedType,
                ModeNone         = 0
            };
            Q_DECLARE_FLAGS(MatchingMode, MatchingModeFlag)

            //! Properties by index
            enum ColumnIndex
            {
                IndexMatchingMode = CPropertyIndex::GlobalIndexCAircraftMatcherSetup
            };

            //! Constructor
            CAircraftMatcherSetup() {}

            //! Matching mode
            MatchingMode getMatchingMode() const { return static_cast<MatchingMode>(m_mode); }

            //! Dynamic offset values?
            void setMatchingMode(MatchingMode mode) { m_mode = static_cast<int>(mode); }

            //! \copydoc BlackMisc::Mixin::String::toQString
            QString convertToQString(bool i18n = false) const;

            //! \copydoc BlackMisc::Mixin::Index::propertyByIndex
            BlackMisc::CVariant propertyByIndex(const BlackMisc::CPropertyIndex &index) const;

            //! \copydoc BlackMisc::Mixin::Index::setPropertyByIndex
            void setPropertyByIndex(const BlackMisc::CPropertyIndex &index, const BlackMisc::CVariant &variant);

            //! Enumeration as string
            static const QString &modeFlagToString(MatchingModeFlag modeFlag);

            //! Enumeration as string
            static QString modeToString(MatchingMode mode);

            //! Mode by flags
            static MatchingMode matchingMode(bool byModelString, bool byIcaoData, bool byFamily, bool byLivery, bool byCombinedType);

        private:
            int m_mode = static_cast<int>(ModeAll);

            BLACK_METACLASS(
                CAircraftMatcherSetup,
                BLACK_METAMEMBER(mode)
            );
        };
    } // ns
} // namespace

Q_DECLARE_METATYPE(BlackMisc::Simulation::CAircraftMatcherSetup)
Q_DECLARE_METATYPE(BlackMisc::Simulation::CAircraftMatcherSetup::MatchingMode)
Q_DECLARE_METATYPE(BlackMisc::Simulation::CAircraftMatcherSetup::MatchingModeFlag)
Q_DECLARE_OPERATORS_FOR_FLAGS(BlackMisc::Simulation::CAircraftMatcherSetup::MatchingMode)

#endif // guard

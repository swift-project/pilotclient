/* Copyright (C) 2015
 * swift Project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

#include "blackmisc/avcallsignobjectlist.h"
#include "blackmisc/predicates.h"
#include "blackmisc/avatcstationlist.h"
#include "blackmisc/avaircraftlist.h"
#include "blackmisc/avaircraftsituationlist.h"
#include "blackmisc/simulation/simulatedaircraftlist.h"

namespace BlackMisc
{
    namespace Aviation
    {
        template <class OBJ, class CONTAINER>
        ICallsignObjectList<OBJ, CONTAINER>::ICallsignObjectList()
        { }

        template <class OBJ, class CONTAINER>
        bool ICallsignObjectList<OBJ, CONTAINER>::containsCallsign(const CCallsign &callsign) const
        {
            return this->getContainer().contains(&OBJ::getCallsign, callsign);
        }

        template <class OBJ, class CONTAINER>
        int ICallsignObjectList<OBJ, CONTAINER>::applyIfCallsign(const CCallsign &callsign, const CPropertyIndexVariantMap &variantMap)
        {
            return this->getContainer().applyIf(&OBJ::getCallsign, callsign, variantMap);
        }

        template <class OBJ, class CONTAINER>
        CONTAINER ICallsignObjectList<OBJ, CONTAINER>::findByCallsign(const CCallsign &callsign) const
        {
            return this->getContainer().findBy(&OBJ::getCallsign, callsign);
        }

        template <class OBJ, class CONTAINER>
        CONTAINER ICallsignObjectList<OBJ, CONTAINER>::findByCallsigns(const CCallsignList &callsigns) const
        {
            return this->getContainer().findBy(Predicates::MemberIsAnyOf(&OBJ::getCallsign, callsigns));
        }

        template <class OBJ, class CONTAINER>
        OBJ ICallsignObjectList<OBJ, CONTAINER>::findFirstByCallsign(const CCallsign &callsign, const OBJ &ifNotFound) const
        {
            return this->findByCallsign(callsign).frontOrDefault(ifNotFound);
        }

        template <class OBJ, class CONTAINER>
        OBJ ICallsignObjectList<OBJ, CONTAINER>::findBackByCallsign(const CCallsign &callsign, const OBJ &ifNotFound) const
        {
            return this->findByCallsign(callsign).backOrDefault(ifNotFound);
        }

        template <class OBJ, class CONTAINER>
        CONTAINER ICallsignObjectList<OBJ, CONTAINER>::findBySuffix(const QString &suffix) const
        {
            CONTAINER r;
            if (suffix.isEmpty()) { return r; }
            QString sfxUpper(suffix.trimmed().toUpper());
            r = this->getContainer().findBy([ = ](const OBJ & csObj) -> bool
            {
                return (csObj.getCallsign().getSuffix() == sfxUpper);
            });
            return r;
        }

        template <class OBJ, class CONTAINER>
        QMap<QString, int> ICallsignObjectList<OBJ, CONTAINER>::getSuffixes() const
        {
            QMap<QString, int> r;
            for (const OBJ &csObj : this->getContainer())
            {
                const QString s = csObj.getCallsign().getSuffix();
                if (s.isEmpty()) { continue; }
                if (r.contains(s))
                {
                    r[s] = r[s] + 1;
                }
                else
                {
                    r.insert(s, 1);
                }
            }
            return r;
        }

        template <class OBJ, class CONTAINER>
        int ICallsignObjectList<OBJ, CONTAINER>::incrementalUpdateOrAdd(const OBJ &objectBeforeChanges, const CPropertyIndexVariantMap &changedValues)
        {
            int c;
            const CCallsign cs = objectBeforeChanges.getCallsign();
            if (this->containsCallsign(cs))
            {
                if (changedValues.isEmpty()) { return 0; }
                c = this->getContainer().applyIf(&OBJ::getCallsign, cs, changedValues);
            }
            else
            {
                c = 1;
                if (changedValues.isEmpty())
                {
                    this->getContainer().push_back(objectBeforeChanges);
                }
                else
                {
                    OBJ objectAdded(objectBeforeChanges);
                    objectAdded.apply(changedValues);
                    this->getContainer().push_back(objectAdded);
                }
            }
            return c;
        }

        // see here for the reason of thess forward instantiations
        // http://www.parashift.com/c++-faq/separate-template-class-defn-from-decl.html
        template class ICallsignObjectList<BlackMisc::Aviation::CAtcStation, BlackMisc::Aviation::CAtcStationList>;
        template class ICallsignObjectList<BlackMisc::Aviation::CAircraft, BlackMisc::Aviation::CAircraftList>;
        template class ICallsignObjectList<BlackMisc::Aviation::CAircraftSituation, BlackMisc::Aviation::CAircraftSituationList>;
        template class ICallsignObjectList<BlackMisc::Simulation::CSimulatedAircraft, BlackMisc::Simulation::CSimulatedAircraftList>;

    } // namespace
} // namespace


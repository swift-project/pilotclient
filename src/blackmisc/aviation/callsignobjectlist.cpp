/* Copyright (C) 2015
 * swift Project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

#include "blackmisc/simulation/simulatedaircraftlist.h"
#include "blackmisc/simulation/interpolationsetuplist.h"
#include "blackmisc/aviation/callsignobjectlist.h"
#include "blackmisc/aviation/callsign.h"
#include "blackmisc/aviation/atcstationlist.h"
#include "blackmisc/aviation/aircraftsituationlist.h"
#include "blackmisc/aviation/flightplanlist.h"
#include "blackmisc/network/clientlist.h"
#include "blackmisc/predicates.h"

#include <QtGlobal>

namespace BlackMisc
{
    namespace Aviation
    {
        template <class OBJ, class CONTAINER>
        ICallsignObjectList<OBJ, CONTAINER>::ICallsignObjectList()
        { }

        template <class OBJ, class CONTAINER>
        const CONTAINER &ICallsignObjectList<OBJ, CONTAINER>::container() const
        {
            return static_cast<const CONTAINER &>(*this);
        }

        template <class OBJ, class CONTAINER>
        CONTAINER &ICallsignObjectList<OBJ, CONTAINER>::container()
        {
            return static_cast<CONTAINER &>(*this);
        }

        template <class OBJ, class CONTAINER>
        bool ICallsignObjectList<OBJ, CONTAINER>::containsCallsign(const CCallsign &callsign) const
        {
            return this->container().contains(&OBJ::getCallsign, callsign);
        }

        template <class OBJ, class CONTAINER>
        int ICallsignObjectList<OBJ, CONTAINER>::applyIfCallsign(const CCallsign &callsign, const CPropertyIndexVariantMap &variantMap, bool skipEqualValues)
        {
            return this->container().applyIf(&OBJ::getCallsign, callsign, variantMap, skipEqualValues);
        }

        template <class OBJ, class CONTAINER>
        CCallsignSet ICallsignObjectList<OBJ, CONTAINER>::getCallsigns() const
        {
            CCallsignSet cs;
            for (const OBJ &obj : this->container())
            {
                cs.push_back(obj.getCallsign());
            }
            return cs;
        }

        template<class OBJ, class CONTAINER>
        QStringList ICallsignObjectList<OBJ, CONTAINER>::getCallsignStrings(bool sorted) const
        {
            return this->getCallsigns().getCallsignStrings(sorted);
        }

        template <class OBJ, class CONTAINER>
        CONTAINER ICallsignObjectList<OBJ, CONTAINER>::findByCallsign(const CCallsign &callsign) const
        {
            return this->container().findBy(&OBJ::getCallsign, callsign);
        }

        template <class OBJ, class CONTAINER>
        CONTAINER ICallsignObjectList<OBJ, CONTAINER>::findByCallsigns(const CCallsignSet &callsigns) const
        {
            return this->container().findBy(Predicates::MemberIsAnyOf(&OBJ::getCallsign, callsigns));
        }

        template <class OBJ, class CONTAINER>
        OBJ ICallsignObjectList<OBJ, CONTAINER>::findFirstByCallsign(const CCallsign &callsign, const OBJ &ifNotFound) const
        {
            return this->container().findFirstByOrDefault(&OBJ::getCallsign, callsign, ifNotFound);
        }

        template <class OBJ, class CONTAINER>
        OBJ ICallsignObjectList<OBJ, CONTAINER>::findLastByCallsign(const CCallsign &callsign, const OBJ &ifNotFound) const
        {
            for (auto current = container().end(); current != container().begin() ; /* Do nothing */)
            {
                --current;
                if (current->getCallsign() == callsign) { return *current; }
            }
            return ifNotFound;
        }

        template <class OBJ, class CONTAINER>
        CONTAINER ICallsignObjectList<OBJ, CONTAINER>::findBySuffix(const QString &suffix) const
        {
            CONTAINER r;
            if (suffix.isEmpty()) { return r; }
            const QString sfxUpper(suffix.trimmed().toUpper());
            r = this->container().findBy([ = ](const OBJ & csObj)
            {
                return (csObj.getCallsign().getSuffix() == sfxUpper);
            });
            return r;
        }

        template <class OBJ, class CONTAINER>
        int ICallsignObjectList<OBJ, CONTAINER>::firstIndexOfCallsign(const CCallsign &callsign)
        {
            for (int i = 0; i < this->container().size(); i++)
            {
                if (this->container()[i].getCallsign() == callsign) { return i; }
            }
            return -1;
        }

        template <class OBJ, class CONTAINER>
        int ICallsignObjectList<OBJ, CONTAINER>::removeByCallsign(const CCallsign &callsign)
        {
            return this->container().removeIf(&OBJ::getCallsign, callsign);
        }

        template <class OBJ, class CONTAINER>
        int ICallsignObjectList<OBJ, CONTAINER>::removeByCallsigns(const CCallsignSet &callsigns)
        {
            return this->container().removeIf([ & ](const OBJ & obj) { return callsigns.contains(obj.getCallsign()); });
        }

        template <class OBJ, class CONTAINER>
        QMap<QString, int> ICallsignObjectList<OBJ, CONTAINER>::getSuffixes() const
        {
            QMap<QString, int> r;
            for (const OBJ &csObj : this->container())
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
        QHash<CCallsign, CONTAINER> ICallsignObjectList<OBJ, CONTAINER>::splitPerCallsign() const
        {
            CONTAINER copyContainer(container());
            copyContainer.sortByCallsign();
            QHash<CCallsign, CONTAINER> result;
            CCallsign cs;
            for (const OBJ &csObj : copyContainer)
            {
                if (csObj.getCallsign().isEmpty())
                {
                    Q_ASSERT(false); // there should be no empty callsigns
                    continue;
                }
                if (cs != csObj.getCallsign())
                {
                    cs = csObj.getCallsign();
                    CONTAINER perCallsign({ csObj });
                    result.insert(cs, perCallsign);
                }
                else
                {
                    result[cs].push_back(csObj);
                }
            }
            return result;
        }

        template<class OBJ, class CONTAINER>
        int ICallsignObjectList<OBJ, CONTAINER>::replaceOrAddObjectByCallsign(const OBJ &otherObject)
        {
            const CCallsign cs(otherObject.getCallsign());
            if (cs.isEmpty()) { return 0; }
            CONTAINER &copy(this->container());
            copy.removeByCallsign(cs);
            copy.push_back(otherObject);
            return 1;
        }

        template<class OBJ, class CONTAINER>
        int ICallsignObjectList<OBJ, CONTAINER>::replaceOrAddObjectsByCallsign(const CONTAINER &others)
        {
            if (others.isEmpty()) { return 0; }
            int c = 0;
            CONTAINER copy(this->container());
            for (const OBJ &obj : others)
            {
                const CCallsign cs(obj.getCallsign());
                if (cs.isEmpty()) { continue; }
                copy.removeByCallsign(cs);
                copy.push_back(obj);
                c++;
            }
            *this = copy;
            return c;
        }

        template <class OBJ, class CONTAINER>
        void ICallsignObjectList<OBJ, CONTAINER>::sortByCallsign()
        {
            container().sortBy(&OBJ::getCallsign);
        }

        template<class OBJ, class CONTAINER>
        QMap<CCallsign, OBJ> ICallsignObjectList<OBJ, CONTAINER>::asCallsignMap() const
        {
            QMap<CCallsign, OBJ> map;
            for (const OBJ &obj : this->container())
            {
                map.insert(obj.getCallsign(), obj);
            }
            return map;
        }

        template<class OBJ, class CONTAINER>
        QHash<CCallsign, OBJ> ICallsignObjectList<OBJ, CONTAINER>::asCallsignHash() const
        {
            QHash<CCallsign, OBJ> hash;
            for (const OBJ &obj : this->container())
            {
                hash.insert(obj.getCallsign(), obj);
            }
            return hash;
        }

        template<class OBJ, class CONTAINER>
        CONTAINER ICallsignObjectList<OBJ, CONTAINER>::sortedByCallsign() const
        {
            CONTAINER copy(this->container());
            copy.sortByCallsign();
            return copy;
        }

        template <class OBJ, class CONTAINER>
        int ICallsignObjectList<OBJ, CONTAINER>::incrementalUpdateOrAdd(const OBJ &objectBeforeChanges, const CPropertyIndexVariantMap &changedValues)
        {
            int c;
            const CCallsign cs = objectBeforeChanges.getCallsign();
            if (this->containsCallsign(cs))
            {
                if (changedValues.isEmpty()) { return 0; }
                c = this->container().applyIf(&OBJ::getCallsign, cs, changedValues);
            }
            else
            {
                c = 1;
                if (changedValues.isEmpty())
                {
                    this->container().push_back(objectBeforeChanges);
                }
                else
                {
                    OBJ objectAdded(objectBeforeChanges);
                    objectAdded.apply(changedValues);
                    this->container().push_back(objectAdded);
                }
            }
            return c;
        }

        // see here for the reason of thess forward instantiations
        // https://isocpp.org/wiki/faq/templates#separate-template-fn-defn-from-decl
        //! \cond PRIVATE
        template class BLACKMISC_EXPORT_DEFINE_TEMPLATE ICallsignObjectList<BlackMisc::Aviation::CAtcStation, BlackMisc::Aviation::CAtcStationList>;
        template class BLACKMISC_EXPORT_DEFINE_TEMPLATE ICallsignObjectList<BlackMisc::Aviation::CAircraftSituation, BlackMisc::Aviation::CAircraftSituationList>;
        template class BLACKMISC_EXPORT_DEFINE_TEMPLATE ICallsignObjectList<BlackMisc::Aviation::CFlightPlan, BlackMisc::Aviation::CFlightPlanList>;
        template class BLACKMISC_EXPORT_DEFINE_TEMPLATE ICallsignObjectList<BlackMisc::Simulation::CSimulatedAircraft, BlackMisc::Simulation::CSimulatedAircraftList>;
        template class BLACKMISC_EXPORT_DEFINE_TEMPLATE ICallsignObjectList<BlackMisc::Simulation::CInterpolationAndRenderingSetupPerCallsign, BlackMisc::Simulation::CInterpolationSetupList>;
        template class BLACKMISC_EXPORT_DEFINE_TEMPLATE ICallsignObjectList<BlackMisc::Network::CClient, BlackMisc::Network::CClientList>;
        //! \endcond

    } // namespace
} // namespace

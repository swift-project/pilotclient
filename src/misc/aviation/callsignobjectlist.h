// SPDX-FileCopyrightText: Copyright (C) 2015 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

//! \file

#ifndef SWIFT_MISC_AVIATION_CALLSIGNOBJECTLIST_H
#define SWIFT_MISC_AVIATION_CALLSIGNOBJECTLIST_H

#include <QHash>
#include <QMap>
#include <QtGlobal>

#include "misc/aviation/callsignset.h"
#include "misc/predicates.h"
#include "misc/propertyindexvariantmap.h"

namespace swift::misc::aviation
{
    //! List of objects with callsign.
    template <class OBJ, class CONTAINER>
    class ICallsignObjectList
    {
    public:
        //! Contains callsign?
        bool containsCallsign(const CCallsign &callsign) const
        {
            return this->container().contains(&OBJ::getCallsign, callsign);
        }

        //! Apply for given callsign
        int applyIfCallsign(const CCallsign &callsign, const CPropertyIndexVariantMap &variantMap,
                            bool skipEqualValues = true)
        {
            return this->container().applyIf(&OBJ::getCallsign, callsign, variantMap, skipEqualValues);
        }

        //! All callsigns
        swift::misc::aviation::CCallsignSet getCallsigns() const
        {
            CCallsignSet cs;
            for (const OBJ &obj : this->container()) { cs.push_back(obj.getCallsign()); }
            return cs;
        }

        //! Get callsign string list
        QStringList getCallsignStrings(bool sorted = false) const
        {
            return this->getCallsigns().getCallsignStrings(sorted);
        }

        //! Get callsigns as strings
        QString getCallsignsAsString(const QString &separator, bool sorted = false) const
        {
            if (this->container().isEmpty()) { return QString(); }
            const QStringList callsigns = this->getCallsignStrings(sorted);
            return callsigns.join(separator);
        }

        //! Find 0..n stations by callsign
        CONTAINER findByCallsign(const CCallsign &callsign) const
        {
            return this->container().findBy(&OBJ::getCallsign, callsign);
        }

        //! Find 0..n aircraft matching any of a set of callsigns
        CONTAINER findByCallsigns(const CCallsignSet &callsigns) const
        {
            return this->container().findBy(predicates::MemberIsAnyOf(&OBJ::getCallsign, callsigns));
        }

        //! Find the first aircraft by callsign, if none return given one
        OBJ findFirstByCallsign(const CCallsign &callsign, const OBJ &ifNotFound = {}) const
        {
            return this->container().findFirstByOrDefault(&OBJ::getCallsign, callsign, ifNotFound);
        }

        //! Find the back object by callsign, if none return given one
        OBJ findLastByCallsign(const CCallsign &callsign, const OBJ &ifNotFound = {}) const
        {
            for (auto current = container().end(); current != container().begin(); /* Do nothing */)
            {
                --current;
                if (current->getCallsign() == callsign) { return *current; }
            }
            return ifNotFound;
        }

        //! All with given suffix, empty suffixes ignored
        CONTAINER findBySuffix(const QString &suffix) const
        {
            CONTAINER r;
            if (suffix.isEmpty()) { return r; }
            const QString sfxUpper(suffix.trimmed().toUpper());
            r = this->container().findBy(
                [=](const OBJ &csObj) { return (csObj.getCallsign().getSuffix() == sfxUpper); });
            return r;
        }

        //! First found index of callsign, otherwise -1
        int firstIndexOfCallsign(const CCallsign &callsign)
        {
            for (int i = 0; i < this->container().size(); i++)
            {
                if (this->container()[i].getCallsign() == callsign) { return i; }
            }
            return -1;
        }

        //! Remove all objects with callsign
        int removeByCallsign(const CCallsign &callsign)
        {
            return this->container().removeIf(&OBJ::getCallsign, callsign);
        }

        //! Remove all objects with callsigns
        int removeByCallsigns(const CCallsignSet &callsigns)
        {
            return this->container().removeIf([&](const OBJ &obj) { return callsigns.contains(obj.getCallsign()); });
        }

        //! All suffixes with their respective count
        //! \remark since using QMap sorted by suffix
        QMap<QString, int> getSuffixesAndCount() const
        {
            QMap<QString, int> r; // sorted by key
            for (const OBJ &csObj : this->container())
            {
                const QString s = csObj.getCallsign().getSuffix();
                if (s.isEmpty()) { continue; }
                if (r.contains(s)) { r[s] = r[s] + 1; }
                else { r.insert(s, 1); }
            }
            return r;
        }

        //! All suffixes, in the order of the list
        //! \remark first found suffixes first
        QStringList getSuffixes() const
        {
            QStringList suffixes;
            for (const OBJ &csObj : this->container())
            {
                const QString s = csObj.getCallsign().getSuffix();
                if (s.isEmpty() || suffixes.contains(s, Qt::CaseInsensitive)) { continue; }
                suffixes << s;
            }
            return suffixes;
        }

        //! Split into 0..n containers as per callsign
        QHash<CCallsign, CONTAINER> splitPerCallsign() const
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
                else { result[cs].push_back(csObj); }
            }
            return result;
        }

        //! Replace or add objects by callsign
        int replaceOrAddObjectByCallsign(const OBJ &otherObject)
        {
            const CCallsign cs(otherObject.getCallsign());
            if (cs.isEmpty()) { return 0; }
            CONTAINER &copy(this->container());
            copy.removeByCallsign(cs);
            copy.push_back(otherObject);
            return 1;
        }

        //! Replace or add objects by callsign
        int replaceOrAddObjectsByCallsign(const CONTAINER &others)
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

        //! Incremental update or add object
        int incrementalUpdateOrAdd(const OBJ &objectBeforeChanges, const CPropertyIndexVariantMap &changedValues)
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
                if (changedValues.isEmpty()) { this->container().push_back(objectBeforeChanges); }
                else
                {
                    OBJ objectAdded(objectBeforeChanges);
                    objectAdded.apply(changedValues);
                    this->container().push_back(objectAdded);
                }
            }
            return c;
        }

        //! Sort by callsign
        void sortByCallsign() { container().sortBy(&OBJ::getCallsign); }

        //! Turn into callsign map
        QMap<CCallsign, OBJ> asCallsignMap() const
        {
            QMap<CCallsign, OBJ> map;
            for (const OBJ &obj : this->container()) { map.insert(obj.getCallsign(), obj); }
            return map;
        }

        //! Turn into callsign hash
        QHash<CCallsign, OBJ> asCallsignHash() const
        {
            QHash<CCallsign, OBJ> hash;
            for (const OBJ &obj : this->container())
            {
                if (obj.getCallsign().isEmpty()) { continue; }
                hash.insert(obj.getCallsign(), obj);
            }
            return hash;
        }

        //! Copy of list sorted by callsign
        CONTAINER sortedByCallsign() const
        {
            CONTAINER copy(this->container());
            copy.sortByCallsign();
            return copy;
        }

    protected:
        //! Constructor
        ICallsignObjectList() {}

        //! Container
        const CONTAINER &container() const { return static_cast<const CONTAINER &>(*this); }

        //! Container
        CONTAINER &container() { return static_cast<CONTAINER &>(*this); }
    };
} // namespace swift::misc::aviation

#endif // guard

/* Copyright (C) 2015
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution. No part of swift project, including this file, may be copied, modified, propagated,
 * or distributed except according to the terms contained in the LICENSE file.
 */

#include "blackmisc/countrylist.h"

#include <QJsonValue>
#include <QRegularExpression>
#include <Qt>

BLACK_DEFINE_SEQUENCE_MIXINS(BlackMisc, CCountry, CCountryList)

namespace BlackMisc
{
    CCountryList::CCountryList() {}

    CCountryList::CCountryList(const CSequence<CCountry> &other) :
        CSequence<CCountry>(other)
    { }

    CCountry CCountryList::findByIsoCode(const QString &isoCode) const
    {
        const QString iso(isoCode.trimmed().toUpper());
        if (!CCountry::isValidIsoCode(iso)) { return CCountry(); }
        return IDatastoreObjectList::findByKey(isoCode);
    }

    CCountry CCountryList::findBestMatchByCountryName(const QString &candidate) const
    {
        if (candidate.isEmpty()) { return CCountry(); }

        thread_local const QRegularExpression reg("^[a-z]+", QRegularExpression::CaseInsensitiveOption);
        const QRegularExpressionMatch match = reg.match(candidate);
        const QString cn(match.hasMatch() ? match.captured(0) : candidate);
        const CCountryList countries = this->findBy([&](const CCountry & country)
        {
            return country.matchesCountryName(cn);
        });

        if (countries.isEmpty()) { return this->findFirstByAlias(cn); }
        if (countries.size() < 2) { return countries.frontOrDefault(); }

        // find best match by further reducing
        for (const CCountry &c : countries)
        {
            if (c.getName() == cn) { return c; }
            if (c.getName().startsWith(cn, Qt::CaseInsensitive)) { return c; }
            if (cn.startsWith(c.getName(), Qt::CaseInsensitive)) { return c; }
        }
        return countries.front();
    }

    CCountry CCountryList::findFirstByAlias(const QString &alias) const
    {
        if (alias.isEmpty()) { return CCountry(); }
        const QString a(alias.toUpper().trimmed());
        for (const CCountry &country : (*this))
        {
            if (country.matchesAlias(a)) { return country;}
        }
        return CCountry();
    }

    QStringList CCountryList::toIsoNameList(bool sorted) const
    {
        QStringList sl;
        for (const CCountry &country : (*this))
        {
            const QString s = country.getCombinedStringIsoName();
            if (s.isEmpty()) { continue; }
            sl.append(s);
        }
        if (sorted) { sl.sort(); }
        return sl;
    }

    QStringList CCountryList::toNameIsoList(bool sorted) const
    {
        QStringList sl;
        for (const CCountry &country : (*this))
        {
            const QString s = country.getCombinedStringNameIso();
            if (s.isEmpty()) { continue; }
            sl.append(s);
        }
        if (sorted) { sl.sort(); }
        return sl;
    }

    QStringList CCountryList::toNameList(bool sorted) const
    {
        QStringList sl;
        for (const CCountry &country : (*this))
        {
            const QString s = country.getName();
            if (s.isEmpty()) { continue; }
            sl.append(s);
        }
        if (sorted) { sl.sort(); }
        return sl;
    }

    QStringList CCountryList::toIsoList(bool sorted) const
    {
        QStringList sl;
        for (const CCountry &country : (*this))
        {
            const QString s = country.getIsoCode();
            if (s.length() != 2) { continue; }
            sl.append(s);
        }
        if (sorted) { sl.sort(); }
        return sl;
    }

    QStringList CCountryList::toIso3List(bool sorted) const
    {
        QStringList sl;
        for (const CCountry &country : (*this))
        {
            const QString s = country.getIso3Code();
            if (s.length() != 3) { continue; }
            sl.append(s);
        }
        if (sorted) { sl.sort(); }
        return sl;
    }

    CCountryList CCountryList::fromDatabaseJson(const QJsonArray &array)
    {
        CCountryList countries;
        for (const QJsonValue &value : array)
        {
            CCountry country(CCountry::fromDatabaseJson(value.toObject()));
            countries.push_back(country);
        }
        return countries;
    }
} // namespace

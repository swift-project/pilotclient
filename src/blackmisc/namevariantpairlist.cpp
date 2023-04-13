/* Copyright (C) 2013
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution. No part of swift project, including this file, may be copied, modified, propagated,
 * or distributed except according to the terms contained in the LICENSE file.
 */

#include "blackmisc/namevariantpairlist.h"
#include "blackmisc/range.h"

BLACK_DEFINE_SEQUENCE_MIXINS(BlackMisc, CNameVariantPair, CNameVariantPairList)

namespace BlackMisc
{
    CNameVariantPairList::CNameVariantPairList() {}

    CNameVariantPairList::CNameVariantPairList(const CSequence<CNameVariantPair> &other) : CSequence<CNameVariantPair>(other)
    {}

    bool CNameVariantPairList::containsName(const QString &name) const
    {
        return this->contains(&CNameVariantPair::getName, name);
    }

    QStringList CNameVariantPairList::getNames(bool sorted) const
    {
        if (this->isEmpty()) { return QStringList(); }
        QStringList codes = this->transform(Predicates::MemberTransform(&CNameVariantPair::getName));
        if (sorted) { codes.sort(); }
        return codes;
    }

    CNameVariantPair CNameVariantPairList::getValue(const QString &name) const
    {
        if (name.isEmpty()) { return CNameVariantPair(); }
        return this->findBy(&CNameVariantPair::getName, name).frontOrDefault();
    }

    CVariant CNameVariantPairList::getVariantValue(const QString &name) const
    {
        if (name.isEmpty()) { return CVariant(); }
        return getValue(name).getVariant();
    }

    QString CNameVariantPairList::getValueAsString(const QString &name) const
    {
        if (name.isEmpty()) { return QString(); }
        const CVariant cs(getValue(name).getVariant());
        if (cs.isNull() || !cs.canConvert<QString>()) { return QString(); }
        return cs.value<QString>();
    }

    bool CNameVariantPairList::addOrReplaceValue(const QString &name, const CVariant &value, const CIcon &icon)
    {
        if (name.isEmpty()) { return false; }
        int i = getIndexForName(name);
        if (i < 0)
        {
            this->push_back(CNameVariantPair(name, value, icon));
            return false;
        }
        else
        {
            (*this)[i] = CNameVariantPair(name, value, icon);
            return true;
        }
    }

    int CNameVariantPairList::getIndexForName(const QString &name) const
    {
        for (int i = 0; i < this->size(); i++)
        {
            if ((*this)[i].getName() == name)
            {
                return i;
            }
        }
        return -1;
    }
} // namespace

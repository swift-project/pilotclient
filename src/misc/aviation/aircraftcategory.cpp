// SPDX-FileCopyrightText: Copyright (C) 2019 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

#include "misc/aviation/aircraftcategory.h"

#include <QRegularExpression>
#include <QStringBuilder>
#include <Qt>
#include <QtGlobal>

#include "misc/comparefunctions.h"
#include "misc/db/datastoreutility.h"
#include "misc/logcategories.h"
#include "misc/propertyindexref.h"
#include "misc/statusmessage.h"
#include "misc/stringutils.h"

using namespace swift::misc;
using namespace swift::misc::db;

SWIFT_DEFINE_VALUEOBJECT_MIXINS(swift::misc::aviation, CAircraftCategory)

namespace swift::misc::aviation
{
    CAircraftCategory::CAircraftCategory(const QString &name, const QString &description, const QString &path,
                                         bool assignable)
        : m_name(name), m_description(description), m_path(path), m_assignable(assignable)
    {}

    QString CAircraftCategory::getNameDbKey() const
    {
        return (this->isLoadedFromDb()) ? this->getName() % u' ' % this->getDbKeyAsStringInParentheses() :
                                          this->getName();
    }

    QString CAircraftCategory::convertToQString(bool i18n) const
    {
        Q_UNUSED(i18n);
        return QStringLiteral("%1 %2").arg(this->getNameDbKey(), this->getDescription());
    }

    CStatusMessageList CAircraftCategory::validate() const { return {}; }

    bool CAircraftCategory::isNull() const { return m_name.isEmpty() && m_description.isEmpty(); }

    const CAircraftCategory &CAircraftCategory::null()
    {
        static const CAircraftCategory null;
        return null;
    }

    bool CAircraftCategory::hasName() const { return !m_name.isEmpty(); }

    bool CAircraftCategory::matchesName(const QString &name, Qt::CaseSensitivity cs) const
    {
        return stringCompare(name, this->getName(), cs);
    }

    void CAircraftCategory::setLevel(int l1, int l2, int l3)
    {
        m_l1 = l1;
        m_l2 = l2;
        m_l3 = l3;
    }

    bool CAircraftCategory::isLevel(int l1, int l2, int l3) const { return l1 == m_l1 && l2 == m_l2 && l3 == m_l3; }

    bool CAircraftCategory::isLevel(const QList<int> &level) const
    {
        if (level.size() != 3) { return false; }
        return m_l1 == level[0] && m_l2 == level[1] && m_l3 == level[2];
    }

    bool CAircraftCategory::isLevel(const CAircraftCategory &category) const
    {
        if (category.isNull()) { return false; }
        return category.m_l1 == m_l1 && category.m_l2 == m_l2 && category.m_l3 == m_l3;
    }

    QList<int> CAircraftCategory::getLevel() const
    {
        QList<int> l;
        if (m_l1 > 0) l << m_l1;
        if (m_l2 > 0) l << m_l2;
        if (m_l3 > 0) l << m_l3;
        return l;
    }

    bool CAircraftCategory::isFirstLevel() const { return (m_l3 == 0 && m_l2 == 0 && m_l1 > 0); }

    int CAircraftCategory::getDepth() const
    {
        if (this->isFirstLevel()) { return 1; }
        if (m_l3 == 0 && m_l2 > 0 && m_l1 > 0) { return 2; }
        return 3;
    }

    QString CAircraftCategory::getLevelString() const
    {
        if (this->isNull()) { return {}; }
        return QStringLiteral("%1.%2.%3").arg(m_l1).arg(m_l2).arg(m_l3);
    }

    QString CAircraftCategory::getLevelAndName() const
    {
        if (this->isNull()) { return {}; }
        return QStringLiteral("%1 %2").arg(this->getLevelString(), this->getName());
    }

    QString CAircraftCategory::getLevelAndPath() const
    {
        if (this->isNull()) { return {}; }
        return QStringLiteral("%1 %2").arg(this->getLevelString(), this->getPath());
    }

    bool CAircraftCategory::matchesPath(const QString &path, Qt::CaseSensitivity cs)
    {
        return stringCompare(path, this->getPath(), cs);
    }

    bool CAircraftCategory::matchesLevel(int l1, int l2, int l3) const
    {
        if (l1 != m_l1) { return false; }
        if (l2 > 0 && l2 != m_l2) { return false; }
        if (l3 > 0 && l3 != m_l3) { return false; }
        return true;
    }

    bool CAircraftCategory::matchesLevel(const QList<int> &level) const
    {
        if (level.isEmpty()) { return false; }
        const int l1 = level.first();
        const int l2 = level.size() > 1 ? level.at(1) : 0;
        const int l3 = level.size() > 2 ? level.at(2) : 0;
        return this->matchesLevel(l1, l2, l3);
    }

    QVariant CAircraftCategory::propertyByIndex(swift::misc::CPropertyIndexRef index) const
    {
        if (index.isMyself()) { return QVariant::fromValue(*this); }
        if (IDatastoreObjectWithIntegerKey::canHandleIndex(index))
        {
            return IDatastoreObjectWithIntegerKey::propertyByIndex(index);
        }
        const auto i = index.frontCasted<ColumnIndex>();
        switch (i)
        {
        case IndexName: return QVariant::fromValue(m_name);
        case IndexDescription: return QVariant::fromValue(m_description);
        case IndexAssignable: return QVariant::fromValue(m_assignable);
        case IndexPath: return QVariant::fromValue(m_path);
        case IndexLevelString: return QVariant::fromValue(this->getLevelString());
        case IndexLevelStringAndName: return QVariant::fromValue(this->getLevelAndName());
        case IndexLevelStringAndPath: return QVariant::fromValue(this->getLevelAndPath());
        default: return CValueObject::propertyByIndex(index);
        }
    }

    void CAircraftCategory::setPropertyByIndex(CPropertyIndexRef index, const QVariant &variant)
    {
        if (index.isMyself())
        {
            (*this) = variant.value<CAircraftCategory>();
            return;
        }
        if (IDatastoreObjectWithIntegerKey::canHandleIndex(index))
        {
            IDatastoreObjectWithIntegerKey::setPropertyByIndex(index, variant);
            return;
        }
        const auto i = index.frontCasted<ColumnIndex>();
        switch (i)
        {
        case IndexName: this->setName(variant.value<QString>()); break;
        case IndexDescription: this->setDescription(variant.value<QString>()); break;
        case IndexAssignable: this->setAssignable(variant.value<bool>()); break;
        default: CValueObject::setPropertyByIndex(index, variant); break;
        }
    }

    int CAircraftCategory::comparePropertyByIndex(CPropertyIndexRef index, const CAircraftCategory &compareValue) const
    {
        if (index.isMyself()) { return m_path.compare(compareValue.getPath(), Qt::CaseInsensitive); }
        if (IDatastoreObjectWithIntegerKey::canHandleIndex(index))
        {
            return IDatastoreObjectWithIntegerKey::comparePropertyByIndex(index, compareValue);
        }
        const auto i = index.frontCasted<ColumnIndex>();
        switch (i)
        {
        case IndexName: return m_name.compare(compareValue.getName(), Qt::CaseInsensitive);
        case IndexPath: return m_path.compare(compareValue.getPath(), Qt::CaseInsensitive);
        case IndexDescription: return m_description.compare(compareValue.getDescription(), Qt::CaseInsensitive);
        case IndexAssignable: return Compare::compare(this->isAssignable(), compareValue.isAssignable());
        case IndexLevelStringAndName:
        case IndexLevelStringAndPath:
        case IndexLevelString: return this->compareByLevel(compareValue);
        default: return CValueObject::comparePropertyByIndex(index, *this);
        }
        Q_ASSERT_X(false, Q_FUNC_INFO, "No comparison");
        return 0;
    }

    CAircraftCategory CAircraftCategory::fromDatabaseJson(const QJsonObject &json, const QString &prefix)
    {
        const QString name(json.value(prefix % u"name").toString());
        const QString description(json.value(prefix % u"description").toString());
        const QString path(json.value(prefix % u"path").toString());
        const bool assignable = CDatastoreUtility::dbBoolStringToBool(json.value(prefix % u"assignable").toString());
        const int l1 = json.value(prefix % u"l1").toInt();
        const int l2 = json.value(prefix % u"l2").toInt();
        const int l3 = json.value(prefix % u"l3").toInt();

        CAircraftCategory cat(name, description, path, assignable);
        cat.setLevel(l1, l2, l3);
        cat.setKeyVersionTimestampFromDatabaseJson(json, prefix);
        return cat;
    }

    int CAircraftCategory::compareByLevel(const CAircraftCategory &other) const
    {
        // any faster or better way to compare can be used here
        int c = Compare::compare(m_l1, other.m_l1);
        if (c != 0) { return c; }
        c = Compare::compare(m_l2, other.m_l2);
        if (c != 0) { return c; }
        c = Compare::compare(m_l3, other.m_l3);
        return c;
    }

    bool CAircraftCategory::isHigherLevel(const CAircraftCategory &other) const
    {
        const int c = this->compareByLevel(other);
        return c < 0;
    }
} // namespace swift::misc::aviation

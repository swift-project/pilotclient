// SPDX-FileCopyrightText: Copyright (C) 2015 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

#include "misc/simulation/distributor.h"

#include <QJsonValue>
#include <QStringBuilder>
#include <Qt>
#include <QtGlobal>

#include "misc/logcategories.h"
#include "misc/statusmessage.h"
#include "misc/stringutils.h"

SWIFT_DEFINE_VALUEOBJECT_MIXINS(swift::misc::simulation, CDistributor)

namespace swift::misc::simulation
{
    CDistributor::CDistributor(const QString &key) { this->setDbKey(unifyKeyOrAlias(key)); }

    CDistributor::CDistributor(const QString &id, const QString &description, const QString &alias1,
                               const QString &alias2, const CSimulatorInfo &simulator)
        : m_description(description), m_alias1(alias1), m_alias2(alias2), m_simulator(simulator)
    {
        this->setDbKey(unifyKeyOrAlias(id));
    }

    QString CDistributor::getIdAndDescription() const
    {
        if (!this->getDbKey().isEmpty() && !this->getDescription().isEmpty())
        {
            return this->getDbKey() % u' ' % this->getDescription();
        }
        if (!this->getDbKey().isEmpty()) { return this->getDbKey(); }
        return {};
    }

    bool CDistributor::matchesKeyOrAlias(const QString &keyOrAlias) const
    {
        QString s(keyOrAlias.trimmed().toUpper());
        if (s.isEmpty()) { return false; }
        return (getDbKey() == s || getAlias1() == s || getAlias2() == s);
    }

    bool CDistributor::matchesKeyOrAlias(const CDistributor &distributor) const
    {
        if (distributor.hasValidDbKey() && this->matchesKeyOrAlias(distributor.getDbKey())) { return true; }
        if (distributor.hasAlias1() && this->matchesKeyOrAlias(distributor.getAlias1())) { return true; }
        return (distributor.hasAlias2() && this->matchesKeyOrAlias(distributor.getAlias2()));
    }

    bool CDistributor::matchesSimulator(const CSimulatorInfo &simulator) const
    {
        return m_simulator.matchesAny(simulator);
    }

    QVariant CDistributor::propertyByIndex(CPropertyIndexRef index) const
    {
        if (index.isMyself()) { return QVariant::fromValue(*this); }
        if (IDatastoreObjectWithStringKey::canHandleIndex(index))
        {
            return IDatastoreObjectWithStringKey::propertyByIndex(index);
        }
        if (IOrderable::canHandleIndex(index)) { return IOrderable::propertyByIndex(index); }

        const auto i = index.frontCasted<ColumnIndex>();
        switch (i)
        {
        case IndexAlias1: return QVariant::fromValue(m_alias1);
        case IndexAlias2: return QVariant::fromValue(m_alias2);
        case IndexDescription: return QVariant::fromValue(m_description);
        case IndexSimulator: return m_simulator.propertyByIndex(index.copyFrontRemoved());
        default: return CValueObject::propertyByIndex(index);
        }
    }

    void CDistributor::setPropertyByIndex(CPropertyIndexRef index, const QVariant &variant)
    {
        if (index.isMyself())
        {
            (*this) = variant.value<CDistributor>();
            return;
        }
        if (IDatastoreObjectWithStringKey::canHandleIndex(index))
        {
            IDatastoreObjectWithStringKey::setPropertyByIndex(index, variant);
            return;
        }
        if (IOrderable::canHandleIndex(index))
        {
            IOrderable::setPropertyByIndex(index, variant);
            return;
        }

        const auto i = index.frontCasted<ColumnIndex>();
        switch (i)
        {
        case IndexAlias1: m_alias1 = variant.value<QString>(); break;
        case IndexAlias2: m_alias2 = variant.value<QString>(); break;
        case IndexDescription: m_description = variant.value<QString>(); break;
        case IndexSimulator: m_simulator.setPropertyByIndex(index.copyFrontRemoved(), variant); break;
        default: CValueObject::setPropertyByIndex(index, variant); break;
        }
    }

    int CDistributor::comparePropertyByIndex(CPropertyIndexRef index, const CDistributor &compareValue) const
    {
        if (IDatastoreObjectWithStringKey::canHandleIndex(index))
        {
            return IDatastoreObjectWithStringKey::comparePropertyByIndex(index, compareValue);
        }
        if (IOrderable::canHandleIndex(index)) { return IOrderable::comparePropertyByIndex(index, compareValue); }

        const auto i = index.frontCasted<ColumnIndex>();
        switch (i)
        {
        case IndexAlias1: return m_alias1.compare(compareValue.m_alias1, Qt::CaseInsensitive);
        case IndexAlias2: return m_alias2.compare(compareValue.m_alias2, Qt::CaseInsensitive);
        case IndexDescription: return m_description.compare(compareValue.getDescription(), Qt::CaseInsensitive);
        case IndexSimulator:
            return m_simulator.comparePropertyByIndex(index.copyFrontRemoved(), compareValue.m_simulator);
        default: break;
        }
        Q_ASSERT_X(false, Q_FUNC_INFO, "Compare failed");
        return 0;
    }

    QString CDistributor::convertToQString(bool i18n) const
    {
        Q_UNUSED(i18n)
        QString s = QStringLiteral("Id: '%1'").arg(m_dbKey);
        if (this->hasAlias1()) { s.append(" ").append(m_alias1); }
        if (this->hasAlias2()) { s.append(" ").append(m_alias2); }
        return s;
    }

    bool CDistributor::hasCompleteData() const { return !m_description.isEmpty() && !m_dbKey.isEmpty(); }

    CStatusMessageList CDistributor::validate() const
    {
        static const CLogCategoryList cats(CLogCategoryList(this).withValidation());
        CStatusMessageList msgs;
        if (!hasValidDbKey())
        {
            msgs.push_back(CStatusMessage(cats, CStatusMessage::SeverityError, u"Distributor: missing id"));
        }
        if (!hasDescription())
        {
            msgs.push_back(CStatusMessage(cats, CStatusMessage::SeverityError, u"Distributor: missing description"));
        }
        return msgs;
    }

    void CDistributor::updateMissingParts(const CDistributor &otherDistributor)
    {
        if (!this->hasValidDbKey() && otherDistributor.hasValidDbKey())
        {
            // we have no DB data, but the other one has
            // so we change roles. We take the DB object as base, and update our parts
            CDistributor copy(otherDistributor);
            copy.updateMissingParts(*this);
            *this = copy;
            return;
        }

        if (!this->hasAlias1()) { this->setAlias1(otherDistributor.getAlias1()); }
        if (!this->hasAlias2()) { this->setAlias1(otherDistributor.getAlias2()); }
        if (!this->hasDescription()) { this->setDescription(otherDistributor.getDescription()); }
    }

    CDistributor CDistributor::fromDatabaseJson(const QJsonObject &json, const QString &prefix)
    {
        if (!existsKey(json, prefix))
        {
            // when using relationship, this can be null
            return {};
        }

        const QString description(json.value(prefix % u"description").toString());
        if (description.isEmpty())
        {
            // stub, only key, maybe also timestamps
            CDistributor distributorStub;
            distributorStub.setKeyVersionTimestampFromDatabaseJson(json, prefix);
            return distributorStub;
        }

        const CSimulatorInfo simulator = CSimulatorInfo::fromDatabaseJson(json, prefix);
        const QString alias1(json.value(prefix % u"alias1").toString());
        const QString alias2(json.value(prefix % u"alias2").toString());
        Q_ASSERT_X(!description.isEmpty(), Q_FUNC_INFO, "Missing description");
        CDistributor distributor("", description, alias1, alias2, simulator);
        distributor.setKeyVersionTimestampFromDatabaseJson(json, prefix);
        distributor.setLoadedFromDb(true);
        return distributor;
    }

    const QString &CDistributor::standardFSX()
    {
        static const QString k("FSX");
        return k;
    }

    const QString &CDistributor::standardP3D()
    {
        static const QString k("P3D");
        return k;
    }

    const QString &CDistributor::standardFS9()
    {
        static const QString k("FS9");
        return k;
    }

    const QString &CDistributor::standardXPlane()
    {
        static const QString k("XP");
        return k;
    }

    const QString &CDistributor::standardFlightGear()
    {
        static const QString k("FG");
        return k;
    }

    const QSet<QString> &CDistributor::standardAllFsFamily()
    {
        static const QSet<QString> fsFamily(
            { standardFS9(), standardFSX(), standardP3D(), standardMsfs(), standardMsfs2024() });
        return fsFamily;
    }

    const QSet<QString> &CDistributor::xplaneMostPopular()
    {
        static const QSet<QString> mp({ xplaneBlueBell(), xplaneXcsl() });
        return mp;
    }

    const QString &CDistributor::xplaneBlueBell()
    {
        static const QString k("BB");
        return k;
    }

    const QString &CDistributor::xplaneXcsl()
    {
        static const QString k("XCSL");
        return k;
    }

    const QString &CDistributor::standardMsfs()
    {
        static const QString k("MSFS2020");
        return k;
    }

    const QString &CDistributor::standardMsfs2024()
    {
        static const QString k("MSFS2024");
        return k;
    }

    QString CDistributor::unifyKeyOrAlias(const QString &value)
    {
        return removeChars(value.trimmed().toUpper(), [](QChar c) { return !c.isLetterOrNumber(); });
    }

} // namespace swift::misc::simulation

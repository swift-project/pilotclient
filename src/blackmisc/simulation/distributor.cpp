/* Copyright (C) 2015
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

#include "blackmisc/logcategory.h"
#include "blackmisc/logcategorylist.h"
#include "blackmisc/simulation/distributor.h"
#include "blackmisc/statusmessage.h"

#include <QJsonValue>
#include <Qt>
#include <QtGlobal>

namespace BlackMisc
{
    namespace Simulation
    {
        CDistributor::CDistributor() { }

        CDistributor::CDistributor(const QString &key)
        {
            this->setDbKey(key);
        }

        CDistributor::CDistributor(const QString &id, const QString &description, const QString &alias1, const QString &alias2, const CSimulatorInfo &simulator) :
            m_description(description), m_alias1(alias1.trimmed().toUpper()), m_alias2(alias2.trimmed().toUpper()), m_simulator(simulator)
        {
            this->setDbKey(id);
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
            return this->m_simulator.matchesAny(simulator);
        }

        CVariant CDistributor::propertyByIndex(const CPropertyIndex &index) const
        {
            if (index.isMyself()) { return CVariant::from(*this); }
            if (IDatastoreObjectWithStringKey::canHandleIndex(index)) { return IDatastoreObjectWithStringKey::propertyByIndex(index); }
            if (IOrderable::canHandleIndex(index)) { return IOrderable::propertyByIndex(index); }

            ColumnIndex i = index.frontCasted<ColumnIndex>();
            switch (i)
            {
            case IndexAlias1: return CVariant::from(this->m_alias1);
            case IndexAlias2: return CVariant::from(this->m_alias2);
            case IndexDescription: return CVariant::from(this->m_description);
            case IndexSimulator: return m_simulator.propertyByIndex(index.copyFrontRemoved());
            default:
                return CValueObject::propertyByIndex(index);
            }
        }

        void CDistributor::setPropertyByIndex(const CPropertyIndex &index, const CVariant &variant)
        {
            if (index.isMyself()) { (*this) = variant.to<CDistributor>(); return; }
            if (IDatastoreObjectWithStringKey::canHandleIndex(index)) { IDatastoreObjectWithStringKey::setPropertyByIndex(index, variant); return; }
            if (IOrderable::canHandleIndex(index)) { IOrderable::setPropertyByIndex(index, variant); return; }

            ColumnIndex i = index.frontCasted<ColumnIndex>();
            switch (i)
            {
            case IndexAlias1:
                this->m_alias1 = variant.value<QString>();
                break;
            case IndexAlias2:
                this->m_alias2 = variant.value<QString>();
                break;
            case IndexDescription:
                this->m_description = variant.value<QString>();
                break;
            case IndexSimulator:
                this->m_simulator.setPropertyByIndex(index.copyFrontRemoved(), variant);
                break;
            default:
                CValueObject::setPropertyByIndex(index, variant);
                break;
            }
        }

        int CDistributor::comparePropertyByIndex(const CPropertyIndex &index, const CDistributor &compareValue) const
        {
            if (IDatastoreObjectWithStringKey::canHandleIndex(index)) { return IDatastoreObjectWithStringKey::comparePropertyByIndex(index, compareValue); }
            if (IOrderable::canHandleIndex(index)) { return IOrderable::comparePropertyByIndex(index, compareValue); }
            ColumnIndex i = index.frontCasted<ColumnIndex>();
            switch (i)
            {
            case IndexAlias1: return this->m_alias1.compare(compareValue.m_alias1, Qt::CaseInsensitive);
            case IndexAlias2: return this->m_alias2.compare(compareValue.m_alias2, Qt::CaseInsensitive);
            case IndexDescription: return this->m_description.compare(compareValue.getDescription(), Qt::CaseInsensitive);
            case IndexSimulator: return this->m_simulator.comparePropertyByIndex(index.copyFrontRemoved(), compareValue.m_simulator);
            default:
                break;
            }
            Q_ASSERT_X(false, Q_FUNC_INFO, "Compare failed");
            return 0;
        }

        QString CDistributor::convertToQString(bool i18n) const
        {
            Q_UNUSED(i18n);
            QString s = QString("Id: %1").arg(m_dbKey);
            if (this->hasAlias1()) { s.append(" ").append(m_alias1); }
            if (this->hasAlias2()) { s.append(" ").append(m_alias2); }
            return s;
        }

        bool CDistributor::hasCompleteData() const
        {
            return !this->m_description.isEmpty() && !this->m_dbKey.isEmpty();
        }

        CStatusMessageList CDistributor::validate() const
        {
            static const CLogCategoryList cats(CLogCategoryList(this).join({ CLogCategory::validation() }));
            CStatusMessageList msgs;
            if (!hasValidDbKey()) { msgs.push_back(CStatusMessage(cats, CStatusMessage::SeverityError, "Distributor: missing id")); }
            if (!hasDescription()) { msgs.push_back(CStatusMessage(cats, CStatusMessage::SeverityError, "Distributor: missing description")); }
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
                return CDistributor();
            }

            const QString description(json.value(prefix + "description").toString());
            if (description.isEmpty())
            {
                // stub, only key, maybe also timestamps
                CDistributor distributorStub;
                distributorStub.setKeyAndTimestampFromDatabaseJson(json, prefix);
                return distributorStub;
            }

            const CSimulatorInfo simulator = CSimulatorInfo::fromDatabaseJson(json, prefix);
            const QString alias1(json.value(prefix + "alias1").toString());
            const QString alias2(json.value(prefix + "alias2").toString());
            Q_ASSERT_X(!description.isEmpty(), Q_FUNC_INFO, "Missing description");
            CDistributor distributor("", description, alias1, alias2, simulator);
            distributor.setKeyAndTimestampFromDatabaseJson(json, prefix);
            distributor.setLoadedFromDb(true);
            return distributor;
        }
    } // namespace
} // namespace

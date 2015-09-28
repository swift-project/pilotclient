/* Copyright (C) 2015
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

#include "distributor.h"
#include <QJsonArray>

namespace BlackMisc
{
    namespace Simulation
    {
        CDistributor::CDistributor() { }

        CDistributor::CDistributor(const QString &id)
        {
            this->setDbKey(id);
        }

        CDistributor::CDistributor(const QString &id, const QString &description, const QString &alias1, const QString &alias2) :
            m_description(description), m_alias1(alias1.trimmed().toUpper()), m_alias2(alias2.trimmed().toUpper())
        {
            this->setDbKey(id);
        }

        bool CDistributor::matchesIdOrAlias(const QString &idOrAlias) const
        {
            QString s(idOrAlias.trimmed().toUpper());
            if (s.isEmpty()) { return false; }
            return (getId() == s || getAlias1() == s || getAlias2() == s);
        }

        CVariant CDistributor::propertyByIndex(const CPropertyIndex &index) const
        {
            if (index.isMyself()) { return CVariant::from(*this); }
            if (IDatastoreObjectWithStringKey::canHandleIndex(index)) { return IDatastoreObjectWithStringKey::propertyByIndex(index); }
            ColumnIndex i = index.frontCasted<ColumnIndex>();
            switch (i)
            {
            case IndexAlias1: return CVariant::from(this->m_alias1);
            case IndexAlias2: return CVariant::from(this->m_alias2);
            case IndexDescription: return CVariant::from(this->m_description);
            default:
                return CValueObject::propertyByIndex(index);
            }
        }

        void CDistributor::setPropertyByIndex(const CVariant &variant, const CPropertyIndex &index)
        {
            if (index.isMyself()) { (*this) = variant.to<CDistributor>(); return; }
            if (IDatastoreObjectWithStringKey::canHandleIndex(index)) { IDatastoreObjectWithStringKey::setPropertyByIndex(variant, index); return; }
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
            default:
                CValueObject::setPropertyByIndex(variant, index);
                break;
            }
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
            if (!hasDescription()) { msgs.push_back(CStatusMessage(cats, CStatusMessage::SeverityWarning, "Distributor: missing description")); }
            return msgs;
        }

        void CDistributor::updateMissingParts(const CDistributor &otherDistributor)
        {
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

            QString description(json.value(prefix + "description").toString());
            QString alias1(json.value(prefix + "alias1").toString());
            QString alias2(json.value(prefix + "alias2").toString());
            Q_ASSERT_X(!description.isEmpty(), Q_FUNC_INFO, "Missing description");
            CDistributor distributor("", description, alias1, alias2);
            distributor.setKeyAndTimestampFromDatabaseJson(json, prefix);
            return distributor;
        }

    } // namespace
} // namespace

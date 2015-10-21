/* Copyright (C) 2015
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

#include "blackmisc/network/authenticateduser.h"
#include "blackmisc/iconlist.h"
#include "blackmisc/blackmiscfreefunctions.h"
#include "blackmisc/propertyindex.h"
#include "blackmisc/variant.h"
#include <tuple>

namespace BlackMisc
{
    namespace Network
    {
        CAuthenticatedUser::CAuthenticatedUser(int id, const QString &realname)
            : IDatastoreObjectWithIntegerKey(id), m_realname(realname.trimmed())
        { }

        CAuthenticatedUser::CAuthenticatedUser(int id, const QString &realname, const QString &email, const QString &password)
            : IDatastoreObjectWithIntegerKey(id), m_realname(realname.trimmed()), m_email(email.trimmed()), m_password(password.trimmed())
        { }

        QString CAuthenticatedUser::getRealNameAndId() const
        {
            if (hasValidRealName())
            {
                return m_realname + " " + getDbKeyAsStringInParentheses();
            }
            else
            {
                return getDbKeyAsString();
            }
        }

        QString CAuthenticatedUser::convertToQString(bool i18n) const
        {
            Q_UNUSED(i18n);
            if (this->m_realname.isEmpty()) return "<no realname>";
            QString s = this->m_realname;
            if (this->hasValidDbKey())
            {
                s.append(" ").append(this->getDbKeyAsStringInParentheses());
            }
            return s;
        }

        CAuthenticatedUser CAuthenticatedUser::fromDatabaseJson(const QJsonObject &json)
        {
            CAuthenticatedUser user;
            user.setDbKey(json.value("id").toInt(-1));
            user.setVatsimId(json.value("vatsimId").toInt(-1));
            user.setRealName(json.value("name").toString());
            user.setEmail(json.value("email").toString(""));
            user.setCountry(CCountry(json.value("country").toString(), json.value("countryname").toString()));
            user.setEnabled(json.value("enabled").toBool());
            user.setAuthenticated(json.value("authenticated").toBool());
            CRoleList roles(CRoleList::fromDatabaseJson(json.value("roles").toArray()));
            user.setRoles(roles);
            return user;
        }

        void CAuthenticatedUser::setRealName(const QString &realname)
        {
            QString rn(realname.trimmed().simplified());
            this->m_realname = rn;
        }

        CStatusMessageList CAuthenticatedUser::validate() const
        {
            static const CLogCategoryList cats(CLogCategoryList(this).join({ CLogCategory::validation()}));
            CStatusMessageList msgs;
            // callsign optional
            if (!this->hasValidDbKey()) { msgs.push_back(CStatusMessage(cats, CStatusMessage::SeverityWarning, "Invalid id"));}
            if (!this->hasValidRealName()) { msgs.push_back(CStatusMessage(cats, CStatusMessage::SeverityWarning, "Invalid real name"));}
            if (!this->hasValidCredentials()) { msgs.push_back(CStatusMessage(cats, CStatusMessage::SeverityWarning, "Invalid credentials"));}
            return msgs;
        }

        bool CAuthenticatedUser::isAdmin() const
        {
            return this->hasRole("ADMIN");
        }

        CIcon CAuthenticatedUser::toIcon() const
        {
            return CIconList::iconByIndex(CIcons::StandardIconUser16);
        }

        CVariant CAuthenticatedUser::propertyByIndex(const BlackMisc::CPropertyIndex &index) const
        {
            if (index.isMyself()) { return CVariant::from(*this); }
            if (IDatastoreObjectWithIntegerKey::canHandleIndex(index)) { return IDatastoreObjectWithIntegerKey::propertyByIndex(index); }
            ColumnIndex i = index.frontCasted<ColumnIndex>();
            switch (i)
            {
            case IndexVatsimId:
                return CVariant::fromValue(this->m_vatsimId);
            case IndexEmail:
                return CVariant::fromValue(this->m_email);
            case IndexPassword:
                return CVariant::fromValue(this->m_password);
            case IndexRealName:
                return CVariant::fromValue(this->m_realname);
            default:
                return CValueObject::propertyByIndex(index);
            }
        }

        void CAuthenticatedUser::setPropertyByIndex(const CVariant &variant, const BlackMisc::CPropertyIndex &index)
        {
            if (index.isMyself()) { (*this) = variant.to<CAuthenticatedUser>(); return; }
            if (IDatastoreObjectWithIntegerKey::canHandleIndex(index)) { IDatastoreObjectWithIntegerKey::setPropertyByIndex(variant, index); return; }
            ColumnIndex i = index.frontCasted<ColumnIndex>();
            switch (i)
            {
            case IndexVatsimId:
                this->setVatsimId(variant.toInt());
                break;
            case IndexEmail:
                this->setEmail(variant.value<QString>());
                break;
            case IndexPassword:
                this->setPassword(variant.value<QString>());
                break;
            case IndexRealName:
                this->setRealName(variant.value<QString>());
                break;
            default:
                CValueObject::setPropertyByIndex(variant, index);
                break;
            }
        }
    } // namespace
} // namespace

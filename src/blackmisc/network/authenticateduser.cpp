/* Copyright (C) 2015
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution. No part of swift project, including this file, may be copied, modified, propagated,
 * or distributed except according to the terms contained in the LICENSE file.
 */

#include "blackmisc/network/authenticateduser.h"
#include "blackmisc/icons.h"
#include "blackmisc/logcategories.h"
#include "blackmisc/propertyindex.h"
#include "blackmisc/statusmessage.h"
#include "blackmisc/variant.h"

#include <QJsonArray>
#include <QJsonValue>
#include <QStringBuilder>
#include <QtGlobal>

namespace BlackMisc
{
    namespace Network
    {
        CAuthenticatedUser::CAuthenticatedUser()
        { }

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
                return m_realname % u' ' % getDbKeyAsStringInParentheses();
            }
            else
            {
                return getDbKeyAsString();
            }
        }

        QString CAuthenticatedUser::convertToQString(bool i18n) const
        {
            Q_UNUSED(i18n);
            if (m_realname.isEmpty()) { return QStringLiteral("<no realname>"); }
            return m_realname % (this->hasValidDbKey() ? this->getDbKeyAsStringInParentheses(" ") : QString());
        }

        CAuthenticatedUser CAuthenticatedUser::fromDatabaseJson(const QJsonObject &json)
        {
            CAuthenticatedUser user;
            user.setDbKey(json.value("id").toInt(-1));
            user.setVatsimId(json.value("vatsimId").toInt(-1));
            user.setRealName(json.value("name").toString());
            user.setUsername(json.value("username").toString());
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
            const QString rn(realname.trimmed().simplified());
            m_realname = rn;
        }

        void CAuthenticatedUser::setUsername(const QString &username)
        {
            const QString un(username.trimmed().simplified().toUpper());
            m_username = un;
        }

        CStatusMessageList CAuthenticatedUser::validate() const
        {
            static const CLogCategoryList cats(CLogCategoryList(this).withValidation());
            CStatusMessageList msgs;
            // callsign optional
            if (!this->hasValidDbKey()) { msgs.push_back(CStatusMessage(cats, CStatusMessage::SeverityWarning, u"Invalid id"));}
            if (!this->hasValidRealName()) { msgs.push_back(CStatusMessage(cats, CStatusMessage::SeverityWarning, u"Invalid real name"));}
            if (!this->hasValidCredentials()) { msgs.push_back(CStatusMessage(cats, CStatusMessage::SeverityWarning, u"Invalid credentials"));}
            return msgs;
        }

        bool CAuthenticatedUser::hasAdminRole() const
        {
            return this->hasRole("ADMIN");
        }

        bool CAuthenticatedUser::hasMappingAdminRole() const
        {
            return this->hasRole("MAPPINGADMIN");
        }

        bool CAuthenticatedUser::hasBulkRole() const
        {
            return this->hasRole("BULK");
        }

        bool CAuthenticatedUser::hasBulkAddRole() const
        {
            return this->hasRole("BULKADD");
        }

        bool CAuthenticatedUser::isAuthenticated() const
        {
            return this->isEnabled() && this->isValid() && m_authenticated;
        }

        bool CAuthenticatedUser::canDirectlyWriteModels() const
        {
            return this->hasBulkRole() || this->hasBulkAddRole();
        }

        CIcons::IconIndex CAuthenticatedUser::toIcon() const
        {
            return CIcons::StandardIconUser16;
        }

        CVariant CAuthenticatedUser::propertyByIndex(const BlackMisc::CPropertyIndex &index) const
        {
            if (index.isMyself()) { return CVariant::from(*this); }
            if (IDatastoreObjectWithIntegerKey::canHandleIndex(index)) { return IDatastoreObjectWithIntegerKey::propertyByIndex(index); }
            const ColumnIndex i = index.frontCasted<ColumnIndex>();
            switch (i)
            {
            case IndexVatsimId: return CVariant::fromValue(m_vatsimId);
            case IndexEmail: return CVariant::fromValue(m_email);
            case IndexPassword: return CVariant::fromValue(m_password);
            case IndexRealName: return CVariant::fromValue(m_realname);
            case IndexUsername: return CVariant::fromValue(m_username);
            default: return CValueObject::propertyByIndex(index);
            }
        }

        void CAuthenticatedUser::setPropertyByIndex(const CPropertyIndex &index, const CVariant &variant)
        {
            if (index.isMyself()) { (*this) = variant.to<CAuthenticatedUser>(); return; }
            if (IDatastoreObjectWithIntegerKey::canHandleIndex(index)) { IDatastoreObjectWithIntegerKey::setPropertyByIndex(index, variant); return; }
            const ColumnIndex i = index.frontCasted<ColumnIndex>();
            switch (i)
            {
            case IndexVatsimId: this->setVatsimId(variant.toInt()); break;
            case IndexEmail: this->setEmail(variant.value<QString>()); break;
            case IndexPassword: this->setPassword(variant.value<QString>()); break;
            case IndexRealName: this->setRealName(variant.value<QString>()); break;
            case IndexUsername: this->setUsername(variant.value<QString>()); break;
            default: CValueObject::setPropertyByIndex(index, variant); break;
            }
        }
    } // namespace
} // namespace

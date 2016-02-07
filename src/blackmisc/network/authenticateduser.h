/* Copyright (C) 2015
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

//! \file

#ifndef BLACKMISC_NETWORK_AUTHENTICATEDUSER_H
#define BLACKMISC_NETWORK_AUTHENTICATEDUSER_H

#include "blackmisc/blackmiscexport.h"
#include "blackmisc/country.h"
#include "blackmisc/datastore.h"
#include "blackmisc/propertyindex.h"
#include "blackmisc/network/rolelist.h"
#include "blackmisc/statusmessagelist.h"

namespace BlackMisc
{
    namespace Network
    {
        /*!
        * Value object encapsulating information of an authentiated user.
        */
        class BLACKMISC_EXPORT CAuthenticatedUser :
            public CValueObject<CAuthenticatedUser>,
            public BlackMisc::IDatastoreObjectWithIntegerKey
        {
        public:
            //! Properties by index
            enum ColumnIndex
            {
                IndexEmail = BlackMisc::CPropertyIndex::GlobalIndexCAuthenticatedUser,
                IndexVatsimId,
                IndexPassword,
                IndexRealName
            };

            //! Default constructor.
            CAuthenticatedUser();

            //! Constructor.
            CAuthenticatedUser(int id, const QString &realname);

            //! Constructor.
            CAuthenticatedUser(int id, const QString &realname, const QString &email = "", const QString &password = "");

            //! Get full name.
            const QString &getRealName() const { return m_realname; }

            //! Full name + id
            QString getRealNameAndId() const;

            //! setRealName
            void setRealName(const QString &realname);

            //! Get password
            const QString &getPassword() const { return m_password; }

            //! Set password
            void setPassword(const QString &pw) { m_password = pw.trimmed(); }

            //! Valid user object?
            bool isValid() const { return !this->m_realname.isEmpty() && this->hasValidDbKey(); }

            //! Valid credentials?
            bool hasValidCredentials() const { return this->isValid() && !this->m_password.isEmpty(); }

            //! Valid real name?
            bool hasValidRealName() const { return !this->m_realname.isEmpty(); }

            //! Validate, provide details about issues
            BlackMisc::CStatusMessageList validate() const;

            //! Get email.
            const QString &getEmail() const { return m_email; }

            //! Set email.
            void setEmail(const QString &email) { m_email = email.trimmed(); }

            //! Valid email?
            bool hasValidEmail() const { return !this->m_email.isEmpty(); }

            //! Get id.
            int getVatsimId() const { return m_vatsimId; }

            //! Set id
            void setVatsimId(int id) { m_vatsimId = id; }

            //! Roles
            const CRoleList &getRoles() const { return m_roles; }

            //! Roles
            QString getRolesAsString() const { return m_roles.namesAsString(); }

            //! Roles
            void setRoles(const CRoleList &roles) { m_roles = roles; }

            //! Has role?
            bool hasRole(const QString &roleName) const { return m_roles.hasRole(roleName); }

            //! Has any role?
            bool hasAnyRole(const QStringList &roles) const { return m_roles.hasAnyRole(roles); }

            //! Country
            const BlackMisc::CCountry &getCountry() const { return m_country; }

            //! Country
            void setCountry(const BlackMisc::CCountry &country) { m_country = country; }

            //! Admin?
            bool isAdmin() const;

            //! Admin?
            bool isMappingAdmin() const;

            //! Authenticated
            void setAuthenticated(bool authenticated) { m_authenticated = authenticated; }

            //! Authenticated
            bool isAuthenticated() const { return m_authenticated; }

            //! Enabled
            void setEnabled(bool enabled) { m_enabled = enabled; }

            //! Enabled
            bool isEnabled() const { return this->m_enabled; }

            //! \copydoc BlackMisc::Mixin::Icon::toIcon()
            BlackMisc::CIcon toIcon() const;

            //! \copydoc BlackMisc::Mixin::Index::propertyByIndex
            CVariant propertyByIndex(const BlackMisc::CPropertyIndex &index) const;

            //! \copydoc BlackMisc::Mixin::Index::setPropertyByIndex
            void setPropertyByIndex(const CVariant &variant, const BlackMisc::CPropertyIndex &index);

            //! \copydoc BlackMisc::Mixin::String::toQString
            QString convertToQString(bool i18n = false) const;

            //! From our database JSON format
            static CAuthenticatedUser fromDatabaseJson(const QJsonObject &json);

        private:
            BLACK_ENABLE_TUPLE_CONVERSION(CAuthenticatedUser)

            int                 m_vatsimId;
            QString             m_realname;
            QString             m_email;
            QString             m_password;
            BlackMisc::CCountry m_country;
            bool                m_enabled = false;
            bool                m_authenticated = false;
            CRoleList           m_roles;
        };
    } // namespace
} // namespace

BLACK_DECLARE_TUPLE_CONVERSION(BlackMisc::Network::CAuthenticatedUser, (
                                   o.m_dbKey,
                                   o.m_vatsimId,
                                   o.m_realname,
                                   o.m_email,
                                   o.m_password,
                                   o.m_country,
                                   o.m_enabled,
                                   o.m_authenticated,
                                   o.m_roles))
Q_DECLARE_METATYPE(BlackMisc::Network::CAuthenticatedUser)

#endif // guard

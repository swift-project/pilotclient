/* Copyright (C) 2015
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution. No part of swift project, including this file, may be copied, modified, propagated,
 * or distributed except according to the terms contained in the LICENSE file.
 */

//! \file

#ifndef BLACKMISC_NETWORK_AUTHENTICATEDUSER_H
#define BLACKMISC_NETWORK_AUTHENTICATEDUSER_H

#include "blackmisc/blackmiscexport.h"
#include "blackmisc/country.h"
#include "blackmisc/db/datastore.h"
#include "blackmisc/metaclass.h"
#include "blackmisc/network/rolelist.h"
#include "blackmisc/propertyindexref.h"
#include "blackmisc/statusmessagelist.h"
#include "blackmisc/valueobject.h"

#include <QJsonObject>
#include <QMetaType>
#include <QString>
#include <QStringList>

BLACK_DECLARE_VALUEOBJECT_MIXINS(BlackMisc::Network, CAuthenticatedUser)

namespace BlackMisc::Network
{
    /*!
     * Value object encapsulating information of an authentiated user.
     */
    class BLACKMISC_EXPORT CAuthenticatedUser :
        public CValueObject<CAuthenticatedUser>,
        public BlackMisc::Db::IDatastoreObjectWithIntegerKey
    {
    public:
        //! Properties by index
        enum ColumnIndex
        {
            IndexEmail = BlackMisc::CPropertyIndexRef::GlobalIndexCAuthenticatedUser,
            IndexVatsimId,
            IndexPassword,
            IndexRealName,
            IndexUsername
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

        //! Set real name
        void setRealName(const QString &realname);

        //! Username
        const QString &getUsername() const { return m_username; }

        //! Set username
        void setUsername(const QString &username);

        //! Get password
        const QString &getPassword() const { return m_password; }

        //! Set password
        void setPassword(const QString &pw) { m_password = pw.trimmed(); }

        //! Valid user object?
        bool isValid() const { return !m_realname.isEmpty() && this->hasValidDbKey(); }

        //! Valid credentials?
        bool hasValidCredentials() const { return this->isValid() && !m_password.isEmpty(); }

        //! Valid real name?
        bool hasValidRealName() const { return !m_realname.isEmpty(); }

        //! Validate, provide details about issues
        BlackMisc::CStatusMessageList validate() const;

        //! Get email.
        const QString &getEmail() const { return m_email; }

        //! Set email.
        void setEmail(const QString &email) { m_email = email.trimmed(); }

        //! Valid email?
        bool hasValidEmail() const { return !m_email.isEmpty(); }

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
        bool hasAdminRole() const;

        //! Admin?
        bool hasMappingAdminRole() const;

        //! Has bulk role?
        bool hasBulkRole() const;

        //! Has bulk add role?
        bool hasBulkAddRole() const;

        //! Authenticated
        void setAuthenticated(bool authenticated) { m_authenticated = authenticated; }

        //! Authenticated
        bool isAuthenticated() const;

        //! Enabled
        void setEnabled(bool enabled) { m_enabled = enabled; }

        //! Enabled
        bool isEnabled() const { return m_enabled; }

        //! Entitled to directly update models
        bool canDirectlyWriteModels() const;

        //! \copydoc BlackMisc::Mixin::Icon::toIcon()
        BlackMisc::CIcons::IconIndex toIcon() const;

        //! \copydoc BlackMisc::Mixin::Index::propertyByIndex
        QVariant propertyByIndex(BlackMisc::CPropertyIndexRef index) const;

        //! \copydoc BlackMisc::Mixin::Index::setPropertyByIndex
        void setPropertyByIndex(BlackMisc::CPropertyIndexRef index, const QVariant &variant);

        //! \copydoc BlackMisc::Mixin::String::toQString
        QString convertToQString(bool i18n = false) const;

        //! From our database JSON format
        static CAuthenticatedUser fromDatabaseJson(const QJsonObject &json);

    private:
        int                 m_vatsimId = -1;
        QString             m_realname;
        QString             m_username;
        QString             m_email;
        QString             m_password;
        BlackMisc::CCountry m_country;
        bool                m_enabled = false;
        bool                m_authenticated = false;
        CRoleList           m_roles;

        BLACK_METACLASS(
            CAuthenticatedUser,
            BLACK_METAMEMBER(dbKey),
            BLACK_METAMEMBER(vatsimId),
            BLACK_METAMEMBER(realname),
            BLACK_METAMEMBER(username),
            BLACK_METAMEMBER(email),
            BLACK_METAMEMBER(password),
            BLACK_METAMEMBER(country),
            BLACK_METAMEMBER(enabled),
            BLACK_METAMEMBER(authenticated),
            BLACK_METAMEMBER(roles)
        );
    };
} // namespace

Q_DECLARE_METATYPE(BlackMisc::Network::CAuthenticatedUser)

#endif // guard

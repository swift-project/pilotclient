// SPDX-FileCopyrightText: Copyright (C) 2015 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

//! \file

#ifndef SWIFT_MISC_NETWORK_AUTHENTICATEDUSER_H
#define SWIFT_MISC_NETWORK_AUTHENTICATEDUSER_H

#include <QJsonObject>
#include <QMetaType>
#include <QString>
#include <QStringList>

#include "misc/country.h"
#include "misc/db/datastore.h"
#include "misc/metaclass.h"
#include "misc/network/rolelist.h"
#include "misc/propertyindexref.h"
#include "misc/statusmessagelist.h"
#include "misc/swiftmiscexport.h"
#include "misc/valueobject.h"

SWIFT_DECLARE_VALUEOBJECT_MIXINS(swift::misc::network, CAuthenticatedUser)

namespace swift::misc::network
{
    /*!
     * Value object encapsulating information of an authentiated user.
     */
    class SWIFT_MISC_EXPORT CAuthenticatedUser :
        public CValueObject<CAuthenticatedUser>,
        public swift::misc::db::IDatastoreObjectWithIntegerKey
    {
    public:
        //! Properties by index
        enum ColumnIndex
        {
            IndexEmail = swift::misc::CPropertyIndexRef::GlobalIndexCAuthenticatedUser,
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
        swift::misc::CStatusMessageList validate() const;

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
        const swift::misc::CCountry &getCountry() const { return m_country; }

        //! Country
        void setCountry(const swift::misc::CCountry &country) { m_country = country; }

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

        //! \copydoc swift::misc::mixin::Icon::toIcon()
        swift::misc::CIcons::IconIndex toIcon() const;

        //! \copydoc swift::misc::mixin::Index::propertyByIndex
        QVariant propertyByIndex(swift::misc::CPropertyIndexRef index) const;

        //! \copydoc swift::misc::mixin::Index::setPropertyByIndex
        void setPropertyByIndex(swift::misc::CPropertyIndexRef index, const QVariant &variant);

        //! \copydoc swift::misc::mixin::String::toQString
        QString convertToQString(bool i18n = false) const;

        //! From our database JSON format
        static CAuthenticatedUser fromDatabaseJson(const QJsonObject &json);

    private:
        int m_vatsimId = -1;
        QString m_realname;
        QString m_username;
        QString m_email;
        QString m_password;
        swift::misc::CCountry m_country;
        bool m_enabled = false;
        bool m_authenticated = false;
        CRoleList m_roles;

        SWIFT_METACLASS(
            CAuthenticatedUser,
            SWIFT_METAMEMBER(dbKey),
            SWIFT_METAMEMBER(vatsimId),
            SWIFT_METAMEMBER(realname),
            SWIFT_METAMEMBER(username),
            SWIFT_METAMEMBER(email),
            SWIFT_METAMEMBER(password),
            SWIFT_METAMEMBER(country),
            SWIFT_METAMEMBER(enabled),
            SWIFT_METAMEMBER(authenticated),
            SWIFT_METAMEMBER(roles));
    };
} // namespace swift::misc::network

Q_DECLARE_METATYPE(swift::misc::network::CAuthenticatedUser)

#endif // SWIFT_MISC_NETWORK_AUTHENTICATEDUSER_H

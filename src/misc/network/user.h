// SPDX-FileCopyrightText: Copyright (C) 2013 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

//! \file

#ifndef SWIFT_MISC_NETWORK_USER_H
#define SWIFT_MISC_NETWORK_USER_H

#include "misc/aviation/airporticaocode.h"
#include "misc/aviation/callsign.h"
#include "misc/metaclass.h"
#include "misc/propertyindexref.h"
#include "misc/statusmessagelist.h"
#include "misc/valueobject.h"
#include "misc/swiftmiscexport.h"

#include <QMetaType>
#include <QString>

BLACK_DECLARE_VALUEOBJECT_MIXINS(swift::misc::network, CUser)

namespace swift::misc::network
{
    /*!
     * Value object encapsulating information of a user.
     */
    class SWIFT_MISC_EXPORT CUser :
        public CValueObject<CUser>
    {
    public:
        /*!
         * Properties by index
         */
        enum ColumnIndex
        {
            IndexEmail = CPropertyIndexRef::GlobalIndexCUser,
            IndexId,
            IndexIdInteger,
            IndexId7Digit,
            IndexPassword,
            IndexRealName,
            IndexCallsign,
            IndexHomebase
        };

        //! Default constructor.
        CUser() = default;

        //! Constructor by callsign
        CUser(const aviation::CCallsign &callsign);

        //! Constructor.
        CUser(const QString &id, const QString &realname, const aviation::CCallsign &callsign);

        //! Constructor.
        CUser(const QString &id, const QString &realname, const QString &email = "", const QString &password = "", const aviation::CCallsign &callsign = {});

        //! Get full name.
        const QString &getRealName() const { return m_realname; }

        //! Set real name
        void setRealName(const QString &realname);

        //! Get password
        const QString &getPassword() const { return m_password; }

        //! Set password
        void setPassword(const QString &pw);

        //! Valid user object?
        bool isValid() const { return !isNull(); }

        //! Null?
        bool isNull() const { return m_realname.isEmpty() && m_id.isEmpty(); }

        //! Valid credentials?
        bool hasCredentials() const { return this->isValid() && !m_password.isEmpty(); }

        //! Valid real name?
        bool hasRealName() const { return !m_realname.isEmpty(); }

        //! Valid id?
        bool hasId() const { return !m_id.isEmpty(); }

        //! Has a valid VATSIM id?
        bool hasValidVatsimId() const { return CUser::isValidVatsimId(this->getId()); }

        //! Has associated callsign?
        bool hasCallsign() const { return !m_callsign.isEmpty(); }

        //! Has home base?
        bool hasHomeBase() const { return !m_homebase.isEmpty(); }

        //! Has valid home base?
        bool hasValidHomeBase() const;

        //! Has valid or empty home base?
        bool hasValidOrEmptyHomeBase() const;

        //! Real name + homebase
        QString getRealNameAndHomeBase(const QString &separator = QString(" ")) const;

        //! Real name and id
        QString getRealNameAndId() const;

        //! Validate, provide details about issues
        CStatusMessageList validate() const;

        //! Get email.
        const QString &getEmail() const { return m_email; }

        //! Set email.
        void setEmail(const QString &email);

        //! Valid email?
        bool hasValidEmail() const { return !m_email.isEmpty(); }

        //! Get id.
        const QString &getId() const { return m_id; }

        //! Numeric ids get a leading zeros if required
        QString get7DigitId() const;

        //! Id as integer if possible, otherwise -1
        int getIntegerId() const;

        //! Has a numeric id?
        bool hasNumericId() const;

        //! Set id
        void setId(const QString &id);

        //! Homebase
        const aviation::CAirportIcaoCode &getHomeBase() const { return m_homebase; }

        //! Set homebase
        void setHomeBase(const aviation::CAirportIcaoCode &homebase) { m_homebase = homebase; }

        //! Get associated callsign.
        const aviation::CCallsign &getCallsign() const { return m_callsign; }

        //! Set associated callsign
        bool setCallsign(const aviation::CCallsign &callsign);

        //! \copydoc swift::misc::mixin::Icon::toIcon()
        CIcons::IconIndex toIcon() const { return this->getCallsign().toIcon(); }

        //! \copydoc swift::misc::mixin::Index::propertyByIndex
        QVariant propertyByIndex(CPropertyIndexRef index) const;

        //! \copydoc swift::misc::mixin::Index::setPropertyByIndex
        void setPropertyByIndex(CPropertyIndexRef index, const QVariant &variant);

        //! \copydoc swift::misc::mixin::Index::comparePropertyByIndex
        int comparePropertyByIndex(CPropertyIndexRef index, const CUser &compareValue) const;

        //! This and another user exchange missing data, This user has priority and overrides first.
        void synchronizeData(CUser &otherUser);

        //! Update missing parts in this object
        void updateMissingParts(const CUser &otherUser);

        //! Valid VATSIM id
        static bool isValidVatsimId(const QString &id);

        //! Beautify real name, e.g. "JOE DoE" -> "Joe Doe";
        static QString beautifyRealName(const QString &realName);

        //! \copydoc swift::misc::mixin::String::toQString
        QString convertToQString(bool i18n = false) const;

    private:
        //! derive homebase from callsign
        void deriveHomeBaseFromCallsign();

        QString m_id;
        QString m_realname;
        QString m_email;
        QString m_password;
        aviation::CCallsign m_callsign;
        aviation::CAirportIcaoCode m_homebase;

        BLACK_METACLASS(
            CUser,
            BLACK_METAMEMBER(id),
            BLACK_METAMEMBER(realname),
            BLACK_METAMEMBER(email),
            BLACK_METAMEMBER(password),
            BLACK_METAMEMBER(callsign),
            BLACK_METAMEMBER(homebase)
        );
    };
} // namespace

Q_DECLARE_METATYPE(swift::misc::network::CUser)

#endif // guard

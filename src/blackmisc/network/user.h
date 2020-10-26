/* Copyright (C) 2013
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution. No part of swift project, including this file, may be copied, modified, propagated,
 * or distributed except according to the terms contained in the LICENSE file.
 */

//! \file

#ifndef BLACKMISC_NETWORK_USER_H
#define BLACKMISC_NETWORK_USER_H

#include "blackmisc/aviation/airporticaocode.h"
#include "blackmisc/aviation/callsign.h"
#include "blackmisc/obfuscation.h"
#include "blackmisc/icon.h"
#include "blackmisc/metaclass.h"
#include "blackmisc/propertyindex.h"
#include "blackmisc/statusmessagelist.h"
#include "blackmisc/valueobject.h"
#include "blackmisc/variant.h"
#include "blackmisc/blackmiscexport.h"

#include <QMetaType>
#include <QString>

namespace BlackMisc
{
    namespace Network
    {
        /*!
         * Value object encapsulating information of a user.
         */
        class BLACKMISC_EXPORT CUser :
            public CValueObject<CUser>,
            public CObfuscation
        {
        public:
            /*!
             * Properties by index
             */
            enum ColumnIndex
            {
                IndexEmail = CPropertyIndex::GlobalIndexCUser,
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
            CUser(const Aviation::CCallsign &callsign);

            //! Constructor.
            CUser(const QString &id, const QString &realname, const Aviation::CCallsign &callsign);

            //! Constructor.
            CUser(const QString &id, const QString &realname, const QString &email = "", const QString &password = "", const Aviation::CCallsign &callsign = {});

            //! Get full name.
            const QString &getRealName() const { return m_realname; }

            //! Set real name
            void setRealName(const QString &realname);

            //! Get password
            const QString &getPassword() const { return m_password; }

            //! Set password
            void setPassword(const QString &pw) { m_password = decode(pw); }

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
            void setEmail(const QString &email) { m_email = decode(email); }

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
            void setId(const QString &id) { m_id = decode(id); }

            //! Homebase
            const Aviation::CAirportIcaoCode &getHomeBase() const { return m_homebase; }

            //! Set homebase
            void setHomeBase(const Aviation::CAirportIcaoCode &homebase) { m_homebase = homebase; }

            //! Get associated callsign.
            const Aviation::CCallsign &getCallsign() const { return m_callsign; }

            //! Set associated callsign
            bool setCallsign(const Aviation::CCallsign &callsign);

            //! \copydoc BlackMisc::Mixin::Icon::toIcon()
            CIcons::IconIndex toIcon() const { return this->getCallsign().toIcon(); }

            //! \copydoc BlackMisc::Mixin::Index::propertyByIndex
            CVariant propertyByIndex(const CPropertyIndex &index) const;

            //! \copydoc BlackMisc::Mixin::Index::setPropertyByIndex
            void setPropertyByIndex(const CPropertyIndex &index, const CVariant &variant);

            //! \copydoc BlackMisc::Mixin::Index::comparePropertyByIndex
            int comparePropertyByIndex(const CPropertyIndex &index, const CUser &compareValue) const;

            //! This and another user exchange missing data, This user has priority and overrides first.
            void synchronizeData(CUser &otherUser);

            //! Update missing parts in this object
            void updateMissingParts(const CUser &otherUser);

            //! Valid VATSIM id
            static bool isValidVatsimId(const QString &id);

            //! Beautify real name, e.g. "JOE DoE" -> "Joe Doe";
            static QString beautifyRealName(const QString &realName);

            //! \copydoc BlackMisc::Mixin::String::toQString
            QString convertToQString(bool i18n = false) const;

        private:
            //! derive homebase from callsign
            void deriveHomeBaseFromCallsign();

            QString m_id;
            QString m_realname;
            QString m_email;
            QString m_password;
            Aviation::CCallsign m_callsign;
            Aviation::CAirportIcaoCode m_homebase;

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
} // namespace

Q_DECLARE_METATYPE(BlackMisc::Network::CUser)

#endif // guard

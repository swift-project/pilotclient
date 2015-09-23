/* Copyright (C) 2013
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

//! \file

#ifndef BLACKMISC_NETWORK_USER_H
#define BLACKMISC_NETWORK_USER_H

#include "blackmisc/blackmiscexport.h"
#include "blackmisc/aviation/callsign.h"
#include "blackmisc/aviation/airporticaocode.h"
#include "blackmisc/propertyindex.h"
#include "blackmisc/statusmessagelist.h"

namespace BlackMisc
{

    namespace Network
    {

        /*!
         * Value object encapsulating information of a user.
         */
        class BLACKMISC_EXPORT CUser : public CValueObject<CUser>
        {
        public:
            /*!
             * Properties by index
             */
            enum ColumnIndex
            {
                IndexEmail = BlackMisc::CPropertyIndex::GlobalIndexCUser,
                IndexId,
                IndexPassword,
                IndexRealName,
                IndexCallsign,
                IndexHomebase
            };

            //! Default constructor.
            CUser() = default;

            //! Constructor by callsign
            CUser(const BlackMisc::Aviation::CCallsign &callsign);

            //! Constructor.
            CUser(const QString &id, const QString &realname, const BlackMisc::Aviation::CCallsign &callsign);

            //! Constructor.
            CUser(const QString &id, const QString &realname, const QString &email = "", const QString &password = "", const BlackMisc::Aviation::CCallsign &callsign = {});

            //! Get full name.
            QString getRealName() const { return m_realname; }

            //! setRealName
            void setRealName(const QString &realname);

            //! Get password
            QString getPassword() const { return m_password; }

            //! Set password
            void setPassword(const QString &pw) { m_password = pw.trimmed(); }

            //! Valid user object?
            bool isValid() const { return !this->m_realname.isEmpty() &&  !this->m_id.isEmpty(); }

            //! Valid credentials?
            bool hasValidCredentials() const { return this->isValid() && !this->m_password.isEmpty(); }

            //! Valid real name?
            bool hasValidRealName() const { return !this->m_realname.isEmpty(); }

            //! Valid id?
            bool hasValidId() const { return !this->m_id.isEmpty(); }

            //! Has associated callsign?
            bool hasValidCallsign() const { return !m_callsign.isEmpty(); }

            //! Valid homebase
            bool hasValidHomebase() const { return !m_homebase.isEmpty(); }

            //! Validate, provide details about issues
            BlackMisc::CStatusMessageList validate() const;

            //! Get email.
            QString getEmail() const { return m_email; }

            //! Set email.
            void setEmail(const QString &email) { m_email = email.trimmed(); }

            //! Valid email?
            bool hasValidEmail() const { return !this->m_email.isEmpty(); }

            //! Get id.
            QString getId() const { return m_id; }

            //! Set id
            void setId(const QString &id) { m_id = id.trimmed(); }

            //! Homebase
            const BlackMisc::Aviation::CAirportIcaoCode &getHomebase() const { return this->m_homebase; }

            //! Set homebase
            void setHomebase(const BlackMisc::Aviation::CAirportIcaoCode &homebase) { this->m_homebase = homebase; }

            //! Get associated callsign.
            const BlackMisc::Aviation::CCallsign &getCallsign() const { return m_callsign; }

            //! Set associated callsign
            void setCallsign(const BlackMisc::Aviation::CCallsign &callsign);

            //! \copydoc CValueObject::toIcon()
            BlackMisc::CIcon toIcon() const { return this->getCallsign().toIcon(); }

            //! \copydoc CValueObject::propertyByIndex
            CVariant propertyByIndex(const BlackMisc::CPropertyIndex &index) const;

            //! \copydoc CValueObject::setPropertyByIndex
            void setPropertyByIndex(const CVariant &variant, const BlackMisc::CPropertyIndex &index);

            //! This and another user exchange missing data, This user has priority and overrides first.
            void syncronizeData(CUser &otherUser);

            //! Update missing parts in this object
            void updateMissingParts(const CUser &otherUser);

            //! Valid VATSIM id
            static bool isValidVatsimId(const QString &id);

            //! Beautify real name, e.g. "JOE DoE" -> "Joe Doe";
            static QString beautifyRealName(const QString &realName);

            //! \copydoc CValueObject::convertToQString
            QString convertToQString(bool i18n = false) const;

        private:
            BLACK_ENABLE_TUPLE_CONVERSION(CUser)

            //! derive homebase from callsign
            void deriveHomeBaseFromCallsign();

            QString m_id;
            QString m_realname;
            QString m_email;
            QString m_password;
            BlackMisc::Aviation::CCallsign m_callsign;
            BlackMisc::Aviation::CAirportIcaoCode m_homebase;
        };
    } // namespace
} // namespace

BLACK_DECLARE_TUPLE_CONVERSION(BlackMisc::Network::CUser, (o.m_id, o.m_realname, o.m_email, o.m_password, o.m_callsign, o.m_homebase))
Q_DECLARE_METATYPE(BlackMisc::Network::CUser)

#endif // guard

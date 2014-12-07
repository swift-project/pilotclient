/* Copyright (C) 2013
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

#include "nwuser.h"
#include "blackmisc/icon.h"
#include "blackmisc/blackmiscfreefunctions.h"
#include "blackmisc/propertyindex.h"
#include "blackmisc/variant.h"
#include <tuple>

namespace BlackMisc
{
    namespace Network
    {
        QString CUser::convertToQString(bool /** i18n **/) const
        {
            if (this->m_realname.isEmpty()) return "<no realname>";
            QString s = this->m_realname;
            if (this->hasValidId())
            {
                s.append(" (").append(this->m_id).append(')');
            }
            if (this->hasValidCallsign())
            {
                s.append(' ').append(this->getCallsign().getStringAsSet());
            }
            return s;
        }

        CStatusMessageList CUser::validate() const
        {
            CStatusMessageList msgs;
            // callsign optional
            if (!this->hasValidId()) { msgs.push_back(CStatusMessage(CStatusMessage::SeverityWarning, "Invalid id"));}
            if (!this->hasValidRealName()) { msgs.push_back(CStatusMessage(CStatusMessage::SeverityWarning, "Invalid real name"));}
            if (!this->hasValidCredentials()) { msgs.push_back(CStatusMessage(CStatusMessage::SeverityWarning, "Invalid credentials"));}
            return msgs;
        }

        /*
         * Exchange data
         */
        void CUser::syncronizeData(CUser &otherUser)
        {
            if (otherUser == (*this)) return;

            if (this->hasValidRealName())
                otherUser.setRealName(this->getRealName());
            else if (otherUser.hasValidRealName())
                this->setRealName(otherUser.getRealName());

            if (this->hasValidId())
                otherUser.setId(this->getId());
            else if (otherUser.hasValidId())
                this->setId(otherUser.getId());

            if (this->hasValidEmail())
                otherUser.setEmail(this->getEmail());
            else if (otherUser.hasValidEmail())
                this->setEmail(otherUser.getEmail());

            if (this->hasValidCallsign())
                otherUser.setCallsign(this->getCallsign());
            else if (otherUser.hasValidCallsign())
                this->setCallsign(otherUser.getCallsign());
        }

        bool CUser::isValidVatsimId(const QString &id)
        {
            if (id.isEmpty()) { return false; }
            bool ok;
            int i = id.toInt(&ok);
            if (!ok) { return false; }
            return i >= 100000 && i <= 9999999;
        }

        /*
         * Property by index
         */
        CVariant CUser::propertyByIndex(const BlackMisc::CPropertyIndex &index) const
        {
            if (index.isMyself()) { return this->toCVariant(); }
            ColumnIndex i = index.frontCasted<ColumnIndex>();
            switch (i)
            {
            case IndexEmail:
                return CVariant(this->m_email);
            case IndexId:
                return CVariant(this->m_id);
            case IndexPassword:
                return CVariant(this->m_password);
            case IndexRealName:
                return CVariant(this->m_realname);
            case IndexCallsign:
                return this->m_callsign.propertyByIndex(index.copyFrontRemoved());
            default:
                return CValueObject::propertyByIndex(index);
            }
        }

        /*
         * Set property as index
         */
        void CUser::setPropertyByIndex(const CVariant &variant, const BlackMisc::CPropertyIndex &index)
        {
            if (index.isMyself())
            {
                this->convertFromCVariant(variant);
                return;
            }
            ColumnIndex i = index.frontCasted<ColumnIndex>();
            switch (i)
            {
            case IndexEmail:
                this->setEmail(variant.value<QString>());
                break;
            case IndexId:
                this->setId(variant.value<QString>());
                break;
            case IndexPassword:
                this->setPassword(variant.value<QString>());
                break;
            case IndexRealName:
                this->setRealName(variant.value<QString>());
                break;
            case IndexCallsign:
                this->m_callsign.setPropertyByIndex(variant, index.copyFrontRemoved());
                break;
            default:
                CValueObject::setPropertyByIndex(variant, index);
                break;
            }
        }
    } // namespace
} // namespace

/* Copyright (C) 2016
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

#include "blackmisc/network/rawfsdmessage.h"
#include "blackmisc/logcategory.h"
#include "blackmisc/logcategorylist.h"
#include "blackmisc/propertyindex.h"
#include "blackmisc/statusmessage.h"
#include "blackmisc/stringutils.h"
#include "blackmisc/variant.h"

#include <Qt>
#include <QtGlobal>

using namespace BlackMisc;

namespace BlackMisc
{
    namespace Network
    {
        CRawFsdMessage::CRawFsdMessage(const QString &rawMessage)
            : m_rawMessage(rawMessage) {}

        QString CRawFsdMessage::convertToQString(bool i18n) const
        {
            Q_UNUSED(i18n);
            static const QString s("%1 %2");
            return s.arg(m_receptionTime.toString("dd.MM.yy HH:mm:ss"), m_rawMessage);
        }

        bool CRawFsdMessage::isPacketType(const QString &type) const
        {
            return m_rawMessage.startsWith("FSD Sent=>" + type) || m_rawMessage.startsWith("FSD Recv=>" + type);
        }

        bool CRawFsdMessage::containsString(const QString &str) const
        {
            return m_rawMessage.contains(str);
        }

        const QStringList &CRawFsdMessage::getAllPacketTypes()
        {
            static const QStringList allPacketTypes = { "@", "%", "#AA", "#DA", "#AP", "#DP", "#TM", "#WX", "#DL", "#TD", "#WD"
                                                          "#CD", "#PC", "#SB", "$FP", "$AM", "$PI", "$PO", "$HO", "$HA", "$AX", "$AR",
                                                          "$CQ", "$CR", "$ER", "$!!" };
            return allPacketTypes;
        }

        CVariant CRawFsdMessage::propertyByIndex(const BlackMisc::CPropertyIndex &index) const
        {
            if (index.isMyself()) { return CVariant::from(*this); }
            ColumnIndex i = index.frontCasted<ColumnIndex>();
            switch (i)
            {
            case IndexReceptionTime:
                return CVariant::fromValue(this->m_rawMessage);
            case IndexRawMessage:
                return CVariant::fromValue(this->m_receptionTime);
            default:
                return CValueObject::propertyByIndex(index);
            }
        }

        void CRawFsdMessage::setPropertyByIndex(const CPropertyIndex &index, const CVariant &variant)
        {
            if (index.isMyself()) { (*this) = variant.to<CRawFsdMessage>(); return; }
            ColumnIndex i = index.frontCasted<ColumnIndex>();
            switch (i)
            {
            case IndexRawMessage:
                this->setRawMessage(variant.value<QString>());
                break;
            default:
                CValueObject::setPropertyByIndex(index, variant);
                break;
            }
        }
    } // namespace
} // namespace

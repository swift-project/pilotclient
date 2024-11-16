// SPDX-FileCopyrightText: Copyright (C) 2016 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

#include "misc/network/rawfsdmessage.h"

#include <QtGlobal>

#include "misc/propertyindexref.h"
#include "misc/statusmessage.h"
#include "misc/stringutils.h"

using namespace swift::misc;

SWIFT_DEFINE_VALUEOBJECT_MIXINS(swift::misc::network, CRawFsdMessage)

namespace swift::misc::network
{
    CRawFsdMessage::CRawFsdMessage(const QString &rawMessage)
        : m_rawMessage(rawMessage) {}

    QString CRawFsdMessage::convertToQString(bool i18n) const
    {
        Q_UNUSED(i18n)
        return QStringLiteral("%1 %2").arg(this->getFormattedUtcTimestampHmsz(), m_rawMessage);
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
                                                                                                                      "#CD",
                                                    "#PC", "#SB", "$FP", "$AM", "$PI", "$PO", "$HO", "$HA", "$AX", "$AR",
                                                    "$CQ", "$CR", "$ER", "$!!" };
        return allPacketTypes;
    }

    QVariant CRawFsdMessage::propertyByIndex(CPropertyIndexRef index) const
    {
        if (index.isMyself()) { return QVariant::fromValue(*this); }
        if (ITimestampBased::canHandleIndex(index)) { return ITimestampBased::propertyByIndex(index); }

        const ColumnIndex i = index.frontCasted<ColumnIndex>();
        switch (i)
        {
        case IndexRawMessage: return QVariant::fromValue(m_rawMessage);
        default: return CValueObject::propertyByIndex(index);
        }
    }

    void CRawFsdMessage::setPropertyByIndex(CPropertyIndexRef index, const QVariant &variant)
    {
        if (index.isMyself())
        {
            (*this) = variant.value<CRawFsdMessage>();
            return;
        }
        if (ITimestampBased::canHandleIndex(index))
        {
            ITimestampBased::setPropertyByIndex(index, variant);
            return;
        }

        const ColumnIndex i = index.frontCasted<ColumnIndex>();
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
} // namespace swift::misc::network

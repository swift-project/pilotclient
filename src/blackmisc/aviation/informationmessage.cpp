// SPDX-FileCopyrightText: Copyright (C) 2013 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

#include "blackmisc/aviation/informationmessage.h"
#include "blackmisc/comparefunctions.h"

BLACK_DEFINE_VALUEOBJECT_MIXINS(BlackMisc::Aviation, CInformationMessage)

namespace BlackMisc::Aviation
{
    QString CInformationMessage::convertToQString(bool i18n) const
    {
        Q_UNUSED(i18n);
        return m_message;
    }

    void CInformationMessage::registerMetadata()
    {
        CValueObject<CInformationMessage>::registerMetadata();
        qRegisterMetaType<CInformationMessage::InformationType>();
    }

    const CInformationMessage &CInformationMessage::unspecified()
    {
        static const CInformationMessage u(Unspecified);
        return u;
    }

    const QString &CInformationMessage::getTypeAsString() const
    {
        switch (m_type)
        {
        case ATIS:
        {
            static const QString atis("ATIS");
            return atis;
        }
        case METAR:
        {
            static const QString metar("METAR");
            return metar;
        }
        case TAF:
        {
            static const QString taf("TAF");
            return taf;
        }
        default:
        {
            static const QString ds("unknown");
            return ds;
        }
        }
    }

    QVariant CInformationMessage::propertyByIndex(CPropertyIndexRef index) const
    {
        if (index.isMyself()) { return QVariant::fromValue(*this); }
        if (ITimestampBased::canHandleIndex(index)) { return ITimestampBased::propertyByIndex(index); }
        const ColumnIndex i = index.frontCasted<ColumnIndex>();
        switch (i)
        {
        case IndexType: return QVariant::fromValue(m_type);
        case IndexMessage: return QVariant::fromValue(m_message);
        default: break;
        }
        return CValueObject::propertyByIndex(index);
    }

    void CInformationMessage::setPropertyByIndex(CPropertyIndexRef index, const QVariant &variant)
    {
        if (index.isMyself())
        {
            (*this) = variant.value<CInformationMessage>();
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
        case IndexType: m_type = static_cast<InformationType>(variant.toInt()); break;
        case IndexMessage: m_message = variant.toString(); break;
        default: break;
        }
        CValueObject::setPropertyByIndex(index, variant);
    }

    int CInformationMessage::comparePropertyByIndex(CPropertyIndexRef index, const CInformationMessage &compareValue) const
    {
        if (index.isMyself())
        {
            const int c = Compare::compare(m_type, compareValue.m_type);
            if (c != 0) return c;
            return m_message.compare(compareValue.m_message, Qt::CaseInsensitive);
        }
        if (ITimestampBased::canHandleIndex(index)) { return ITimestampBased::comparePropertyByIndex(index, compareValue); }
        const ColumnIndex i = index.frontCasted<ColumnIndex>();
        switch (i)
        {
        case IndexMessage: return m_message.compare(compareValue.m_message, Qt::CaseInsensitive);
        case IndexType: return Compare::compare(this->getType(), compareValue.getType());
        default:
            return CValueObject::comparePropertyByIndex(index, *this);
        }
        Q_ASSERT_X(false, Q_FUNC_INFO, "Compare failed");
        return 0;
    }
} // namespace

// SPDX-FileCopyrightText: Copyright (C) 2015 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

#include "core/data/vatsimsetup.h"
#include <QStringList>

using namespace swift::misc;
using namespace swift::misc::network;

BLACK_DEFINE_VALUEOBJECT_MIXINS(swift::core::data, CVatsimSetup)

namespace swift::core::data
{
    CVatsimSetup::CVatsimSetup() : ITimestampBased(0)
    {}

    bool CVatsimSetup::setUrls(const CUrl &dataFileUrl, const CUrl &serverFileUrl, const CUrl &metarFileUrl)
    {
        const bool changed = (dataFileUrl != getDataFileUrl() || serverFileUrl != getServerFileUrl() || metarFileUrl != getMetarFileUrl());
        this->setServerFileUrl(serverFileUrl);
        this->setMetarFileUrl(metarFileUrl);
        this->setDataFileUrl(dataFileUrl);
        return changed;
    }

    bool CVatsimSetup::setServers(const CServerList &fsdServers, const CServerList &voiceServers)
    {
        const bool changed = (this->getVoiceServers() != voiceServers || this->getFsdServers() != fsdServers);
        this->setFsdServers(fsdServers);
        this->setVoiceServers(voiceServers);
        return changed;
    }

    QString CVatsimSetup::convertToQString(bool i18n) const
    {
        return convertToQString(", ", i18n);
    }

    QString CVatsimSetup::convertToQString(const QString &separator, bool i18n) const
    {
        QString s("timestamp: ");
        s.append(this->getFormattedUtcTimestampYmdhms());
        s.append(separator);

        s.append("VATSIM data file: ");
        s.append(getDataFileUrl().toQString(i18n));
        s.append(separator);

        s.append("FSD servers: ");
        s.append(getFsdServers().toQString(i18n));
        return s;
    }

    QVariant CVatsimSetup::propertyByIndex(swift::misc::CPropertyIndexRef index) const
    {
        if (index.isMyself()) { return QVariant::fromValue(*this); }
        if (ITimestampBased::canHandleIndex(index)) { return ITimestampBased::propertyByIndex(index); }

        const ColumnIndex i = index.frontCasted<ColumnIndex>();
        switch (i)
        {
        case IndexFsdServers: return QVariant::fromValue(this->m_fsdServers);
        case IndexDataFiles: return QVariant::fromValue(this->m_dataFileUrl);
        default: return CValueObject::propertyByIndex(index);
        }
    }

    void CVatsimSetup::setPropertyByIndex(CPropertyIndexRef index, const QVariant &variant)
    {
        if (index.isMyself())
        {
            (*this) = variant.value<CVatsimSetup>();
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
        case IndexFsdServers: this->m_fsdServers = variant.value<CServerList>(); break;
        case IndexDataFiles: this->m_dataFileUrl = variant.value<CUrl>(); break;
        default: CValueObject::setPropertyByIndex(index, variant); break;
        }
    }
} // ns

// SPDX-FileCopyrightText: Copyright (C) 2015 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

#include "blackcore/data/vatsimsetup.h"
#include <QStringList>

using namespace BlackMisc;
using namespace BlackMisc::Network;

BLACK_DEFINE_VALUEOBJECT_MIXINS(BlackCore::Data, CVatsimSetup)

namespace BlackCore::Data
{
    CVatsimSetup::CVatsimSetup() : ITimestampBased(0),
                                   m_dataFileUrls(QStringList({ "http://info.vroute.net/vatsim-data.txt" }))
    {}

    bool CVatsimSetup::setUrls(const CUrlList &dataFileUrls, const CUrlList &serverFileUrls, const CUrlList &metarFileUrls)
    {
        const bool changed = (dataFileUrls != getDataFileUrls() || serverFileUrls != getServerFileUrls() || metarFileUrls != getMetarFileUrls());
        this->setServerFileUrls(serverFileUrls);
        this->setMetarFileUrls(metarFileUrls);
        this->setDataFileUrls(dataFileUrls);
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
        s.append(getDataFileUrls().toQString(i18n));
        s.append(separator);

        s.append("FSD servers: ");
        s.append(getFsdServers().toQString(i18n));
        return s;
    }

    QVariant CVatsimSetup::propertyByIndex(BlackMisc::CPropertyIndexRef index) const
    {
        if (index.isMyself()) { return QVariant::fromValue(*this); }
        if (ITimestampBased::canHandleIndex(index)) { return ITimestampBased::propertyByIndex(index); }

        const ColumnIndex i = index.frontCasted<ColumnIndex>();
        switch (i)
        {
        case IndexFsdServers: return QVariant::fromValue(this->m_fsdServers);
        case IndexDataFiles: return QVariant::fromValue(this->m_dataFileUrls);
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
        case IndexDataFiles: this->m_dataFileUrls = variant.value<CUrlList>(); break;
        default: CValueObject::setPropertyByIndex(index, variant); break;
        }
    }
} // ns

/* Copyright (C) 2015
 * swift project community / contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

#include "blackcore/data/vatsimsetup.h"

#include <QStringList>

using namespace BlackMisc;
using namespace BlackMisc::Network;

namespace BlackCore
{
    namespace Data
    {
        CVatsimSetup::CVatsimSetup() :
            ITimestampBased(0),
            m_dataFileUrls(QStringList( { "http://info.vroute.net/vatsim-data.txt" }))
        { }

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

        CVariant CVatsimSetup::propertyByIndex(const BlackMisc::CPropertyIndex &index) const
        {
            if (index.isMyself()) { return CVariant::from(*this); }
            if (ITimestampBased::canHandleIndex(index)) { return ITimestampBased::propertyByIndex(index); }

            ColumnIndex i = index.frontCasted<ColumnIndex>();
            switch (i)
            {
            case IndexFsdServers:
                return CVariant::fromValue(this->m_fsdServers);
            case IndexDataFiles:
                return CVariant::fromValue(this->m_dataFileUrls);
            default:
                return CValueObject::propertyByIndex(index);
            }
        }

        void CVatsimSetup::setPropertyByIndex(const CPropertyIndex &index, const CVariant &variant)
        {
            if (index.isMyself()) { (*this) = variant.to<CVatsimSetup>(); return; }
            if (ITimestampBased::canHandleIndex(index))
            {
                ITimestampBased::setPropertyByIndex(index, variant);
                return;
            }

            ColumnIndex i = index.frontCasted<ColumnIndex>();
            switch (i)
            {
            case IndexFsdServers:
                this->m_fsdServers = variant.value<CServerList>();
                break;
            case IndexDataFiles:
                this->m_dataFileUrls = variant.value<CUrlList>();
                break;
            default:
                CValueObject::setPropertyByIndex(index, variant);
                break;
            }
        }

    } // ns
} // ns

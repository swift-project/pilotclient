/* Copyright (C) 2018
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

#include "blackmisc/audio/voicesetup.h"
#include "blackmisc/logcategorylist.h"
#include "blackmisc/stringutils.h"
#include "blackmisc/verify.h"
#include "blackmisc/comparefunctions.h"

#include <Qt>
#include <QtGlobal>

namespace BlackMisc
{
    namespace Audio
    {
        QString CVoiceSetup::convertToQString(bool i18n) const
        {
            Q_UNUSED(i18n);
            static const QString s("Port: %1");
            return s.arg(getVatsimUdpVoicePort());
        }

        CStatusMessageList CVoiceSetup::validate() const
        {
            static const CLogCategoryList cats(CLogCategoryList(this).join({ CLogCategory::validation()}));
            CStatusMessageList msgs;
            if (this->getVatsimUdpVoicePort() < 1 || this->getVatsimUdpVoicePort() > 65535) { msgs.push_back(CStatusMessage(CStatusMessage::SeverityError, "Invalid voice port")); }
            msgs.addCategories(cats);
            return msgs;
        }

        const CVoiceSetup &CVoiceSetup::vatsimStandard()
        {
            static const CVoiceSetup s;
            return s;
        }

        CVoiceSetup::CVoiceSetup(int vatsimUdpPort) : m_vatismVoiceUdpPort(vatsimUdpPort)
        { }

        CVariant CVoiceSetup::propertyByIndex(const BlackMisc::CPropertyIndex &index) const
        {
            if (index.isMyself()) { return CVariant::from(*this); }
            const ColumnIndex i = index.frontCasted<ColumnIndex>();
            switch (i)
            {
            case IndexVatsimVoiceUdpPort: return CVariant::fromValue(m_vatismVoiceUdpPort);
            default: return CValueObject::propertyByIndex(index);
            }
        }

        void CVoiceSetup::setPropertyByIndex(const CPropertyIndex &index, const CVariant &variant)
        {
            if (index.isMyself()) { (*this) = variant.to<CVoiceSetup>(); return; }
            const ColumnIndex i = index.frontCasted<ColumnIndex>();
            switch (i)
            {
            case IndexVatsimVoiceUdpPort: m_vatismVoiceUdpPort = variant.toInt(); break;
            default:
                CValueObject::setPropertyByIndex(index, variant);
                break;
            }
        }

        int CVoiceSetup::comparePropertyByIndex(const CPropertyIndex &index, const CVoiceSetup &compareValue) const
        {
            if (index.isMyself()) { return this->convertToQString(true).compare(compareValue.convertToQString()); }
            const ColumnIndex i = index.frontCasted<ColumnIndex>();
            switch (i)
            {
            case IndexVatsimVoiceUdpPort: return Compare::compare(m_vatismVoiceUdpPort, compareValue.m_vatismVoiceUdpPort);
            default: break;
            }
            BLACK_VERIFY_X(false, Q_FUNC_INFO, qUtf8Printable("No comparison for index " + index.toQString()));
            return 0;
        }
    } // namespace
} // namespace

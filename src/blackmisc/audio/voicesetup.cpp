/* Copyright (C) 2018
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution. No part of swift project, including this file, may be copied, modified, propagated,
 * or distributed except according to the terms contained in the LICENSE file.
 */

#include "blackmisc/audio/voicesetup.h"
#include "blackmisc/logcategories.h"
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
            Q_UNUSED(i18n)
            return QStringLiteral("Port: %1").arg(getAfvVoiceServerUrl());
        }

        CStatusMessageList CVoiceSetup::validate() const
        {
            static const CLogCategoryList cats(CLogCategoryList(this).withValidation());
            CStatusMessageList msgs;
            if (this->getAfvVoiceServerUrl().isEmpty()) { msgs.push_back(CStatusMessage(CStatusMessage::SeverityError, u"Invalid voice server url")); }
            if (this->getAfvMapUrl().isEmpty()) { msgs.push_back(CStatusMessage(CStatusMessage::SeverityError, u"Invalid voice server url")); }
            msgs.addCategories(cats);
            return msgs;
        }

        const CVoiceSetup &CVoiceSetup::vatsimStandard()
        {
            static const CVoiceSetup s;
            return s;
        }

        CVoiceSetup::CVoiceSetup(const QString &afvVoiceServerUrl, const QString &afvMapUrl) :
            m_afvVoiceServerUrl(afvVoiceServerUrl), m_afvMapUrl(afvMapUrl)
        { }

        CVariant CVoiceSetup::propertyByIndex(const BlackMisc::CPropertyIndex &index) const
        {
            if (index.isMyself()) { return CVariant::from(*this); }
            const ColumnIndex i = index.frontCasted<ColumnIndex>();
            switch (i)
            {
            case IndexAfvVoiceServerUrl: return CVariant::fromValue(m_afvVoiceServerUrl);
            case IndexAfvMapUrl:         return CVariant::fromValue(m_afvMapUrl);
            default: return CValueObject::propertyByIndex(index);
            }
        }

        void CVoiceSetup::setPropertyByIndex(const CPropertyIndex &index, const CVariant &variant)
        {
            if (index.isMyself()) { (*this) = variant.to<CVoiceSetup>(); return; }
            const ColumnIndex i = index.frontCasted<ColumnIndex>();
            switch (i)
            {
            case IndexAfvVoiceServerUrl: m_afvVoiceServerUrl = variant.toQString(); break;
            case IndexAfvMapUrl:         m_afvMapUrl         = variant.toQString(); break;
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
            case IndexAfvVoiceServerUrl: return m_afvVoiceServerUrl.compare(compareValue.m_afvVoiceServerUrl, Qt::CaseInsensitive);
            case IndexAfvMapUrl:         return m_afvMapUrl.compare(compareValue.m_afvMapUrl, Qt::CaseInsensitive);
            default: break;
            }
            BLACK_VERIFY_X(false, Q_FUNC_INFO, qUtf8Printable("No comparison for index " + index.toQString()));
            return 0;
        }
    } // namespace
} // namespace

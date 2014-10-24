/* Copyright (C) 2013
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

#include "blackmisc/nwvoicecapabilities.h"
#include "blackmisc/blackmiscfreefunctions.h"
#include "blackmisc/iconlist.h"
#include <tuple>

using namespace BlackMisc;

namespace BlackMisc
{
    namespace Network
    {

        /*
         * Constructor
         */
        CVoiceCapabilities::CVoiceCapabilities(const QString &flightPlanRemarks)
        {
            this->setFromFlightPlanRemarks(flightPlanRemarks);
        }

        /*
         * Convert to string
         */
        QString CVoiceCapabilities::convertToQString(bool i18n) const
        {
            Q_UNUSED(i18n);
            switch (this->m_voiceCapabilities)
            {
            case Voice:
                {
                    static const QString v("voice");
                    return v;
                }
            case TextOnly:
                {
                    static const QString t("text only");
                    return t;
                }
            case VoiceReceivingOnly:
                {
                    static const QString r("voice listening only");
                    return r;
                }
            case Unknown:
            default:
                {
                    static const QString u("unknown");
                    return u;
                }
            }
            Q_ASSERT("Wrong index");
            return QString(""); // never reached
        }

        void CVoiceCapabilities::setFromFlightPlanRemarks(const QString &flightPlanRemarks)
        {
            // http://www.vatpac.org/cms/index.php?option=com_content&view=article&id=608&Itemid=213
            if (flightPlanRemarks.isEmpty())
            {
                this->setCapabilities(Unknown);
                return;
            }

            const QString r = flightPlanRemarks.toLower();
            if (r.contains("/v/") || r.contains("/voice/"))
            {
                this->setCapabilities(Voice);
                return;
            }

            if (r.contains("/t/"))
            {
                this->setCapabilities(TextOnly);
                return;
            }

            if (r.contains("/r/"))
            {
                this->setCapabilities(VoiceReceivingOnly);
                return;
            }

            this->setCapabilities(Unknown);
        }

        /*
         * Icon
         */
        CIcon CVoiceCapabilities::toIcon() const
        {
            switch (this->m_voiceCapabilities)
            {
            case Voice:
                return CIconList::iconByIndex(CIcons::NetworkCapabilityVoiceBackground);
            case TextOnly:
                return CIconList::iconByIndex(CIcons::NetworkCapabilityTextOnly);
            case Unknown:
                return CIconList::iconByIndex(CIcons::NetworkCapabilityUnknown);
            case VoiceReceivingOnly:
                return CIconList::iconByIndex(CIcons::NetworkCapabilityVoiceReceiveOnlyBackground);
            default:
                break;
            }
            Q_ASSERT(false);
            return CIconList::iconByIndex(CIcons::NetworkCapabilityUnknown); // never reached
        }

        /*
         * From enum
         */
        const CVoiceCapabilities &CVoiceCapabilities::fromVoiceCapabilities(CVoiceCapabilities::VoiceCapabilities capabilities)
        {
            static const CVoiceCapabilities u(CVoiceCapabilities::Unknown);
            static const CVoiceCapabilities to(CVoiceCapabilities::TextOnly);
            static const CVoiceCapabilities v(CVoiceCapabilities::Voice);
            static const CVoiceCapabilities vro(CVoiceCapabilities::VoiceReceivingOnly);

            switch (capabilities)
            {
            case CVoiceCapabilities::TextOnly:
                return to;
            case CVoiceCapabilities::Voice:
                return v;
            case CVoiceCapabilities::VoiceReceivingOnly:
                return vro;
            case CVoiceCapabilities::Unknown:
            default:
                return u;
            }
        }

        /*
         * All
         */
        const QList<CVoiceCapabilities> &CVoiceCapabilities::allCapabilities()
        {
            static const QList<CVoiceCapabilities> all({fromVoiceCapabilities(Unknown), fromVoiceCapabilities(Voice), fromVoiceCapabilities(VoiceReceivingOnly), fromVoiceCapabilities(TextOnly)});
            return all;
        }

    } // namespace
} // namespace

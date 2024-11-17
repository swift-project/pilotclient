// SPDX-FileCopyrightText: Copyright (C) 2013 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

#include "misc/network/voicecapabilities.h"

#include <QList>
#include <QString>
#include <QtGlobal>

#include "misc/icons.h"

using namespace swift::misc;

SWIFT_DEFINE_VALUEOBJECT_MIXINS(swift::misc::network, CVoiceCapabilities)

namespace swift::misc::network
{
    CVoiceCapabilities::CVoiceCapabilities(const QString &flightPlanRemarks)
    {
        this->setFromFlightPlanRemarks(flightPlanRemarks);
    }

    const QString &CVoiceCapabilities::toFlightPlanRemarks() const
    {
        static const QString v("/V/");
        static const QString t("/T/");
        static const QString r("/R/");
        static const QString u("");

        switch (m_voiceCapabilities)
        {
        case Voice: return v;
        case TextOnly: return t;
        case Unknown: return u;
        case VoiceReceivingOnly: return r;
        default: break;
        }
        Q_ASSERT_X(false, Q_FUNC_INFO, "Illegal mode");
        return u;
    }

    QString CVoiceCapabilities::convertToQString(bool i18n) const
    {
        Q_UNUSED(i18n);
        static const QString v("voice");
        static const QString t("text only");
        static const QString r("voice listening only");
        static const QString u("unknown");

        switch (m_voiceCapabilities)
        {
        case Voice: return v;
        case TextOnly: return t;
        case VoiceReceivingOnly: return r;
        case Unknown: return u;
        default: break;
        }
        Q_ASSERT_X(false, Q_FUNC_INFO, "Illegal mode");
        return u;
    }

    void CVoiceCapabilities::setFromFlightPlanRemarks(const QString &flightPlanRemarks)
    {
        // http://www.vatpac.org/cms/index.php?option=com_content&view=article&id=608&Itemid=213

        // this function is very frequently called, so I put the most likely scenarios on top
        const QString r = flightPlanRemarks.toUpper().trimmed();
        if (flightPlanRemarks.isEmpty())
        {
            this->setCapabilities(Unknown);
            return;
        }
        if (r.contains("/V/"))
        {
            this->setCapabilities(Voice);
            return;
        }
        if (r.contains("/T/"))
        {
            this->setCapabilities(TextOnly);
            return;
        }
        if (r.contains("/R/"))
        {
            this->setCapabilities(VoiceReceivingOnly);
            return;
        }
        if (r.contains("/VOICE/"))
        {
            this->setCapabilities(Voice);
            return;
        }
        if (r.contains("/TEXT/"))
        {
            this->setCapabilities(TextOnly);
            return;
        }
        this->setCapabilities(Unknown);
    }

    CIcons::IconIndex CVoiceCapabilities::toIcon() const
    {
        switch (m_voiceCapabilities)
        {
        case Voice: return CIcons::NetworkCapabilityVoiceBackground;
        case TextOnly: return CIcons::NetworkCapabilityTextOnly;
        case VoiceReceivingOnly: return CIcons::NetworkCapabilityVoiceReceiveOnlyBackground;
        case Unknown: return CIcons::NetworkCapabilityUnknown;
        default: break;
        }
        Q_ASSERT_X(false, Q_FUNC_INFO, "Illegal mode");
        return CIcons::NetworkCapabilityUnknown; // never reached
    }

    const CVoiceCapabilities &
    CVoiceCapabilities::fromVoiceCapabilities(CVoiceCapabilities::VoiceCapabilities capabilities)
    {
        static const CVoiceCapabilities u(CVoiceCapabilities::Unknown);
        static const CVoiceCapabilities to(CVoiceCapabilities::TextOnly);
        static const CVoiceCapabilities v(CVoiceCapabilities::Voice);
        static const CVoiceCapabilities vro(CVoiceCapabilities::VoiceReceivingOnly);

        switch (capabilities)
        {
        case CVoiceCapabilities::TextOnly: return to;
        case CVoiceCapabilities::Voice: return v;
        case CVoiceCapabilities::VoiceReceivingOnly: return vro;
        case CVoiceCapabilities::Unknown: return u;
        default: break;
        }
        Q_ASSERT_X(false, Q_FUNC_INFO, "Illegal mode");
        return u; // normally never reached
    }

    CVoiceCapabilities CVoiceCapabilities::fromFlightPlanRemarks(const QString &remarks)
    {
        return CVoiceCapabilities(remarks);
    }

    CVoiceCapabilities CVoiceCapabilities::fromText(const QString &text)
    {
        if (text.startsWith("/"))
        {
            const CVoiceCapabilities vc(text);
            return vc;
        }
        if (text.contains("TEXT", Qt::CaseInsensitive)) { return CVoiceCapabilities(TextOnly); }
        if (text.contains("ONLY", Qt::CaseInsensitive)) { return CVoiceCapabilities(TextOnly); }
        if (text.contains("RECEIVE", Qt::CaseInsensitive)) { return CVoiceCapabilities(VoiceReceivingOnly); }
        if (text.contains("VOICE", Qt::CaseInsensitive)) { return CVoiceCapabilities(Voice); }
        return CVoiceCapabilities(Unknown);
    }

    const QList<CVoiceCapabilities> &CVoiceCapabilities::allCapabilities()
    {
        static const QList<CVoiceCapabilities> all({ fromVoiceCapabilities(Unknown), fromVoiceCapabilities(Voice),
                                                     fromVoiceCapabilities(VoiceReceivingOnly),
                                                     fromVoiceCapabilities(TextOnly) });
        return all;
    }
} // namespace swift::misc::network

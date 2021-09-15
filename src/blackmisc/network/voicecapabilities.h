/* Copyright (C) 2013
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution. No part of swift project, including this file, may be copied, modified, propagated,
 * or distributed except according to the terms contained in the LICENSE file.
 */

//! \file

#ifndef BLACKMISC_NETWORK_VOICECAPABILITIES_H
#define BLACKMISC_NETWORK_VOICECAPABILITIES_H

#include "blackmisc/blackmiscexport.h"
#include "blackmisc/valueobject.h"

namespace BlackMisc::Network
{
    //! Value object encapsulating information for voice capabilities.
    class BLACKMISC_EXPORT CVoiceCapabilities : public CValueObject<CVoiceCapabilities>
    {
    public:
        //! Voice capabilities
        enum VoiceCapabilities
        {
            Unknown,
            Voice,
            VoiceReceivingOnly,
            TextOnly,
        };

        //! Default constructor.
        CVoiceCapabilities() = default;

        //! Constructor by callsign
        CVoiceCapabilities(VoiceCapabilities capabilities) : m_voiceCapabilities(capabilities) {}

        //! Constructor.
        CVoiceCapabilities(const QString &flightPlanRemarks);

        //! Get capabilities
        VoiceCapabilities getCapabilities() const { return m_voiceCapabilities; }

        //! Set capabilites
        void setCapabilities(VoiceCapabilities capabilites) { m_voiceCapabilities = capabilites; }

        //! Is capability known
        bool isUnknown() const { return m_voiceCapabilities == Unknown; }

        //! To flight plan remarks
        const QString &toFlightPlanRemarks() const;

        //! From enum
        static const CVoiceCapabilities &fromVoiceCapabilities(VoiceCapabilities capabilities);

        //! From flight plan remarks
        static CVoiceCapabilities fromFlightPlanRemarks(const QString &remarks);

        //! From text like "text only"
        static CVoiceCapabilities fromText(const QString &text);

        //! All capabilities as list
        static const QList<CVoiceCapabilities> &allCapabilities();

        //! \copydoc BlackMisc::Mixin::Icon::toIcon()
        CIcons::IconIndex toIcon() const;

        //! \copydoc BlackMisc::Mixin::String::toQString
        QString convertToQString(bool i18n = false) const;

    private:
        VoiceCapabilities m_voiceCapabilities = Unknown;

        //! Capabilites from flight plans remarks such as "/V/"
        void setFromFlightPlanRemarks(const QString &flightPlanRemarks);

        BLACK_METACLASS(
            CVoiceCapabilities,
            BLACK_METAMEMBER(voiceCapabilities)
        );
    };
} // namespace

Q_DECLARE_METATYPE(BlackMisc::Network::CVoiceCapabilities)

#endif // guard

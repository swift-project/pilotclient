// SPDX-FileCopyrightText: Copyright (C) 2013 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

//! \file

#ifndef SWIFT_MISC_NETWORK_VOICECAPABILITIES_H
#define SWIFT_MISC_NETWORK_VOICECAPABILITIES_H

#include "misc/swiftmiscexport.h"
#include "misc/valueobject.h"

BLACK_DECLARE_VALUEOBJECT_MIXINS(swift::misc::network, CVoiceCapabilities)

namespace swift::misc::network
{
    //! Value object encapsulating information for voice capabilities.
    class SWIFT_MISC_EXPORT CVoiceCapabilities : public CValueObject<CVoiceCapabilities>
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

        //! \copydoc swift::misc::mixin::Icon::toIcon()
        CIcons::IconIndex toIcon() const;

        //! \copydoc swift::misc::mixin::String::toQString
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

Q_DECLARE_METATYPE(swift::misc::network::CVoiceCapabilities)

#endif // guard

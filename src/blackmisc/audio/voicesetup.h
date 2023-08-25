// SPDX-FileCopyrightText: Copyright (C) 2018 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

//! \file

#ifndef BLACKMISC_AUDIO_VOICESETUP_H
#define BLACKMISC_AUDIO_VOICESETUP_H

#include "blackmisc/statusmessagelist.h"
#include "blackmisc/settingscache.h"
#include "blackmisc/valueobject.h"
#include "blackmisc/metaclass.h"
#include "blackmisc/propertyindexref.h"
#include "blackmisc/blackmiscexport.h"

#include <QMetaType>
#include <QString>

BLACK_DECLARE_VALUEOBJECT_MIXINS(BlackMisc::Audio, CVoiceSetup)

namespace BlackMisc::Audio
{
    //! Value object for a voice setup
    //! \deprecated
    class BLACKMISC_EXPORT CVoiceSetup : public CValueObject<CVoiceSetup>
    {
    public:
        //! Properties by index
        enum ColumnIndex
        {
            IndexAfvVoiceServerUrl = CPropertyIndexRef::GlobalIndexCVoiceSetup,
            IndexAfvMapUrl
        };

        //! Default constructor.
        CVoiceSetup() = default;

        //! Setup with values
        CVoiceSetup(const QString &afvVoiceServerUrl, const QString &afvMapUrl);

        //! AFV voice server URL
        void setAfvVoiceServerUrl(const QString &serverUrl) { m_afvVoiceServerUrl = serverUrl; }

        //! AFV voice server URL
        const QString &getAfvVoiceServerUrl() const { return m_afvVoiceServerUrl; }

        //! AFV map URL
        void setAfvMapUrl(const QString &mapUrl) { m_afvMapUrl = mapUrl; }

        //! AFV map URL
        const QString &getAfvMapUrl() const { return m_afvMapUrl; }

        //! \copydoc BlackMisc::Mixin::Index::propertyByIndex
        QVariant propertyByIndex(CPropertyIndexRef index) const;

        //! \copydoc BlackMisc::Mixin::Index::setPropertyByIndex
        void setPropertyByIndex(CPropertyIndexRef index, const QVariant &variant);

        //! \copydoc BlackMisc::Mixin::Index::comparePropertyByIndex
        int comparePropertyByIndex(CPropertyIndexRef index, const CVoiceSetup &compareValue) const;

        //! \copydoc BlackMisc::Mixin::String::toQString()
        QString convertToQString(bool i18n = false) const;

        //! Validate
        CStatusMessageList validate() const;

        //! Standard FSD setup for official VATSIM servers
        static const CVoiceSetup &vatsimStandard();

    private:
        QString m_afvVoiceServerUrl = "https://voice1.vatsim.uk";
        QString m_afvMapUrl = "https://afv-map.vatsim.net/";

        BLACK_METACLASS(
            CVoiceSetup,
            BLACK_METAMEMBER(afvVoiceServerUrl),
            BLACK_METAMEMBER(afvMapUrl)
        );
    };

    //! Voice settings
    //! \deprecated Voice VATLIB specifics
    struct TVoiceSetup : public TSettingTrait<CVoiceSetup>
    {
        //! \copydoc BlackMisc::TSettingTrait::key
        static const char *key() { return "audio/%Application%/currentvoicesetup"; }

        //! \copydoc BlackMisc::TSettingTrait::humanReadable
        static const QString &humanReadable()
        {
            static const QString name("Voice setup");
            return name;
        }

        //! \copydoc BlackMisc::TSettingTrait::isValid
        static bool isValid(const CVoiceSetup &setup, QString &) { return setup.validate().isSuccess(); }
    };
} // namespace

Q_DECLARE_METATYPE(BlackMisc::Audio::CVoiceSetup)

#endif // guard

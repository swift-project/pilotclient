// SPDX-FileCopyrightText: Copyright (C) 2018 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

//! \file

#ifndef BLACKMISC_CRASHSETTINGS_H
#define BLACKMISC_CRASHSETTINGS_H

#include "blackmisc/valueobject.h"

BLACK_DECLARE_VALUEOBJECT_MIXINS(BlackMisc::Settings, CCrashSettings)

namespace BlackMisc::Settings
{
    /*!
     * Crash info. Details about crash context.
     */
    class BLACKMISC_EXPORT CCrashSettings : public CValueObject<CCrashSettings>
    {
    public:
        //! Properties by index
        enum ColumnIndex
        {
            IndexEnabled = CPropertyIndexRef::GlobalIndexCCrashSettings,
            IndexPrivateInfo
        };

        //! Default constructor.
        CCrashSettings();

        //! Is enabled?
        bool isEnabled() const { return m_enabled; }

        //! Set enabled
        void setEnabled(bool enabled) { m_enabled = enabled; }

        //! With privacy info
        bool withPrivacyInfo() const { return m_privacyInfo; }

        //! Set privacy info
        void setPrivacyInfo(bool privacy) { m_privacyInfo = privacy; }

        //! \copydoc BlackMisc::Mixin::String::toQString
        QString convertToQString(bool i18n = false) const;

        //! \copydoc BlackMisc::Mixin::Index::propertyByIndex
        QVariant propertyByIndex(CPropertyIndexRef index) const;

        //! \copydoc BlackMisc::Mixin::Index::setPropertyByIndex
        void setPropertyByIndex(CPropertyIndexRef index, const QVariant &variant);

        //! \copydoc BlackMisc::Mixin::Index::comparePropertyByIndex
        int comparePropertyByIndex(CPropertyIndexRef index, const CCrashSettings &compareValue) const;

    private:
        bool m_enabled = false;
        bool m_privacyInfo = false;

        BLACK_METACLASS(
            CCrashSettings,
            BLACK_METAMEMBER(enabled),
            BLACK_METAMEMBER(privacyInfo)
        );
    };
} // ns

Q_DECLARE_METATYPE(BlackMisc::Settings::CCrashSettings)

#endif // guard

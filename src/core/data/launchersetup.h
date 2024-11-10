// SPDX-FileCopyrightText: Copyright (C) 2017 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

//! \file

#ifndef SWIFT_CORE_DATA_LAUNCHERSETUP
#define SWIFT_CORE_DATA_LAUNCHERSETUP

#include "core/swiftcoreexport.h"
#include "misc/propertyindex.h"
#include "misc/datacache.h"
#include "misc/valueobject.h"
#include "misc/variant.h"

SWIFT_DECLARE_VALUEOBJECT_MIXINS(swift::core::data, CLauncherSetup)

namespace swift::core::data
{
    //! Launcher setup
    class SWIFT_CORE_EXPORT CLauncherSetup : public swift::misc::CValueObject<CLauncherSetup>
    {
    public:
        //! Properties by index
        enum ColumnIndex
        {
            IndexDBusAddress = swift::misc::CPropertyIndexRef::GlobalIndexCLauncherSetup,
            IndexFramelessWindow,
            IndexCoreMode,
            IndexAudioMode
        };

        //! Core mode
        enum CoreMode
        {
            Standalone,
            Distributed
        };

        //! Audio setup
        enum AudioModeFlag
        {
            AudioNothingDisabled = 0,
            AudioDisableStandaloneAudio = 1 << 0,
            AudioDisableDistributedCoreAudio = 1 << 1,
            AudioDisableDistributedGuiAudio = 1 << 2
        };
        Q_DECLARE_FLAGS(AudioMode, AudioModeFlag)

        //! Default constructor
        CLauncherSetup() {}

        //! Destructor.
        // virtual ~CLauncherSetup() {}

        //! DBus address
        const QString &getDBusAddress() const { return m_dBusAddress; }

        //! DBus address
        void setDBusAddress(const QString &dBusAddress) { m_dBusAddress = dBusAddress.trimmed(); }

        //! Core mode
        CoreMode getCoreMode() const { return static_cast<CoreMode>(m_coreMode); }

        //! Core mode
        void setCoreMode(CoreMode mode) { m_coreMode = static_cast<int>(mode); }

        //! Audio mode
        AudioMode getAudioMode() const { return static_cast<AudioMode>(m_audioMode); }

        //! Audio mode
        void setAudioMode(AudioMode mode) { m_audioMode = static_cast<int>(mode); }

        //! Frameless window?
        bool useFramelessWindow() const { return m_windowFrameless; }

        //! Frameless window?
        void setFramelessWindow(bool frameless) { m_windowFrameless = frameless; }

        //! \copydoc swift::misc::mixin::String::toQString
        QString convertToQString(bool i18n = false) const;

        //! \copydoc swift::misc::mixin::Index::propertyByIndex
        QVariant propertyByIndex(swift::misc::CPropertyIndexRef index) const;

        //! \copydoc swift::misc::mixin::Index::setPropertyByIndex
        void setPropertyByIndex(swift::misc::CPropertyIndexRef index, const QVariant &variant);

    private:
        QString m_dBusAddress { "tcp:host=127.0.0.1,port=45000" }; //!< DBus address
        bool m_windowFrameless = false; //!< frameless window
        int m_coreMode = static_cast<int>(Standalone); //!< core
        int m_audioMode = static_cast<int>(AudioNothingDisabled);

        SWIFT_METACLASS(
            CLauncherSetup,
            SWIFT_METAMEMBER(dBusAddress),
            SWIFT_METAMEMBER(windowFrameless),
            SWIFT_METAMEMBER(coreMode),
            SWIFT_METAMEMBER(audioMode));
    };

    //! Trait for global setup data
    struct TLauncherSetup : public swift::misc::TDataTrait<CLauncherSetup>
    {
        //! Key in data cache
        static const char *key() { return "swiftlaunchersetup"; }

        //! First load is synchronous
        static constexpr bool isPinned() { return true; }
    };
} // ns

Q_DECLARE_METATYPE(swift::core::data::CLauncherSetup)
Q_DECLARE_METATYPE(swift::core::data::CLauncherSetup::AudioModeFlag)
Q_DECLARE_METATYPE(swift::core::data::CLauncherSetup::AudioMode)
Q_DECLARE_OPERATORS_FOR_FLAGS(swift::core::data::CLauncherSetup::AudioMode)

#endif // guard

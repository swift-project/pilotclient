/* Copyright (C) 2017
 * swift project community / contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution. No part of swift project, including this file, may be copied, modified, propagated,
 * or distributed except according to the terms contained in the LICENSE file.
 */

//! \file

#ifndef BLACKCORE_DATA_LAUNCHERSETUP
#define BLACKCORE_DATA_LAUNCHERSETUP

#include "blackcore/blackcoreexport.h"
#include "blackmisc/propertyindex.h"
#include "blackmisc/datacache.h"
#include "blackmisc/valueobject.h"
#include "blackmisc/variant.h"

BLACK_DECLARE_VALUEOBJECT_MIXINS(BlackCore::Data, CLauncherSetup)

namespace BlackCore::Data
{
    //! Launcher setup
    class BLACKCORE_EXPORT CLauncherSetup : public BlackMisc::CValueObject<CLauncherSetup>
    {
    public:
        //! Properties by index
        enum ColumnIndex
        {
            IndexDBusAddress = BlackMisc::CPropertyIndexRef::GlobalIndexCLauncherSetup,
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
            AudioDisableStandaloneAudio      = 1 << 0,
            AudioDisableDistributedCoreAudio = 1 << 1,
            AudioDisableDistributedGuiAudio  = 1 << 2
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

        //! \copydoc BlackMisc::Mixin::String::toQString
        QString convertToQString(bool i18n = false) const;

        //! \copydoc BlackMisc::Mixin::Index::propertyByIndex
        QVariant propertyByIndex(BlackMisc::CPropertyIndexRef index) const;

        //! \copydoc BlackMisc::Mixin::Index::setPropertyByIndex
        void setPropertyByIndex(BlackMisc::CPropertyIndexRef index, const QVariant &variant);

    private:
        QString m_dBusAddress { "tcp:host=127.0.0.1,port=45000" }; //!< DBus address
        bool    m_windowFrameless = false;                         //!< frameless window
        int     m_coreMode  = static_cast<int>(Standalone);        //!< core
        int     m_audioMode = static_cast<int>(AudioNothingDisabled);

        BLACK_METACLASS(
            CLauncherSetup,
            BLACK_METAMEMBER(dBusAddress),
            BLACK_METAMEMBER(windowFrameless),
            BLACK_METAMEMBER(coreMode),
            BLACK_METAMEMBER(audioMode)
        );
    };

    //! Trait for global setup data
    struct TLauncherSetup : public BlackMisc::TDataTrait<CLauncherSetup>
    {
        //! Key in data cache
        static const char *key() { return "swiftlaunchersetup"; }

        //! First load is synchronous
        static constexpr bool isPinned() { return true; }
    };
} // ns

Q_DECLARE_METATYPE(BlackCore::Data::CLauncherSetup)
Q_DECLARE_METATYPE(BlackCore::Data::CLauncherSetup::AudioModeFlag)
Q_DECLARE_METATYPE(BlackCore::Data::CLauncherSetup::AudioMode)
Q_DECLARE_OPERATORS_FOR_FLAGS(BlackCore::Data::CLauncherSetup::AudioMode)

#endif // guard

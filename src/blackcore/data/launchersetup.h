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

namespace BlackCore
{
    namespace Data
    {
        //! Launcher setup
        class BLACKCORE_EXPORT CLauncherSetup : public BlackMisc::CValueObject<CLauncherSetup>
        {
        public:
            //! Properties by index
            enum ColumnIndex
            {
                IndexDBusAddress = BlackMisc::CPropertyIndex::GlobalIndexCLauncherSetup,
                IndexFramelessWindow,
                IndexCoreMode
            };

            //! Core mode
            enum CoreMode
            {
                Standalone,
                CoreWithAudioOnGui,
                CoreWithAudioOnCore,
            };

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

            //! Frameless window?
            bool useFramelessWindow() const { return m_windowFrameless; }

            //! Frameless window?
            void setFramelessWindow(bool frameless) { m_windowFrameless = frameless; }

            //! \copydoc BlackMisc::Mixin::String::toQString
            QString convertToQString(bool i18n = false) const;

            //! \copydoc BlackMisc::Mixin::Index::propertyByIndex
            BlackMisc::CVariant propertyByIndex(const BlackMisc::CPropertyIndex &index) const;

            //! \copydoc BlackMisc::Mixin::Index::setPropertyByIndex
            void setPropertyByIndex(const BlackMisc::CPropertyIndex &index, const BlackMisc::CVariant &variant);

        private:
            QString m_dBusAddress { "session" }; //!< DBus address
            bool    m_windowFrameless = false;   //!< frameless window
            int     m_coreMode = static_cast<int>(Standalone); //!< core

            BLACK_METACLASS(
                CLauncherSetup,
                BLACK_METAMEMBER(dBusAddress),
                BLACK_METAMEMBER(windowFrameless),
                BLACK_METAMEMBER(coreMode)
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
} // ns

Q_DECLARE_METATYPE(BlackCore::Data::CLauncherSetup)

#endif // guard

/* Copyright (C) 2017
 * Swift Project Community / Contributors
 *
 * This file is part of Swift Project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution. No part of swift project, including this file, may be copied, modified, propagated,
 * or distributed except according to the terms contained in the LICENSE file.
 */

//! \file

#ifndef BLACKMISC_PLATFORM_H
#define BLACKMISC_PLATFORM_H

#include "blackmisc/blackmiscexport.h"
#include "blackmisc/metaclass.h"
#include "blackmisc/propertyindex.h"
#include "blackmisc/valueobject.h"
#include "blackmisc/variant.h"

#include <QMetaEnum>
#include <QString>

namespace BlackMisc
{
    /*!
     * Platform (i.e. a platform swift supports)
     */
    class BLACKMISC_EXPORT CPlatform : public CValueObject<CPlatform>
    {
    public:
        //! Properties by index
        enum ColumnIndex
        {
            IndexPlatform = CPropertyIndex::GlobalIndexCPlatform
        };

        //! OS
        enum PlatformFlag
        {
            UnknownOs   = 0,
            Win32       = 1 << 0,
            Win64       = 1 << 1,
            Linux       = 1 << 2,
            MacOS       = 1 << 3,
            Independent = 1 << 4,
            AllWindows  = Win32 | Win64,
            All         = AllWindows | Linux | MacOS,
            All32       = Win32,
            All64       = Win64 | Linux | MacOS
        };
        Q_DECLARE_FLAGS(Platform, PlatformFlag)

        //! Constructor
        CPlatform() {}

        //! Constructor
        CPlatform(const QString &p);

        //! Constructor
        CPlatform(Platform p);

        //! Platform
        Platform getPlatform() const { return static_cast<Platform>(m_platform); }

        //! Platform flag
        PlatformFlag getPlatformFlag() const;

        //! Matches any other platform
        bool matchesAny(const CPlatform &otherPlatform) const;

        //! Number of supported platforms
        int numberPlatforms() const;

        //! Single platform?
        bool isSinglePlatform() const;

        //! Any Windows
        bool isAnyWindows() const;

        //! Name of platform
        QString getPlatformName() const;

        //! Set platform
        void setPlatform(Platform p) { m_platform = p; }

        //! Unknown?
        bool isUnknown() const { return m_platform == static_cast<int>(UnknownOs); }

        //! Set platform
        void setPlatform(const QString &p) { setPlatform(stringToPlatform(p)); }

        //! Representing icon
        CIcons::IconIndex toIcon() const;

        //! \copydoc BlackMisc::Mixin::String::toQString
        QString convertToQString(bool i18n = false) const;

        //! \copydoc BlackMisc::Mixin::Index::propertyByIndex
        QVariant propertyByIndex(CPropertyIndexRef index) const;

        //! \copydoc BlackMisc::Mixin::Index::setPropertyByIndex
        void setPropertyByIndex(CPropertyIndexRef index, const QVariant &variant);

        //! \copydoc BlackMisc::Mixin::Index::comparePropertyByIndex
        int comparePropertyByIndex(CPropertyIndexRef index, const CPlatform &compareValue) const;

        //! Convert to QString
        operator QString() { return this->toQString(); }

        //! Convert to enum
        static Platform stringToPlatform(const QString &str);

        //! Convert to enum
        const static CPlatform &stringToPlatformObject(const QString &str);

        //! Current platform
        static const CPlatform &currentPlatform();

        //! Is this the current platform
        static bool isCurrentPlatform(const QString &platform);

        //! Is this the current platform
        static bool isCurrentPlatform(const CPlatform &platform);

        //! Can run on this platform
        static bool canRunOnCurrentPlatform(const CPlatform &platform);

        //! Win32
        static const CPlatform &win32Platform();

        //! Win64
        static const CPlatform &win64Platform();

        //! Linux
        static const CPlatform &linuxPlatform();

        //! Mac OS
        static const CPlatform &macOSPlatform();

        //! Unknown OS
        static const CPlatform &unknownOs();

        //! All OS
        static const CPlatform &allOs();

        //! Independent OS
        static const CPlatform &independent();

    private:
        int m_platform = static_cast<int>(UnknownOs); //!< platform

        BLACK_METACLASS(
            CPlatform,
            BLACK_METAMEMBER(platform)
        );
    };
} // namespace

Q_DECLARE_METATYPE(BlackMisc::CPlatform)
Q_DECLARE_METATYPE(BlackMisc::CPlatform::Platform)
Q_DECLARE_METATYPE(BlackMisc::CPlatform::PlatformFlag)
Q_DECLARE_OPERATORS_FOR_FLAGS(BlackMisc::CPlatform::Platform)

#endif // guard

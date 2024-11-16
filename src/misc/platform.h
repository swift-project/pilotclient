// SPDX-FileCopyrightText: Copyright (C) 2017 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

//! \file

#ifndef SWIFT_MISC_PLATFORM_H
#define SWIFT_MISC_PLATFORM_H

#include <QMetaEnum>
#include <QString>

#include "misc/metaclass.h"
#include "misc/propertyindexref.h"
#include "misc/swiftmiscexport.h"
#include "misc/valueobject.h"

SWIFT_DECLARE_VALUEOBJECT_MIXINS(swift::misc, CPlatform)

namespace swift::misc
{
    /*!
     * Platform (i.e. a platform swift supports)
     */
    class SWIFT_MISC_EXPORT CPlatform : public CValueObject<CPlatform>
    {
    public:
        //! Properties by index
        enum ColumnIndex
        {
            IndexPlatform = CPropertyIndexRef::GlobalIndexCPlatform
        };

        //! OS
        enum PlatformFlag
        {
            UnknownOs = 0,
            Win32 = 1 << 0,
            Win64 = 1 << 1,
            Linux = 1 << 2,
            MacOS = 1 << 3,
            Independent = 1 << 4,
            AllWindows = Win32 | Win64,
            All = AllWindows | Linux | MacOS,
            All32 = Win32,
            All64 = Win64 | Linux | MacOS
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

        //! \copydoc swift::misc::mixin::String::toQString
        QString convertToQString(bool i18n = false) const;

        //! \copydoc swift::misc::mixin::Index::propertyByIndex
        QVariant propertyByIndex(CPropertyIndexRef index) const;

        //! \copydoc swift::misc::mixin::Index::setPropertyByIndex
        void setPropertyByIndex(CPropertyIndexRef index, const QVariant &variant);

        //! \copydoc swift::misc::mixin::Index::comparePropertyByIndex
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

        SWIFT_METACLASS(
            CPlatform,
            SWIFT_METAMEMBER(platform));
    };
} // namespace swift::misc

Q_DECLARE_METATYPE(swift::misc::CPlatform)
Q_DECLARE_METATYPE(swift::misc::CPlatform::Platform)
Q_DECLARE_METATYPE(swift::misc::CPlatform::PlatformFlag)
Q_DECLARE_OPERATORS_FOR_FLAGS(swift::misc::CPlatform::Platform)

#endif // guard

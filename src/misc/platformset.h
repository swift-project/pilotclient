// SPDX-FileCopyrightText: Copyright (C) 2017 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

//! \file

#ifndef SWIFT_MISC_PLATFORMSET_H
#define SWIFT_MISC_PLATFORMSET_H

#include <tuple>

#include <QMetaType>

#include "misc/collection.h"
#include "misc/platform.h"
#include "misc/sequence.h"
#include "misc/swiftmiscexport.h"

SWIFT_DECLARE_COLLECTION_MIXINS(swift::misc, CPlatform, CPlatformSet)

namespace swift::misc
{
    //! Value object for a set of platforms.
    class SWIFT_MISC_EXPORT CPlatformSet :
        public CCollection<CPlatform>,
        public mixin::MetaType<CPlatformSet>
    {
        using CCollection::CCollection;
        SWIFT_MISC_DECLARE_USING_MIXIN_METATYPE(CPlatformSet)

    public:
        //! Default constructor.
        CPlatformSet();

        //! Construct from a base class object.
        CPlatformSet(const CCollection<CPlatform> &other);

        //! All platform names
        QStringList getPlatformNames() const;

        //! Matches any given platform
        CPlatformSet matchesAny(CPlatform::Platform platform) const;

        //! Matches given platform
        CPlatformSet exactMatch(CPlatform::Platform platform) const;

        //! Contains name?
        bool containsName(const QString &platformName) const;

        //! All platforms
        static const CPlatformSet &allPlatforms();

        //! Register metadata
        static void registerMetadata();
    };
} // namespace swift::misc

Q_DECLARE_METATYPE(swift::misc::CPlatformSet)
Q_DECLARE_METATYPE(swift::misc::CCollection<swift::misc::CPlatform>)

#endif // guard

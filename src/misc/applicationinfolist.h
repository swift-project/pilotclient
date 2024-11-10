// SPDX-FileCopyrightText: Copyright (C) 2016 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

//! \file

#ifndef SWIFT_MISC_APPLICATIONINFOLIST_H
#define SWIFT_MISC_APPLICATIONINFOLIST_H

#include "misc/applicationinfo.h"
#include "misc/sequence.h"
#include "misc/collection.h"

BLACK_DECLARE_SEQUENCE_MIXINS(swift::misc, CApplicationInfo, CApplicationInfoList)

namespace swift::misc
{
    /*!
     * List of swift application descriptions.
     */
    class SWIFT_MISC_EXPORT CApplicationInfoList :
        public CSequence<CApplicationInfo>,
        public mixin::MetaType<CApplicationInfoList>
    {
    public:
        SWIFT_MISC_DECLARE_USING_MIXIN_METATYPE(CApplicationInfoList)
        using CSequence::CSequence;

        //! Default constructor.
        CApplicationInfoList();

        //! Construct from base class object.
        CApplicationInfoList(const CSequence<CApplicationInfo> &other);

        //! List containing entry for CApplicationInfo::Application ?
        bool containsApplication(CApplicationInfo::Application application) const;

        //! Remove given application
        int removeApplication(CApplicationInfo::Application application);

        //! Running application names
        QStringList processNames() const;

        //! Fill from cache data directories
        int otherSwiftVersionsFromDataDirectories(bool reinit = false);

        //! Filled from cache data directories
        static CApplicationInfoList fromOtherSwiftVersionsFromDataDirectories(bool reinit = false);

        //! swift application data sub directories with info if available
        static const QMap<QString, CApplicationInfo> &applicationDataDirectoryMapWithoutCurrentVersion();

        //! swift application data sub directories with info if available
        static QMap<QString, CApplicationInfo> currentApplicationDataDirectoryMapWithoutCurrentVersion();

        //! Other swift data directories
        static bool hasOtherSwiftDataDirectories();
    };
} // ns

Q_DECLARE_METATYPE(swift::misc::CApplicationInfoList)
Q_DECLARE_METATYPE(swift::misc::CCollection<swift::misc::CApplicationInfo>)

#endif

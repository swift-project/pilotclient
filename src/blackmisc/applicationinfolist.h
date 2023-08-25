// SPDX-FileCopyrightText: Copyright (C) 2016 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

//! \file

#ifndef BLACKMISC_APPLICATIONINFOLIST_H
#define BLACKMISC_APPLICATIONINFOLIST_H

#include "blackmisc/applicationinfo.h"
#include "blackmisc/sequence.h"
#include "blackmisc/collection.h"

BLACK_DECLARE_SEQUENCE_MIXINS(BlackMisc, CApplicationInfo, CApplicationInfoList)

namespace BlackMisc
{
    /*!
     * List of swift application descriptions.
     */
    class BLACKMISC_EXPORT CApplicationInfoList :
        public CSequence<CApplicationInfo>,
        public Mixin::MetaType<CApplicationInfoList>
    {
    public:
        BLACKMISC_DECLARE_USING_MIXIN_METATYPE(CApplicationInfoList)
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

Q_DECLARE_METATYPE(BlackMisc::CApplicationInfoList)
Q_DECLARE_METATYPE(BlackMisc::CCollection<BlackMisc::CApplicationInfo>)

#endif

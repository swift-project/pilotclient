/* Copyright (C) 2016
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution. No part of swift project, including this file, may be copied, modified, propagated,
 * or distributed except according to the terms contained in the LICENSE file.
 */

//! \file

#ifndef BLACKMISC_APPLICATIONINFOLIST_H
#define BLACKMISC_APPLICATIONINFOLIST_H

#include "blackmisc/applicationinfo.h"
#include "blackmisc/sequence.h"
#include "blackmisc/collection.h"

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
    };
} // ns

Q_DECLARE_METATYPE(BlackMisc::CApplicationInfoList)
Q_DECLARE_METATYPE(BlackMisc::CCollection<BlackMisc::CApplicationInfo>)

#endif

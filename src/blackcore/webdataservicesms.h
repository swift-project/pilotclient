/* Copyright (C) 2019
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution. No part of swift project, including this file, may be copied, modified, propagated,
 * or distributed except according to the terms contained in the LICENSE file.
 */

//! \file

#ifndef BLACKCORE_WEBDATASERVICES_MATCHINGSCRIPT_H
#define BLACKCORE_WEBDATASERVICES_MATCHINGSCRIPT_H

#include "blackcore/blackcoreexport.h"

#include <QObject>

namespace BlackCore
{
    /*!
     * Encapsulates reading data from web sources
     */

    //! Access to database
    class BLACKCORE_EXPORT MSWebServices : public QObject
    {
        Q_OBJECT

    public:
        //! Ctor
        Q_INVOKABLE MSWebServices() {}

        //! @{
        //! Functions calling the web services
        Q_INVOKABLE int countAircraftIcaoCodesForDesignator(const QString &designator) const;
        Q_INVOKABLE int countAirlineIcaoCodesForDesignator(const QString &designator) const;
        //! @}

    private:
        //! sApp available?
        static bool checkApp();
    };
} // namespace

#endif

// SPDX-FileCopyrightText: Copyright (C) 2019 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

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

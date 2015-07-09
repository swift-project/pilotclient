/* Copyright (C) 2015
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

#ifndef BLACKCORE_DATABASE_READER_H
#define BLACKCORE_DATABASE_READER_H

//! \file

#include "blackcore/blackcoreexport.h"
#include "blackmisc/threadedreader.h"
#include <QNetworkReply>
#include <QJsonArray>

namespace BlackCore
{
    //! Support for threaded based reading and parsing tasks such
    //! as data files via http, or file system and parsing (such as FSX models)
    class BLACKCORE_EXPORT CDatabaseReader : public BlackMisc::CThreadedReader
    {
        Q_OBJECT

    public:
        //! Start reading in own thread
        void readInBackgroundThread();

    protected:
        //! Constructor
        CDatabaseReader(QObject *owner, const QString &name);

        //! Check if terminated or error, otherwise split into array of objects
        QJsonArray transformReplyIntoJsonArray(QNetworkReply *nwReply) const;

        //! Build service URL
        static QString buildUrl(const QString &protocol, const QString &server, const QString &baseUrl, const QString &serviceUrl);

    };
} // namespace

#endif // guard

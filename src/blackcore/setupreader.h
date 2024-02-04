// SPDX-FileCopyrightText: Copyright (C) 2015 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

//! \file

#ifndef BLACKCORE_SETUPREADER_H
#define BLACKCORE_SETUPREADER_H

#include "blackcore/blackcoreexport.h"
#include "blackcore/data/globalsetup.h"
#include "blackmisc/db/updateinfo.h"
#include "blackmisc/datacache.h"
#include "blackmisc/network/urllist.h"
#include "blackmisc/statusmessagelist.h"

#include <QCommandLineOption>
#include <QCoreApplication>
#include <QJsonObject>
#include <QNetworkReply>
#include <QList>
#include <QObject>
#include <QString>
#include <atomic>

namespace BlackCoreTest
{
    class CTestConnectivity;
}
namespace BlackCore
{
    //! Read the central URLs / locations of our data, setup and versions.
    //!
    //! \details This class should be only used in BlackCore::CApplication.
    //!
    //! \note This class is no(!) BlackCore::CThreadedReader as it will be loaded once during startup
    //!       and reading setup data is fast. The read file is also called "bootstrap" file as it tells
    //!       swift which data and versions are located where. Without that file we cannot start.
    //!
    //! \sa BlackMisc::Db::TUpdateInfo
    class BLACKCORE_EXPORT CSetupReader : public QObject
    {
        Q_OBJECT
        friend class BlackCoreTest::CTestConnectivity;

    public:
        //! Categories
        static const QStringList &getLogCategories();

        //! Constructor
        explicit CSetupReader(QObject *parent);

        //! Load the setup. If the setup is already loaded, the setup is reloaded
        BlackMisc::CStatusMessageList loadSetup();

        //! Setup available?
        //! \threadsafe
        bool isSetupAvailable() const { return m_setupAvailable; }

        //! Current setup (reader URLs, DB location, crash server)
        //! \remarks aka "bootstrap file"
        //! \threadsafe
        Data::CGlobalSetup getSetup() const;

    private:
        std::atomic<bool> m_setupAvailable { false }; //!< setup available?
        Data::CGlobalSetup m_setup {}; //!< data setup

        //! Read by local file
        BlackMisc::CStatusMessageList readLocalBootstrapFile();
    };
} // ns

#endif // guard

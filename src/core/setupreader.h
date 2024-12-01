// SPDX-FileCopyrightText: Copyright (C) 2015 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

//! \file

#ifndef SWIFT_CORE_SETUPREADER_H
#define SWIFT_CORE_SETUPREADER_H

#include <atomic>

#include <QObject>
#include <QString>

#include "core/data/globalsetup.h"
#include "core/swiftcoreexport.h"
#include "misc/datacache.h"
#include "misc/db/updateinfo.h"
#include "misc/statusmessagelist.h"

namespace SwiftCoreTest
{
    class CTestConnectivity;
}
namespace swift::core
{
    //! Read the central URLs / locations of our data, setup and versions.
    //!
    //! \details This class should be only used in swift::core::CApplication.
    //!
    //! \note This class is no(!) swift::core::CThreadedReader as it will be loaded once during startup
    //!       and reading setup data is fast. The read file is also called "bootstrap" file as it tells
    //!       swift which data and versions are located where. Without that file we cannot start.
    //!
    //! \sa swift::misc::db::TUpdateInfo
    class SWIFT_CORE_EXPORT CSetupReader : public QObject
    {
        Q_OBJECT
        friend class SwiftCoreTest::CTestConnectivity;

    public:
        //! Categories
        static const QStringList &getLogCategories();

        //! Constructor
        explicit CSetupReader(QObject *parent);

        //! Load the setup. If the setup is already loaded, the setup is reloaded
        swift::misc::CStatusMessageList loadSetup();

        //! Setup available?
        //! \threadsafe
        bool isSetupAvailable() const { return m_setupAvailable; }

        //! Current setup (reader URLs, DB location, crash server)
        //! \remarks aka "bootstrap file"
        //! \threadsafe
        data::CGlobalSetup getSetup() const;

    private:
        std::atomic<bool> m_setupAvailable { false }; //!< setup available?
        data::CGlobalSetup m_setup {}; //!< data setup

        //! Read by local file
        swift::misc::CStatusMessageList readLocalBootstrapFile();
    };
} // namespace swift::core

#endif // SWIFT_CORE_SETUPREADER_H

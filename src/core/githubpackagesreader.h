// SPDX-FileCopyrightText: Copyright (C) 2020 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

//! \file

#ifndef SWIFT_CORE_GITHUBPACKAGESREADER_H
#define SWIFT_CORE_GITHUBPACKAGESREADER_H

#include "misc/db/updateinfo.h"
#include <QObject>

namespace swift::core
{
    /*!
     * Read available updates from GitHub Packages REST API.
     */
    class CGitHubPackagesReader : public QObject
    {
        Q_OBJECT

    public:
        //! Constructor.
        CGitHubPackagesReader(QObject *parent = nullptr);

        //! Read updates from GitHub Packages.
        void readUpdateInfo();

        //! Get updates cached from previous read.
        swift::misc::db::CUpdateInfo getUpdateInfo() const;

    signals:
        //! Updates have been received from GitHub Packages.
        void updateInfoAvailable(bool available);

    private:
        swift::misc::CData<swift::misc::db::TUpdateInfo> m_updateInfo { this };
    };
}

#endif

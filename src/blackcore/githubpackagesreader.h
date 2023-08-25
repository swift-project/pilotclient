// SPDX-FileCopyrightText: Copyright (C) 2020 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

//! \file

#ifndef BLACKCORE_GITHUBPACKAGESREADER_H
#define BLACKCORE_GITHUBPACKAGESREADER_H

#include "blackmisc/db/updateinfo.h"
#include <QObject>

namespace BlackCore
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
        BlackMisc::Db::CUpdateInfo getUpdateInfo() const;

    signals:
        //! Updates have been received from GitHub Packages.
        void updateInfoAvailable(bool available);

    private:
        BlackMisc::CData<BlackMisc::Db::TUpdateInfo> m_updateInfo { this };
    };
}

#endif

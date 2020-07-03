/* Copyright (C) 2020
 * swift project community / contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution. No part of swift project, including this file, may be copied, modified, propagated,
 * or distributed except according to the terms contained in the LICENSE file.
 */

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

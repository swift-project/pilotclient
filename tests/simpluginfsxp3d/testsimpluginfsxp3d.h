// SPDX-FileCopyrightText: Copyright (C) 2018 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

#ifndef SWIFT_SIMPLUGINFSXP3D_TESTFSXP3DCOMMON_H
#define SWIFT_SIMPLUGINFSXP3D_TESTFSXP3DCOMMON_H

//! \cond PRIVATE_TESTS
//! \file
//! \ingroup testsimplugin

#include <QObject>

#include "test.h"

namespace SwiftSimPluginFsxP3D
{
    //! FSX/P3D common tests
    class CSimPluginFsxP3d : public QObject
    {
        Q_OBJECT

    public:
        //! Standard test case constructor
        explicit CSimPluginFsxP3d(QObject *parent = nullptr) : QObject(parent) {}

    private slots:
        //! Resolve SimConnect Symbols
        void resolveSymbols();

        //! Request IDs
        void requestIds();
    };
} // namespace SwiftSimPluginFsxP3D

//! \endcond

#endif // guard

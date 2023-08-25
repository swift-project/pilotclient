// SPDX-FileCopyrightText: Copyright (C) 2018 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

//! \file

#ifndef BLACKGUI_COPYXSWIFTBUSDIALOG_H
#define BLACKGUI_COPYXSWIFTBUSDIALOG_H

#include <QWidget>
#include "blackgui/blackguiexport.h"

namespace BlackGui
{
    //! Show dialog about to copy XSwiftBus build files
    class BLACKGUI_EXPORT CCopyXSwiftBusDialog
    {
    public:
        //! Display a dialog to copy the latest XSwiftBus files
        //! \remark normally only displayed in a local environment
        static int displayDialogAndCopyBuildFiles(const QString &xplaneRootDir, bool checkLatestFile = true, QWidget *parent = nullptr);
    };
} // ns
#endif

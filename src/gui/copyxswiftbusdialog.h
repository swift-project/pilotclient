// SPDX-FileCopyrightText: Copyright (C) 2018 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

//! \file

#ifndef SWIFT_GUI_COPYXSWIFTBUSDIALOG_H
#define SWIFT_GUI_COPYXSWIFTBUSDIALOG_H

#include <QWidget>

#include "gui/swiftguiexport.h"

namespace swift::gui
{
    //! Show dialog about to copy xswiftbus build files
    class SWIFT_GUI_EXPORT CCopyXSwiftBusDialog
    {
    public:
        //! Display a dialog to copy the latest xswiftbus files
        //! \remark normally only displayed in a local environment
        static int displayDialogAndCopyBuildFiles(const QString &xplaneRootDir, bool checkLatestFile = true,
                                                  QWidget *parent = nullptr);
    };
} // namespace swift::gui
#endif

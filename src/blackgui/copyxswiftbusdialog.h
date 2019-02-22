/* Copyright (C) 2018
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution. No part of swift project, including this file, may be copied, modified, propagated,
 * or distributed except according to the terms contained in the LICENSE file.
 *
 * Class based on qLed: Copyright (C) 2010 by P. Sereno, http://www.sereno-online.com
 */

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

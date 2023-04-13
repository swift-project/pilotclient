/* Copyright (C) 2017
 * swift Project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution. No part of swift project, including this file, may be copied, modified, propagated,
 * or distributed except according to the terms contained in the LICENSE file.
 */

//! \file

/*                                         ---------
 *                                        ! WARNING !
 *      ---------------------------------------------------------------------------------
 * >>>> CHANGES IN ANY HEADERS INCLUDED HEREIN WILL TRIGGER A FULL REBUILD OF EVERYTHING! <<<<
 *      ---------------------------------------------------------------------------------
 */

#ifdef BLACKGUI_PCH_H
#    error "Don't #include this file"
#else
#    define BLACKGUI_PCH_H

#    include "blackcore/pch/pch.h"

#    ifdef __cplusplus

#        include "blackcore/context/contextapplication.h"
#        include "blackcore/context/contextaudio.h"
#        include "blackcore/context/contextnetwork.h"
#        include "blackcore/context/contextownaircraft.h"
#        include "blackcore/context/contextsimulator.h"

#        include <QAbstractButton>
#        include <QAbstractItemModel>
#        include <QAbstractItemView>
#        include <QAbstractScrollArea>
#        include <QAbstractTableModel>
#        include <QComboBox>
#        include <QDialog>
#        include <QDockWidget>
#        include <QDrag>
#        include <QDragEnterEvent>
#        include <QDragLeaveEvent>
#        include <QDragMoveEvent>
#        include <QDropEvent>
#        include <QFrame>
#        include <QImage>
#        include <QLabel>
#        include <QLayout>
#        include <QLayoutItem>
#        include <QLineEdit>
#        include <QMenu>
#        include <QMenuBar>
#        include <QMessageBox>
#        include <QPoint>
#        include <QPushButton>
#        include <QRadioButton>
#        include <QRect>
#        include <QSize>
#        include <QSlider>
#        include <QSplitter>
#        include <QStackedWidget>
#        include <QStyle>
#        include <QTabBar>
#        include <QTabWidget>
#        include <QTableView>
#        include <QTextEdit>
#        include <QValidator>
#        include <QWidget>
#        include <QWindow>

#    endif // __cplusplus
#endif // guard

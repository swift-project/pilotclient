/* Copyright (C) 2014
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

//! \file

#ifndef BLACKGUI_GUIUTILITY_H
#define BLACKGUI_GUIUTILITY_H

#include "blackgui/blackguiexport.h"
#include "blackmisc/icon.h"
#include "enableforframelesswindow.h"
#include <QWidgetList>
#include <QComboBox>
#include <QCheckBox>
#include <QCommandLineParser>

namespace BlackGui
{
    //! GUI utilities
    class BLACKGUI_EXPORT CGuiUtility
    {
    public:
        //! Main (frameless enabled) application window
        static CEnableForFramelessWindow *mainFramelessEnabledApplicationWindow();

        //! Main application window widget
        static QWidget *mainApplicationWindow();

        //! Is main window frameless?
        static bool isMainWindowFrameless();

        //! Top level widgets with names
        static QWidgetList topLevelApplicationWidgetsWithName();

        //! Position of main window
        static QPoint mainWindowPosition();

        //! Replace count in name such as "stations (4)"
        static QString replaceTabCountValue(const QString &oldName, int count);

        //! Delete hierarchy of layouts
        static void deleteLayout(QLayout *layout, bool deleteWidgets);

        //! Leninet / relaxed
        static bool lenientTitleComparison(const QString &title, const QString &comparison);

        //! Find best match in comboBox
        static bool setComboBoxValueByStartingString(QComboBox *box, const QString &candidate, const QString &unspecified = QString());

        //! Mime data with swift type
        static bool hasSwiftVariantMimeType(const QMimeData *mime);

        //! From text dropped
        static BlackMisc::CVariant fromSwiftDragAndDropData(const QMimeData *mime);

        //! From text dropped
        static BlackMisc::CVariant fromSwiftDragAndDropData(const QByteArray &utf8Data);

        //! Meta type id from dropped data
        static int metaTypeIdFromSwiftDragAndDropData(const QMimeData *mime);

        //! Metatype
        static const QString &swiftJsonDragAndDropMimeType();

        //! Pseudo readonly state for checkbox
        static void checkBoxReadOnly(QCheckBox *checkBox, bool readOnly);

        //! Toogle window flags / stay on top
        static bool toggleStayOnTop(QWidget *widget);

        //! Window on top?
        static bool staysOnTop(QWidget *widget);

    private:
        //! Constructor, use static methods only
        CGuiUtility() {}
    };
} // ns

#endif // guard

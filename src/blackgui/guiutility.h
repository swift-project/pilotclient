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
#include "blackmisc/variant.h"

#include <QByteArray>
#include <QPoint>
#include <QString>
#include <QWidget>
#include <QWidgetList>
#include <QModelIndexList>

class QCheckBox;
class QComboBox;
class QLayout;
class QMimeData;
class QTabWidget;
class QGraphicsOpacityEffect;

namespace BlackGui
{
    class CEnableForFramelessWindow;
    class COverlayMessagesFrame;

    //! GUI utilities
    class BLACKGUI_EXPORT CGuiUtility
    {
    public:
        //! Main (frameless enabled) application window
        static CEnableForFramelessWindow *mainFramelessEnabledApplicationWindow();

        //! Register main application window widget if this is known
        static void registerMainApplicationWindow(QWidget *mainWindow);

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

        //! Lenient / relaxed
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

        //! Find next BlackGui::COverlayMessages QFrame
        static COverlayMessagesFrame *nextOverlayMessageFrame(QWidget *widget, int maxLevels = 10);

        //! Metatype
        static const QString &swiftJsonDragAndDropMimeType();

        //! Pseudo readonly state for checkbox
        static void checkBoxReadOnly(QCheckBox *checkBox, bool readOnly);

        //! Pseudo readonly state for checkboxes of widget
        static void checkBoxesReadOnly(QWidget *parent, bool readOnly);

        //! Enable/disable all child widgets
        template <class WIDGET>
        static void childrenSetEnabled(QWidget *parent, bool enabled)
        {
            if (!parent) { return; }
            QList<WIDGET *> children = parent->findChildren<WIDGET *>();
            for (WIDGET *w : children)
            {
                w->setEnabled(enabled);
            }
        }

        //! Toogle window flags / stay on top
        static bool toggleStayOnTop(QWidget *widget);

        //! Window on top?
        static bool staysOnTop(QWidget *widget);

        //! From a given widget try to find parent tab widget (where widget is embedded)
        static QTabWidget *parentTabWidget(QWidget *widget, int maxLevels = 5);

        //! Convert to string
        static QString marginsToString(const QMargins &margins);

        //! Convert from string
        static QMargins stringToMargins(const QString &str);

        //! Only the row part and unique (so no rows is twice in the list)
        static QList<int> indexToUniqueRows(const QModelIndexList &indexes);

        //! Is top level widget?
        static bool isTopLevelWidget(QWidget *widget);

        //! Fade in a widget
        static QGraphicsOpacityEffect *fadeInWidget(int durationMs, QWidget *widget, double startValue = 0.0, double endValue = 1.0);

        //! Fade out a widget
        static QGraphicsOpacityEffect *fadeOutWidget(int durationMs, QWidget *widget, double startValue = 1.0, double endValue = 0.0);

    private:
        //! Constructor, use static methods only
        CGuiUtility() {}

        static QWidget *s_mainApplicationWindow;
    };
} // ns

#endif // guard

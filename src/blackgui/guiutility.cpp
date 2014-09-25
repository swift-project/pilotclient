/* Copyright (C) 2014
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

#include "guiutility.h"
#include <QWidget>
#include <QApplication>

namespace BlackGui
{

    QMainWindow *CGuiUtility::mainWindow()
    {
        QWidgetList tlw = topLevelApplicationWidgetsWithName();
        foreach(QWidget * w, tlw)
        {
            QMainWindow *mw = qobject_cast<QMainWindow *>(w);
            if (!mw) { continue; }
            QString n = mw->objectName().toLower();
            if (n.contains("main") && n.contains("window")) return mw;
        }
        return nullptr;
    }

    QWidgetList CGuiUtility::topLevelApplicationWidgetsWithName()
    {
        QWidgetList tlw = QApplication::topLevelWidgets();
        QWidgetList rl;
        foreach(QWidget * w, tlw)
        {
            if (w->objectName().isEmpty()) continue;
            rl.append(w);
        }
        return rl;
    }

    QPoint CGuiUtility::mainWindowPosition()
    {
        QMainWindow *mw = mainWindow();
        return (mw) ? mw->pos() : QPoint();
    }

    QPoint CGuiUtility::introWindowPosition()
    {
        QWidgetList tlw = topLevelApplicationWidgetsWithName();
        foreach(QWidget * w, tlw)
        {
            QString n = w->objectName().toLower();
            if (n.contains("intro")) return w->pos();
        }
        return QPoint(0, 0);
    }

    QPoint CGuiUtility::assumedMainWindowPosition()
    {
        QPoint p = mainWindowPosition();
        return (p.isNull()) ? introWindowPosition() : p;
    }

    QString CGuiUtility::replaceTabCountValue(const QString &oldName, int count)
    {
        const QString v = QString("(").append(QString::number(count)).append(")");
        if (oldName.isEmpty()) {return v; }
        int index = oldName.lastIndexOf('(');
        if (index == 0) { return v; }
        if (index < 0) { return QString(oldName).append(" ").append(v); }
        return QString(oldName.left(index)).append(" ").append(v);
    }
}

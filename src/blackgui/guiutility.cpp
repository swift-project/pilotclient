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

    CEnableForFramelessWindow *CGuiUtility::mainApplicationWindow()
    {
        QWidgetList tlw = topLevelApplicationWidgetsWithName();
        foreach(QWidget * w, tlw)
        {
            CEnableForFramelessWindow *mw = dynamic_cast<CEnableForFramelessWindow *>(w);
            if (!mw) { continue; }
            if (mw->isMainApplicationWindow()) return mw;
        }
        return nullptr;
    }

    bool CGuiUtility::isMainWindowFrameless()
    {
        CEnableForFramelessWindow *mw = mainApplicationWindow();
        Q_ASSERT(mw); // there should be a main window
        return (mw && mw->isFrameless());
    }

    QWidgetList CGuiUtility::topLevelApplicationWidgetsWithName()
    {
        QWidgetList tlw = QApplication::topLevelWidgets();
        QWidgetList rl;
        foreach(QWidget * w, tlw)
        {
            if (w->objectName().isEmpty()) { continue; }
            rl.append(w);
        }
        return rl;
    }

    QPoint CGuiUtility::mainWindowPosition()
    {
        CEnableForFramelessWindow *mw = mainApplicationWindow();
        return (mw) ? mw->getWidget()->pos() : QPoint();
    }

    QPoint CGuiUtility::introWindowPosition()
    {
        QWidgetList tlw = topLevelApplicationWidgetsWithName();
        foreach(QWidget * w, tlw)
        {
            QString n = w->objectName().toLower();
            if (n.contains("intro")) { return w->pos(); }
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
        if (oldName.isEmpty()) { return v; }
        int index = oldName.lastIndexOf('(');
        if (index == 0) { return v; }
        if (index < 0) { return QString(oldName).append(" ").append(v); }
        return QString(oldName.left(index)).append(" ").append(v);
    }
}

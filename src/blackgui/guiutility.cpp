/* Copyright (C) 2014
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

#include "guiutility.h"
#include "blackcore/context_runtime.h"
#include "blackmisc/filelogger.h"
#include "blackmisc/logmessage.h"
#include "blackmisc/project.h"
#include <QWidget>
#include <QApplication>
#include <QGuiApplication>
#include <QMessageBox>

using namespace BlackCore;
using namespace BlackMisc;

namespace BlackGui
{
    CEnableForFramelessWindow *CGuiUtility::mainApplicationWindow()
    {
        QWidgetList tlw = topLevelApplicationWidgetsWithName();
        foreach(QWidget * w, tlw)
        {
            CEnableForFramelessWindow *mw = dynamic_cast<CEnableForFramelessWindow *>(w);
            if (!mw) { continue; }
            if (mw->isMainApplicationWindow()) { return mw; }
        }
        return nullptr;
    }

    QWidget *CGuiUtility::mainApplicationWindowWidget()
    {
        CEnableForFramelessWindow *mw = mainApplicationWindow();
        return mw ? mw->getWidget() : nullptr;
    }

    bool CGuiUtility::isMainWindowFrameless()
    {
        CEnableForFramelessWindow *mw = mainApplicationWindow();
        Q_ASSERT(mw); // there should be a main window
        return (mw && mw->isFrameless());
    }

    void CGuiUtility::initSwiftGuiApplication(QApplication &a, const QString &applicationName, const QPixmap &icon)
    {
        CRuntime::registerMetadata(); // register metadata
        CLogHandler::instance()->install(); // make sure we have a log handler!

        QApplication::setApplicationName(applicationName);
        QApplication::setApplicationVersion(CProject::version());
        QApplication::setWindowIcon(icon);

        // Logging
        QString category("swift." + applicationName);

        // Translations
        QFile file(":blackmisc/translations/blackmisc_i18n_de.qm");
        CLogMessage(category).debug() << (file.exists() ? "Found translations in resources" : "No translations in resources");
        QTranslator translator;
        if (translator.load("blackmisc_i18n_de", ":blackmisc/translations/"))
        {
            CLogMessage(category).debug() << "Translator loaded";
        }

        // File logger
        CFileLogger fileLogger(applicationName, QString(), &a);
        fileLogger.changeLogPattern(CLogPattern().withSeverityAtOrAbove(CStatusMessage::SeverityDebug));

        // GUI icon
        a.installTranslator(&translator);
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
        const QString v = QString(" (").append(QString::number(count)).append(")");
        if (oldName.isEmpty()) { return v; }
        int index = oldName.lastIndexOf('(');
        if (index == 0) { return v; }
        if (index < 0) { return QString(oldName).trimmed().append(v); }
        return QString(oldName.left(index)).trimmed().append(v);
    }

    void CGuiUtility::deleteLayout(QLayout *layout, bool deleteWidgets)
    {
        // http://stackoverflow.com/a/7569928/356726
        if (!layout) { return; }
        QLayoutItem *item {nullptr};
        while ((item = layout->takeAt(0)))
        {
            QLayout *sublayout {nullptr};
            QWidget *widget {nullptr};
            if ((sublayout = item->layout()))
            {
                deleteLayout(sublayout, deleteWidgets);
            }
            else if ((widget = item->widget()))
            {
                widget->hide();
                if (deleteWidgets)
                {
                    delete widget;
                }
            }
            else {delete item;}
        }

        // then finally
        delete layout;
    }

    void CGuiUtility::commandLineErrorMessage(const QString &errorMessage, const QCommandLineParser &parser)
    {
#   ifdef Q_OS_WIN
        QMessageBox::warning(0, QGuiApplication::applicationDisplayName(), "<html><head/><body><h2>" + errorMessage + "</h2><pre>" + parser.helpText() + "</pre></body></html>");
#   else
        fputs(qPrintable(errorMessage), stderr);
        fputs("\n\n", stderr);
        fputs(qPrintable(parser.helpText()), stderr);
#   endif
    }

    void CGuiUtility::commandLineVersionRequested()
    {
#   ifdef Q_OS_WIN
        QMessageBox::information(0, QGuiApplication::applicationDisplayName(), QGuiApplication::applicationDisplayName() + ' ' + QCoreApplication::applicationVersion());
#   else
        printf("%s %s\n", qPrintable(QCoreApplication::applicationName()), qPrintable(QCoreApplication::applicationVersion()));
#   endif
    }

    void CGuiUtility::commandLineHelpRequested(QCommandLineParser &parser)
    {
#   ifdef Q_OS_WIN
        QMessageBox::warning(0, QGuiApplication::applicationDisplayName(), "<html><head/><body><pre>" + parser.helpText() + "</pre></body></html>");
#   else
        parser.showHelp(); // terminates
        Q_UNREACHABLE();
#   endif
    }
} // ns

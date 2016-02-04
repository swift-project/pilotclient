/* Copyright (C) 2014
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

#include "guiutility.h"
#include "blackcore/contextruntime.h"
#include "blackcore/cookiemanager.h"
#include "blackmisc/filelogger.h"
#include "blackmisc/logmessage.h"
#include "blackmisc/project.h"
#include "blackmisc/verify.h"
#include <QMainWindow>
#include <QApplication>
#include <QGuiApplication>
#include <QMessageBox>
#include <QRegularExpression>
#include <QThreadStorage>
#include <QMimeData>
#include <QJsonDocument>
#include <QJsonObject>
#include <QStandardPaths>

using namespace BlackCore;
using namespace BlackMisc;

namespace BlackGui
{
    CEnableForFramelessWindow *CGuiUtility::mainFramelessEnabledApplicationWindow()
    {
        QWidgetList tlw = topLevelApplicationWidgetsWithName();
        for (QWidget *w : tlw)
        {
            // best choice is to check on frameless window
            CEnableForFramelessWindow *mw = dynamic_cast<CEnableForFramelessWindow *>(w);
            if (mw && mw->isMainApplicationWindow()) { return mw; }
        }
        return nullptr;
    }

    QWidget *CGuiUtility::mainApplicationWindowWidget()
    {
        CEnableForFramelessWindow *mw = mainFramelessEnabledApplicationWindow();
        if (mw && mw->getWidget())
        {
            return mw->getWidget();
        }

        // second choice, try via QMainWindow
        QWidgetList tlw = topLevelApplicationWidgetsWithName();
        for (QWidget *w : tlw)
        {
            QMainWindow *qmw = qobject_cast<QMainWindow *>(w);
            if (!qmw) { continue; }
            if (!qmw->parentWidget()) { return qmw; }

        }
        return nullptr;
    }

    bool CGuiUtility::isMainWindowFrameless()
    {
        CEnableForFramelessWindow *mw = mainFramelessEnabledApplicationWindow();
        Q_ASSERT(mw); // there should be a main window
        return (mw && mw->isFrameless());
    }

    void CGuiUtility::initSwiftGuiApplication(QApplication &a, const QString &applicationName, const QPixmap &icon)
    {
        CRuntime::registerMetadata(); // register metadata
        CCookieManager::instance(); // init cookie manager if ever needed

        CLogHandler::instance()->install(); // make sure we have a log handler!

        QApplication::setApplicationName(applicationName);
        QApplication::setApplicationVersion(CProject::version());
        QApplication::setWindowIcon(icon);

        // Logging
        QString executableName = QFileInfo(QCoreApplication::applicationFilePath()).completeBaseName();
        QString category("swift." + executableName);

        // Translations
        QFile file(":blackmisc/translations/blackmisc_i18n_de.qm");
        CLogMessage(category).debug() << (file.exists() ? "Found translations in resources" : "No translations in resources");
        QTranslator translator;
        if (translator.load("blackmisc_i18n_de", ":blackmisc/translations/"))
        {
            CLogMessage(category).debug() << "Translator loaded";
        }

        // File logger
        static const QString logPath = QStandardPaths::writableLocation(QStandardPaths::GenericDataLocation) + "/org.swift-project/logs";
        CFileLogger *fileLogger = new CFileLogger(executableName, logPath, &a);
        fileLogger->changeLogPattern(CLogPattern().withSeverityAtOrAbove(CStatusMessage::SeverityDebug));

        // GUI icon
        a.installTranslator(&translator);
    }

    bool CGuiUtility::lenientTitleComparison(const QString &title, const QString &comparison)
    {
        if (title == comparison) { return true; }

        QString t(title.trimmed().toLower().simplified());
        QString c(comparison.trimmed().toLower().simplified());
        Q_ASSERT_X(!t.isEmpty(), Q_FUNC_INFO, "missing value");
        Q_ASSERT_X(!c.isEmpty(), Q_FUNC_INFO, "missing value");
        if (t == c) { return true; }

        // further unify
        static QThreadStorage<QRegularExpression> tsRegex;
        if (! tsRegex.hasLocalData()) { tsRegex.setLocalData(QRegularExpression("[^a-z0-9\\s]")); }
        const QRegularExpression &regexp = tsRegex.localData();
        t = t.remove(regexp);
        c = c.remove(regexp);
        return t == c;
    }

    bool CGuiUtility::setComboBoxValueByStartingString(QComboBox *box, const QString &candidate, const QString &unspecified)
    {
        if (!box) { return false; }
        if (!candidate.isEmpty())
        {
            for (int i = 0; i < box->count(); i++)
            {
                QString t(box->itemText(i));
                if (t.startsWith(candidate, Qt::CaseInsensitive))
                {
                    box->setCurrentIndex(i);
                    return true;
                }
            }
        }

        // not found
        if (unspecified.isEmpty()) { return false; }
        for (int i = 0; i < box->count(); i++)
        {
            QString t(box->itemText(i));
            if (t.startsWith(unspecified, Qt::CaseInsensitive))
            {
                box->setCurrentIndex(i);
                return true;
            }
        }
        return false;
    }

    bool CGuiUtility::hasSwiftVariantMimeType(const QMimeData *mime)
    {
        return mime && mime->hasFormat(swiftJsonDragAndDropMimeType());
    }

    CVariant CGuiUtility::fromSwiftDragAndDropData(const QMimeData *mime)
    {
        if (hasSwiftVariantMimeType(mime))
        {
            return fromSwiftDragAndDropData(mime->data(swiftJsonDragAndDropMimeType()));
        }
        return CVariant();
    }

    CVariant CGuiUtility::fromSwiftDragAndDropData(const QByteArray &utf8Data)
    {
        if (utf8Data.isEmpty()) { return CVariant(); }
        QJsonDocument jsonDoc(QJsonDocument::fromJson(utf8Data));
        QJsonObject jsonObj(jsonDoc.object());
        QString typeName(jsonObj.value("type").toString());
        int typeId = QMetaType::type(qPrintable(typeName));

        // check if a potential valid value object
        if (typeName.isEmpty() || typeId == QMetaType::UnknownType) { return CVariant(); }

        CVariant valueVariant;
        valueVariant.convertFromJson(jsonObj);
        return valueVariant;
    }

    int CGuiUtility::metaTypeIdFromSwiftDragAndDropData(const QMimeData *mime)
    {
        static const int Unknown = static_cast<int>(QMetaType::UnknownType);

        if (!hasSwiftVariantMimeType(mime)) { return Unknown; }
        QJsonDocument jsonDoc(QJsonDocument::fromJson(mime->data(swiftJsonDragAndDropMimeType())));
        QJsonObject jsonObj(jsonDoc.object());
        if (jsonObj.isEmpty()) { return Unknown; }
        QString typeName(jsonObj.value("type").toString());
        if (typeName.isEmpty()) { return Unknown; }
        int typeId = QMetaType::type(qPrintable(typeName));
        return typeId;
    }

    const QString &CGuiUtility::swiftJsonDragAndDropMimeType()
    {
        static const QString m("text/json/swift");
        return m;
    }

    void CGuiUtility::checkBoxReadOnly(QCheckBox *checkBox, bool readOnly)
    {
        static const QCheckBox defaultBox;
        BLACK_VERIFY_X(checkBox, Q_FUNC_INFO, "no checkbox");
        if (!checkBox) { return; }

        if (readOnly)
        {
            checkBox->setAttribute(Qt::WA_TransparentForMouseEvents);
            checkBox->setFocusPolicy(Qt::NoFocus);
        }
        else
        {
            checkBox->setAttribute(Qt::WA_TransparentForMouseEvents, defaultBox.testAttribute(Qt::WA_TransparentForMouseEvents));
            checkBox->setFocusPolicy(defaultBox.focusPolicy());
        }
    }

    QWidgetList CGuiUtility::topLevelApplicationWidgetsWithName()
    {
        QWidgetList tlw = QApplication::topLevelWidgets();
        QWidgetList rl;
        foreach (QWidget *w, tlw)
        {
            if (w->objectName().isEmpty()) { continue; }
            rl.append(w);
        }
        return rl;
    }

    QPoint CGuiUtility::mainWindowPosition()
    {
        CEnableForFramelessWindow *mw = mainFramelessEnabledApplicationWindow();
        return (mw) ? mw->getWidget()->pos() : QPoint();
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
        QMessageBox::warning(nullptr, QGuiApplication::applicationDisplayName(), "<html><head/><body><h2>" + errorMessage + "</h2><pre>" + parser.helpText() + "</pre></body></html>");
#   else
        fputs(qPrintable(errorMessage), stderr);
        fputs("\n\n", stderr);
        fputs(qPrintable(parser.helpText()), stderr);
#   endif
    }

    void CGuiUtility::commandLineVersionRequested()
    {
#   ifdef Q_OS_WIN
        QMessageBox::information(nullptr, QGuiApplication::applicationDisplayName(), QGuiApplication::applicationDisplayName() + ' ' + QCoreApplication::applicationVersion());
#   else
        printf("%s %s\n", qPrintable(QCoreApplication::applicationName()), qPrintable(QCoreApplication::applicationVersion()));
#   endif
    }

    void CGuiUtility::commandLineHelpRequested(QCommandLineParser &parser)
    {
#   ifdef Q_OS_WIN
        QMessageBox::information(nullptr, QGuiApplication::applicationDisplayName(), "<html><head/><body><pre>" + parser.helpText() + "</pre></body></html>");
#   else
        parser.showHelp(); // terminates
        Q_UNREACHABLE();
#   endif
    }
} // ns

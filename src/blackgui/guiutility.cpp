/* Copyright (C) 2014
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

#include "guiutility.h"
#include "blackcore/corefacade.h"
#include "blackcore/cookiemanager.h"
#include "blackmisc/filelogger.h"
#include "blackmisc/logmessage.h"
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
        const QWidgetList tlw = topLevelApplicationWidgetsWithName();
        for (QWidget *w : tlw)
        {
            // best choice is to check on frameless window
            CEnableForFramelessWindow *mw = dynamic_cast<CEnableForFramelessWindow *>(w);
            if (mw && mw->isMainApplicationWindow()) { return mw; }
        }
        return nullptr;
    }

    QWidget *CGuiUtility::mainApplicationWindow()
    {
        CEnableForFramelessWindow *mw = mainFramelessEnabledApplicationWindow();
        if (mw && mw->getWidget())
        {
            return mw->getWidget();
        }

        // second choice, try via QMainWindow
        const QWidgetList tlw = topLevelApplicationWidgetsWithName();
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
        constexpr int Unknown = static_cast<int>(QMetaType::UnknownType);

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

    bool CGuiUtility::staysOnTop(QWidget *widget)
    {
        if (!widget) { return false; }
        const Qt::WindowFlags flags = widget->windowFlags();
        return Qt::WindowStaysOnTopHint & flags;
    }

    bool CGuiUtility::toggleStayOnTop(QWidget *widget)
    {
        if (!widget) { return false; }
        Qt::WindowFlags flags = widget->windowFlags();
        if (Qt::WindowStaysOnTopHint & flags)
        {
            flags ^= Qt::WindowStaysOnTopHint;
            flags |= Qt::WindowStaysOnBottomHint;
        }
        else
        {
            flags ^= Qt::WindowStaysOnBottomHint;
            flags |= Qt::WindowStaysOnTopHint;
        }
        widget->setWindowFlags(flags);
        widget->show();
        return Qt::WindowStaysOnTopHint & flags;
    }
} // ns

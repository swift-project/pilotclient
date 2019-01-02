/* Copyright (C) 2014
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

#include "blackgui/enableforframelesswindow.h"
#include "blackgui/guiutility.h"
#include "blackgui/overlaymessagesframe.h"
#include "blackmisc/icon.h"
#include "blackmisc/verify.h"

#include <QApplication>
#include <QCheckBox>
#include <QComboBox>
#include <QLineEdit>
#include <QAbstractItemModel>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonValue>
#include <QLayout>
#include <QLayoutItem>
#include <QList>
#include <QMainWindow>
#include <QMetaType>
#include <QMimeData>
#include <QObject>
#include <QRegularExpression>
#include <QTabWidget>
#include <QThreadStorage>
#include <QWidget>
#include <QTimer>
#include <Qt>
#include <QPointer>
#include <QtGlobal>
#include <QGraphicsOpacityEffect>
#include <QPropertyAnimation>
#include <QDesktopWidget>

using namespace BlackMisc;

namespace BlackGui
{
    QWidget *CGuiUtility::s_mainApplicationWidget = nullptr;

    CEnableForFramelessWindow *CGuiUtility::mainFramelessEnabledWindow()
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

    namespace Private
    {
        QWidget *mainApplicationWidgetSearch()
        {
            CEnableForFramelessWindow *mw = CGuiUtility::mainFramelessEnabledWindow();
            if (mw && mw->getWidget())
            {
                return mw->getWidget();
            }

            // second choice, try via QMainWindow
            const QWidgetList tlw = CGuiUtility::topLevelApplicationWidgetsWithName();
            for (QWidget *w : tlw)
            {
                QMainWindow *qmw = qobject_cast<QMainWindow *>(w);
                if (!qmw) { continue; }
                if (!qmw->parentWidget()) { return qmw; }
            }
            return nullptr;
        }
    } // ns

    void CGuiUtility::registerMainApplicationWidget(QWidget *mainWidget)
    {
        CGuiUtility::s_mainApplicationWidget = mainWidget;
    }

    QWidget *CGuiUtility::mainApplicationWidget()
    {
        if (!CGuiUtility::s_mainApplicationWidget)
        {
            CGuiUtility::s_mainApplicationWidget = Private::mainApplicationWidgetSearch();
        }
        return CGuiUtility::s_mainApplicationWidget;
    }

    qreal CGuiUtility::mainApplicationWidgetPixelRatio()
    {
        const QWidget *mw = CGuiUtility::mainApplicationWidget();
        if (mw) { return mw->devicePixelRatio(); }
        return 1.0;
    }

    QSize CGuiUtility::desktopSize()
    {
        const QWidget *mw = CGuiUtility::mainApplicationWidget();
        if (mw) { return QApplication::desktop()->screenGeometry(mw).size(); }

        // main screen
        return QApplication::desktop()->screenGeometry(QApplication::desktop()).size();
    }

    bool CGuiUtility::isMainWindowFrameless()
    {
        const CEnableForFramelessWindow *mw = CGuiUtility::mainFramelessEnabledWindow();
        return (mw && mw->isFrameless());
    }

    bool CGuiUtility::lenientTitleComparison(const QString &title, const QString &comparison)
    {
        if (title == comparison) { return true; }

        QString t(title.trimmed().toLower().simplified());
        QString c(comparison.trimmed().toLower().simplified());
        Q_ASSERT_X(!t.isEmpty(), Q_FUNC_INFO, "missing title");
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
                const QString t(box->itemText(i));
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
            const QString t(box->itemText(i));
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
        if (!hasSwiftVariantMimeType(mime)) { return CVariant(); }
        return CGuiUtility::fromSwiftDragAndDropData(mime->data(swiftJsonDragAndDropMimeType()));
    }

    CVariant CGuiUtility::fromSwiftDragAndDropData(const QByteArray &utf8Data)
    {
        if (utf8Data.isEmpty()) { return CVariant(); }
        const QJsonDocument jsonDoc(QJsonDocument::fromJson(utf8Data));
        const QJsonObject jsonObj(jsonDoc.object());
        const QString typeName(jsonObj.value("type").toString());
        const int typeId = QMetaType::type(qPrintable(typeName));

        // check if a potential valid value object
        if (typeName.isEmpty() || typeId == QMetaType::UnknownType) { return CVariant(); }

        CVariant valueVariant;
        const CStatusMessage status = valueVariant.convertFromJsonNoThrow(jsonObj, {}, {});
        if (status.isFailure()) { return CVariant(); }
        return valueVariant;
    }

    int CGuiUtility::metaTypeIdFromSwiftDragAndDropData(const QMimeData *mime)
    {
        constexpr int Unknown = static_cast<int>(QMetaType::UnknownType);

        if (!hasSwiftVariantMimeType(mime)) { return Unknown; }
        static const QJsonObject jsonObj(QJsonDocument::fromJson(mime->data(swiftJsonDragAndDropMimeType())).object());
        Q_ASSERT_X(!jsonObj.isEmpty(), Q_FUNC_INFO, "Empty JSON object");
        const QString typeName(jsonObj.value("type").toString());
        if (typeName.isEmpty()) { return Unknown; }
        const int typeId = QMetaType::type(qPrintable(typeName));
        return typeId;
    }

    const QString &CGuiUtility::swiftJsonDragAndDropMimeType()
    {
        static const QString m("text/json/swift");
        return m;
    }

    QFileInfo CGuiUtility::representedMimeFile(const QMimeData *mime)
    {
        if (!mime->hasText()) { return QFileInfo(); }
        const QString candidate = mime->text();
        if (candidate.isEmpty()) { return QFileInfo(); }
        if (!candidate.contains("://")) { return QFileInfo(candidate); }
        QUrl url(candidate);
        const QString localFile = url.toLocalFile();
        return QFileInfo(localFile);
    }

    bool CGuiUtility::isMimeRepresentingReadableFile(const QMimeData *mime)
    {
        const QFileInfo fi = CGuiUtility::representedMimeFile(mime);
        return fi.isReadable();
    }

    bool CGuiUtility::isMimeRepresentingReadableJsonFile(const QMimeData *mime)
    {
        const QFileInfo fi = CGuiUtility::representedMimeFile(mime);
        if (!fi.isReadable()) { return false; }
        const QString fn = fi.fileName();
        return fn.endsWith("json", Qt::CaseInsensitive);
    }

    COverlayMessagesFrame *CGuiUtility::nextOverlayMessageFrame(QWidget *widget, int maxLevels)
    {
        return nextOverlayMessageWidget<COverlayMessagesFrame>(widget, maxLevels);
    }

    COverlayMessagesTabWidget *CGuiUtility::nextOverlayMessageTabWidget(QWidget *widget, int maxLevels)
    {
        return nextOverlayMessageWidget<COverlayMessagesTabWidget>(widget, maxLevels);
    }

    COverlayMessagesWizardPage *CGuiUtility::nextOverlayMessageWizardPage(QWidget *widget, int maxLevels)
    {
        return nextOverlayMessageWidget<COverlayMessagesWizardPage>(widget, maxLevels);
    }

    void CGuiUtility::checkBoxReadOnly(QCheckBox *checkBox, bool readOnly)
    {
        static const QCheckBox defaultBox;
        BLACK_VERIFY_X(checkBox, Q_FUNC_INFO, "no checkbox");
        if (!checkBox) { return; }

        static const QString background("background: rgba(40,40,40)"); //! \fixme hardcoded stylesheet setting, should come from stylesheet
        if (readOnly)
        {
            checkBox->setAttribute(Qt::WA_TransparentForMouseEvents);
            checkBox->setFocusPolicy(Qt::NoFocus);

            // without that, the checkboxes appear not readonly
            // obviously style sheet only does not work
            checkBox->setStyleSheet(background); //! fixme hardcoded stylesheet setting
        }
        else
        {
            checkBox->setAttribute(Qt::WA_TransparentForMouseEvents, defaultBox.testAttribute(Qt::WA_TransparentForMouseEvents));
            checkBox->setFocusPolicy(defaultBox.focusPolicy());
            checkBox->setStyleSheet("");
        }
    }

    void CGuiUtility::checkBoxesReadOnly(QWidget *parent, bool readOnly)
    {
        if (!parent) { return; }
        QList<QCheckBox *> allCheckBoxes = parent->findChildren<QCheckBox *>();
        for (QCheckBox *cb : allCheckBoxes)
        {
            CGuiUtility::checkBoxReadOnly(cb, readOnly);
        }
    }

    void CGuiUtility::tempUnhidePassword(QLineEdit *lineEdit, int unhideMs)
    {
        if (!lineEdit) { return; }
        if (lineEdit->text().isEmpty()) { return; }
        if (lineEdit->echoMode() != QLineEdit::Password && lineEdit->echoMode() != QLineEdit::PasswordEchoOnEdit) { return; }
        const QLineEdit::EchoMode mode = lineEdit->echoMode();
        lineEdit->setEchoMode(QLineEdit::Normal);
        QPointer<QLineEdit> qpLineEdit(lineEdit);
        QTimer::singleShot(unhideMs, lineEdit, [ = ]
        {
            if (qpLineEdit) { qpLineEdit->setEchoMode(mode); }
        });
    }

    QWidgetList CGuiUtility::topLevelApplicationWidgetsWithName()
    {
        QWidgetList tlw = QApplication::topLevelWidgets();
        QWidgetList rl;
        for (QWidget *w : tlw)
        {
            if (w->objectName().isEmpty()) { continue; }
            rl.append(w);
        }
        return rl;
    }

    QPoint CGuiUtility::mainWidgetPosition()
    {
        CEnableForFramelessWindow *mw = CGuiUtility::mainFramelessEnabledWindow();
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

    QTabWidget *CGuiUtility::parentTabWidget(QWidget *widget, int maxLevels)
    {
        int level = 0;
        do
        {
            widget = widget->parentWidget();
            if (!widget) { return nullptr; }
            QTabWidget *tw = qobject_cast<QTabWidget *>(widget);
            if (tw) { return tw; }
            level++;
        }
        while (level < maxLevels);
        return nullptr;
    }

    bool CGuiUtility::toggleStayOnTop(QWidget *widget)
    {
        if (!widget) { return false; }
        Qt::WindowFlags flags = widget->windowFlags();
        if (Qt::WindowStaysOnTopHint & flags)
        {
            flags &= ~Qt::WindowStaysOnTopHint;
            flags |= Qt::WindowStaysOnBottomHint;
        }
        else
        {
            flags &= ~Qt::WindowStaysOnBottomHint;
            flags |= Qt::WindowStaysOnTopHint;
        }
        widget->setWindowFlags(flags);
        widget->show();
        return Qt::WindowStaysOnTopHint & flags;
    }

    bool CGuiUtility::stayOnTop(bool onTop, QWidget *widget)
    {
        if (!widget) { return false; }
        Qt::WindowFlags flags = widget->windowFlags();
        if (onTop)
        {
            flags &= ~Qt::WindowStaysOnBottomHint;
            flags |= Qt::WindowStaysOnTopHint;
        }
        else
        {
            flags &= ~Qt::WindowStaysOnTopHint;
            // flags |= Qt::WindowStaysOnBottomHint;
        }
        widget->setWindowFlags(flags);
        return onTop;
    }

    QString CGuiUtility::marginsToString(const QMargins &margins)
    {
        return QStringLiteral("%1:%2:%3:%4").arg(margins.left()).arg(margins.top()).arg(margins.right()).arg(margins.bottom());
    }

    QMargins CGuiUtility::stringToMargins(const QString &str)
    {
        const QStringList parts = str.split(":");
        Q_ASSERT_X(parts.size() == 4, Q_FUNC_INFO, "malformed");
        bool ok = false;
        const int l = parts.at(0).toInt(&ok);
        Q_ASSERT_X(ok, Q_FUNC_INFO, "malformed number");
        const int t = parts.at(1).toInt(&ok);
        Q_ASSERT_X(ok, Q_FUNC_INFO, "malformed number");
        const int r = parts.at(2).toInt(&ok);
        Q_ASSERT_X(ok, Q_FUNC_INFO, "malformed number");
        const int b = parts.at(3).toInt(&ok);
        Q_ASSERT_X(ok, Q_FUNC_INFO, "malformed number");
        Q_UNUSED(ok);
        return QMargins(l, t, r, b);
    }

    QList<int> CGuiUtility::indexToUniqueRows(const QModelIndexList &indexes)
    {
        QList<int> rows;
        for (const QModelIndex &i : indexes)
        {
            const int r = i.row();
            if (rows.contains(r)) { continue; }
            rows.append(r);
        }
        return rows;
    }

    int CGuiUtility::clearModel(QAbstractItemModel *model)
    {
        if (!model) { return 0; }
        const int count = model->rowCount();
        if (count < 1) { return 0; }
        model->removeRows(0, count);
        return count;
    }

    bool CGuiUtility::isTopLevelWidget(QWidget *widget)
    {
        if (!widget) { return false; }
        return QApplication::topLevelWidgets().contains(widget);
    }

    bool CGuiUtility::isTopLevelWindow(QWidget *widget)
    {
        if (!widget) { return false; }
        if (!widget->isWindow()) { return false; }
        return QApplication::topLevelWidgets().contains(widget);
    }

    bool CGuiUtility::isQMainWindow(QWidget *widget)
    {
        if (!widget) { return false; }
        QMainWindow *mw = qobject_cast<QMainWindow *>(widget);
        return mw;
    }

    bool CGuiUtility::isDialog(QWidget *widget)
    {
        if (!widget) { return false; }
        QDialog *mw = qobject_cast<QDialog *>(widget);
        return mw;
    }

    QGraphicsOpacityEffect *CGuiUtility::fadeInWidget(int durationMs, QWidget *widget, double startValue, double endValue)
    {
        // http://stackoverflow.com/questions/19087822/how-to-make-qt-widgets-fade-in-or-fade-out#
        Q_ASSERT(widget);
        QGraphicsOpacityEffect *effect = new QGraphicsOpacityEffect(widget);
        widget->setGraphicsEffect(effect);
        QPropertyAnimation *a = new QPropertyAnimation(effect, "opacity");
        a->setDuration(durationMs);
        a->setStartValue(startValue);
        a->setEndValue(endValue);
        a->setEasingCurve(QEasingCurve::InBack);
        a->start(QPropertyAnimation::DeleteWhenStopped);
        return effect;
    }

    QGraphicsOpacityEffect *CGuiUtility::fadeOutWidget(int durationMs, QWidget *widget, double startValue, double endValue)
    {
        Q_ASSERT(widget);
        QGraphicsOpacityEffect *effect = new QGraphicsOpacityEffect(widget);
        widget->setGraphicsEffect(effect);
        QPropertyAnimation *a = new QPropertyAnimation(effect, "opacity");
        a->setDuration(durationMs);
        a->setStartValue(startValue);
        a->setEndValue(endValue);
        a->setEasingCurve(QEasingCurve::OutBack);
        a->start(QPropertyAnimation::DeleteWhenStopped);
        return effect;
    }

    QFontMetrics CGuiUtility::currentFontMetrics()
    {
        const QWidget *w = CGuiUtility::mainApplicationWidget();
        if (w) { return w->fontMetrics(); }
        return QApplication::fontMetrics();
    }

    QFontMetricsF CGuiUtility::currentFontMetricsF()
    {
        return QFontMetricsF(CGuiUtility::currentFontMetrics());
    }

    QFont CGuiUtility::currentFont()
    {
        const QWidget *w = CGuiUtility::mainApplicationWidget();
        if (w) { return w->font(); }
        return QApplication::font();
    }

    QSizeF CGuiUtility::fontMetrics80Chars(bool withRatio)
    {
        // scale is 3.0 on my hires display
        static const QString s("01234567890123456789012345678901234567890123456789012345678901234567890123456789");
        const QFontMetricsF fm = CGuiUtility::currentFontMetricsF();
        const qreal scale = withRatio ? CGuiUtility::mainApplicationWidgetPixelRatio() : 1.0;
        const qreal w = fm.width(s) * scale;
        const qreal h = fm.height() * scale;
        return QSizeF(w, h);
    }

    QSizeF CGuiUtility::fontMetricsLazyDog43Chars(bool withRatio)
    {
        // 43 characters        0123456789012345678901234567890123456789012
        static const QString s("The quick brown fox jumps over the lazy dog");
        const QFontMetricsF fm = CGuiUtility::currentFontMetrics();
        const qreal scale = withRatio ? CGuiUtility::mainApplicationWidgetPixelRatio() : 1.0;
        const qreal w = fm.width(s) * scale;
        const qreal h = fm.height() * scale;
        return QSizeF(w, h);
    }

    QSizeF CGuiUtility::fontMetricsEstimateSize(int xCharacters, int yCharacters, bool withRatio)
    {
        // 1920/1080: 560/16 256/16 => 530/960
        // 3840/2160: 400/10 178/10 => 375/600
        // with ratio we get the physical solution, otherwise logical solution
        const QSizeF s1 = CGuiUtility::fontMetrics80Chars(withRatio);
        const QSizeF s2 = CGuiUtility::fontMetricsLazyDog43Chars(withRatio);
        const QSizeF s = s1 + s2;
        const qreal w = s.width()  * xCharacters / 123; // 123 chars
        const qreal h = s.height() * yCharacters / 2;   // 2 lines
        return QSizeF(w, h);
    }

    QString CGuiUtility::metricsInfo()
    {
        static const QString s("%1 %2 %3 | 80 chars: w%4 h%5 | 43 chars: w%6 h%7");
        const QSizeF s80 = CGuiUtility::fontMetrics80Chars();
        const QSizeF s43 = CGuiUtility::fontMetricsLazyDog43Chars();

        QString ratio("-");
        QString desktop("-");

        const QWidget *mainWidget = CGuiUtility::mainApplicationWidget();
        if (mainWidget)
        {
            // const QSize sd = QApplication::desktop()->screenGeometry().size();
            const QSize sd = QApplication::desktop()->screenGeometry(mainWidget).size();
            desktop = QStringLiteral("Desktop w%1 w%2").arg(sd.width()).arg(sd.height());
            ratio = QStringLiteral("ratio: %1").arg(mainWidget->devicePixelRatioF());
        }
        return s.
               arg(desktop).
               arg(CGuiUtility::isUsingHighDpiScreenSupport() ? "hi DPI" : "-").
               arg(ratio).
               arg(s80.width()).arg(s80.height()).arg(s43.width()).arg(s43.height());
    }

    bool CGuiUtility::isUsingHighDpiScreenSupport()
    {
        const QByteArray v = qgetenv("QT_AUTO_SCREEN_SCALE_FACTOR");
        const QString vs(v);
        const bool highDpi = stringToBool(vs);
        return highDpi;
    }

    void CGuiUtility::forceStyleSheetUpdate(QWidget *widget)
    {
        if (!widget) { return; }
        widget->setStyleSheet(widget->styleSheet());
    }

    void CGuiUtility::superviseMainWindowMinSizes(qreal wRatio, qreal hRatio)
    {
        QWidget *w = CGuiUtility::mainApplicationWidget();
        if (!w) { return; }
        const QSize s = CGuiUtility::desktopSize();
        const int minW = qRound(wRatio * s.width());
        const int minH = qRound(hRatio * s.height());
        w->setMinimumWidth(qMin(minW, w->minimumWidth()));
        w->setMinimumHeight(qMin(minH, w->minimumHeight()));
    }

    QString CGuiUtility::asSimpleHtmlImageWidth(const CIcon &icon, int width)
    {
        if (!icon.hasFileResourcePath()) return {};
        const QString p = icon.getFileResourcePath();

        if (width < 0) { return QStringLiteral("<img src=\"%1\">").arg(p); }
        return QStringLiteral("<img src=\"%1\" width=%2>").arg(p, QString::number(width));
    }

    QString CGuiUtility::asSimpleHtmlImageHeight(const CIcon &icon, int height)
    {
        if (height < 0) { return CGuiUtility::asSimpleHtmlImageWidth(icon); }
        if (!icon.hasFileResourcePath()) return {};
        const QString p = icon.getFileResourcePath();

        return QStringLiteral("<img src=\"%1\" height=%2>").arg(p, QString::number(height));
    }
} // ns

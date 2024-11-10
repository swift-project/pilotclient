// SPDX-FileCopyrightText: Copyright (C) 2014 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

#include "gui/enableforframelesswindow.h"
#include "gui/guiutility.h"
#include "gui/overlaymessagesframe.h"
#include "misc/icon.h"
#include "misc/verify.h"

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
#include <QWizard>
#include <QScreen>
#include <QLabel>
#include <QTimer>
#include <Qt>
#include <QPointer>
#include <QtGlobal>
#include <QStringBuilder>
#include <QGraphicsOpacityEffect>
#include <QPropertyAnimation>

// for the screen size

#ifdef Q_OS_WINDOWS
#    include "wtypes.h"
#    include <iostream>
#endif

using namespace swift::misc;

namespace swift::gui
{
    QWidget *CGuiUtility::s_mainApplicationWidget = nullptr;

    CEnableForFramelessWindow *CGuiUtility::mainFramelessEnabledWindow()
    {
        const QWidgetList tlw = topLevelApplicationWidgetsWithName();
        for (QWidget *w : tlw)
        {
            // best choice is to check on frameless window
            CEnableForFramelessWindow *mw = dynamic_cast<CEnableForFramelessWindow *>(w);
            if (!mw) { continue; }
            if (mw->isMainApplicationWindow()) { return mw; }
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
        if (!mw) return QGuiApplication::primaryScreen()->size();

        const QWindow *win = mw->windowHandle();
        if (!win) return QGuiApplication::primaryScreen()->size();

        return win->size();
    }

    namespace Private
    {
#ifdef Q_OS_WINDOWS
        QSize windowsGetDesktopResolution()
        {
            /**
            // https://stackoverflow.com/questions/8690619/how-to-get-screen-resolution-in-c
            RECT desktop;
            // Get a handle to the desktop window
            const HWND hDesktop = GetDesktopWindow();
            // Get the size of screen to the variable desktop
            GetWindowRect(hDesktop, &desktop);
            // The top left corner will have coordinates (0,0)
            // and the bottom right corner will have coordinates
            // (horizontal, vertical)
            const int horizontal = desktop.right;
            const int vertical   = desktop.bottom;
            return QSize(vertical, horizontal);
            **/

            // https://stackoverflow.com/questions/2156212/how-to-get-the-monitor-screen-resolution-from-a-hwnd
            const HWND hDesktop = GetDesktopWindow();
            HMONITOR monitor = MonitorFromWindow(hDesktop, MONITOR_DEFAULTTONEAREST);
            MONITORINFO info;
            info.cbSize = sizeof(MONITORINFO);
            GetMonitorInfo(monitor, &info);

            /**
            DEVICE_SCALE_FACTOR pScale;
            GetScaleFactorForMonitor(monitor, &pScale);
            **/

            const int monitor_width = info.rcMonitor.right - info.rcMonitor.left;
            const int monitor_height = info.rcMonitor.bottom - info.rcMonitor.top;
            return QSize(monitor_height, monitor_width);

            /**
            // https://stackoverflow.com/a/50205813/356726
            const int width  = static_cast<int>(GetSystemMetrics(SM_CXSCREEN));
            const int height = static_cast<int>(GetSystemMetrics(SM_CYSCREEN));
            return QSize(width, height);
            **/
        }
#endif
    } // namespace Private

    QSize CGuiUtility::physicalScreenSizeOs()
    {
#ifdef Q_OS_WINDOWS
        return Private::windowsGetDesktopResolution();
#elif defined(Q_OS_MAC)
        return QSize();
#elif defined(Q_OS_LINUX)
        return QSize();
#else
        return QSize();
#endif
    }

    bool CGuiUtility::isMainWindowFrameless()
    {
        const CEnableForFramelessWindow *mw = CGuiUtility::mainFramelessEnabledWindow();
        return (mw && mw->isFrameless());
    }

    QString CGuiUtility::screenInformation(const QString &separator)
    {
        const QSize ps = physicalScreenSizeOs();
        QString i = u"Number of screens: " % QString::number(QGuiApplication::screens().size()) % separator %
                    u"Primary screen: " % QGuiApplication::primaryScreen()->name() % separator %
                    u"QT_AUTO_SCREEN_SCALE_FACTOR: " % qgetenv("QT_AUTO_SCREEN_SCALE_FACTOR") % separator %
                    u"QT_SCALE_FACTOR: " % qgetenv("QT_SCALE_FACTOR") % separator %
                    u"QT_ENABLE_HIGHDPI_SCALING: " % qgetenv("QT_ENABLE_HIGHDPI_SCALING") % separator %
                    u"QT_SCALE_FACTOR_ROUNDING_POLICY: " % qgetenv("QT_SCALE_FACTOR_ROUNDING_POLICY") % separator %
                    u"QT_SCREEN_SCALE_FACTORS: " % qgetenv("QT_SCREEN_SCALE_FACTORS") % separator %
                    u"OS screen res." % QString::number(ps.width()) % u"/" % QString::number(ps.height()) % separator;

        for (const QScreen *screen : QGuiApplication::screens())
        {
            i += separator %
                 u"Information for screen: " % screen->name() % separator %
                 u"Available geometry: " % rectAsString(screen->availableGeometry()) % separator %
                 u"Available size: " % sizeAsString(screen->availableSize()) % separator %
                 u"Available virtual geometry: " % rectAsString(screen->availableVirtualGeometry()) % separator %
                 u"Available virtual size: " % sizeAsString(screen->availableVirtualSize()) % separator %
                 u"Device ratio: " % QString::number(screen->devicePixelRatio()) % separator %
                 u"Depth: " % QString::number(screen->depth()) % u"bits" % separator %
                 u"Geometry: " % rectAsString(screen->geometry()) % separator %
                 u"Logical DPI: " % QString::number(screen->logicalDotsPerInch()) % separator %
                 u"Logical DPI X: " % QString::number(screen->logicalDotsPerInchX()) % separator %
                 u"Logical DPI Y: " % QString::number(screen->logicalDotsPerInchY()) % separator %
                 u"Orientation: " % orientationAsString(screen->orientation()) % separator %
                 u"Physical DPI: " % QString::number(screen->physicalDotsPerInch()) % separator %
                 u"Physical DPI X: " % QString::number(screen->physicalDotsPerInchX()) % separator %
                 u"Physical DPI Y: " % QString::number(screen->physicalDotsPerInchY()) % separator %
                 u"Physical size: " % sizeAsString(screen->physicalSize()) % u"mm" % separator %
                 u"Primary orientation: " % orientationAsString(screen->primaryOrientation()) % separator %
                 u"Refresh rate: " % QString::number(screen->refreshRate()) % u"Hz" %
                 u"Size: " % sizeAsString(screen->size()) % separator %
                 u"Virtual geometry: " % rectAsString(screen->virtualGeometry()) % separator %
                 u"Virtual size: " % sizeAsString(screen->virtualSize());
        }
        return i;
    }

    const QString &CGuiUtility::orientationAsString(Qt::ScreenOrientation orientation)
    {
        static const QString pr("Primary");
        static const QString la("Landscape");
        static const QString po("Portrait");
        static const QString il("Inverted landscape");
        static const QString ip("Inverted portrait");

        switch (orientation)
        {
        case Qt::PrimaryOrientation: return pr;
        case Qt::LandscapeOrientation: return la;
        case Qt::PortraitOrientation: return po;
        case Qt::InvertedLandscapeOrientation: return il;
        case Qt::InvertedPortraitOrientation: return ip;
        default: break;
        }

        static const QString unknown("Unknown");
        return unknown;
    }

    QString CGuiUtility::rectAsString(const QRect &rect)
    {
        return QStringLiteral("x: %1 y: %2 w: %3 h: %4").arg(rect.x()).arg(rect.y()).arg(rect.width()).arg(rect.height());
    }

    QString CGuiUtility::rectAsString(const QRectF &rect)
    {
        return QStringLiteral("x: %1 y: %2 w: %3 h: %4").arg(rect.x()).arg(rect.y()).arg(rect.width()).arg(rect.height());
    }

    QString CGuiUtility::sizeAsString(const QSize &size)
    {
        return QStringLiteral("w: %1 h: %2").arg(size.width()).arg(size.height());
    }

    QString CGuiUtility::sizeAsString(const QSizeF &size)
    {
        return QStringLiteral("w: %1 h: %2").arg(size.width()).arg(size.height());
    }

    static QThreadStorage<QRegularExpression> tsRegex;

    bool CGuiUtility::lenientTitleComparison(const QString &title, const QString &comparison)
    {
        if (title == comparison) { return true; }

        QString t(title.trimmed().toLower().simplified());
        QString c(comparison.trimmed().toLower().simplified());

        // we should not have empty titles
        SWIFT_VERIFY_X(!t.isEmpty(), Q_FUNC_INFO, "missing title");
        SWIFT_VERIFY_X(!c.isEmpty(), Q_FUNC_INFO, "missing comparison value");
        if (t.isEmpty() || c.isEmpty()) { return false; }

        // same?
        if (t == c) { return true; }

        // further unify
        if (!tsRegex.hasLocalData()) { tsRegex.setLocalData(QRegularExpression("[^a-z0-9\\s]")); }
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

    bool CGuiUtility::setComboBoxValueByContainingString(QComboBox *box, const QString &candidate, const QString &unspecified)
    {
        if (!box) { return false; }
        if (!candidate.isEmpty())
        {
            const int ci = box->currentIndex();
            for (int i = 0; i < box->count(); i++)
            {
                const QString t(box->itemText(i));
                if (t.contains(candidate, Qt::CaseInsensitive))
                {
                    if (ci == i) { return true; } // avoid signals
                    box->setCurrentIndex(i);
                    return true;
                }
            }
        }

        // not found
        if (unspecified.isEmpty()) { return false; }
        const int ci = box->currentIndex();
        for (int i = 0; i < box->count(); i++)
        {
            const QString t(box->itemText(i));
            if (t.contains(unspecified, Qt::CaseInsensitive))
            {
                if (ci == i) { return true; } // avoid signals
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
        SWIFT_VERIFY_X(checkBox, Q_FUNC_INFO, "no checkbox");
        if (!checkBox) { return; }

        static const QString background("background: rgb(40,40,40)"); //! \fixme hardcoded stylesheet setting, should come from stylesheet
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
        QTimer::singleShot(unhideMs, lineEdit, [=] {
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

    QPoint CGuiUtility::mainWidgetGlobalPosition()
    {
        QWidget *mw = CGuiUtility::mainApplicationWidget();
        if (mw) { return mw->pos(); }

        // fallback, can be mfw it is not found
        CEnableForFramelessWindow *mfw = CGuiUtility::mainFramelessEnabledWindow();
        if (!mfw || !mfw->getWidget()) { return QPoint(); }
        return mfw->getWidget()->pos(); // is main window, so not mapToGlobal
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
        QLayoutItem *item { nullptr };
        while ((item = layout->takeAt(0)))
        {
            QLayout *sublayout { nullptr };
            QWidget *widget { nullptr };
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
            else { delete item; }
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
            // flags |= Qt::WindowStaysOnBottomHint;
        }
        else
        {
            flags &= ~Qt::WindowStaysOnBottomHint;
            flags |= Qt::WindowStaysOnTopHint;
        }
        widget->setWindowFlags(flags);
        widget->show(); // without that the window sometimes just disappears
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
        widget->show(); // without that the window sometimes just disappears
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
        Q_UNUSED(ok)
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

    bool CGuiUtility::isQMainWindow(const QWidget *widget)
    {
        if (!widget) { return false; }
        const QMainWindow *mw = qobject_cast<const QMainWindow *>(widget);
        return mw;
    }

    bool CGuiUtility::isDialog(const QWidget *widget)
    {
        if (!widget) { return false; }
        const QDialog *mw = qobject_cast<const QDialog *>(widget);
        return mw;
    }

    void CGuiUtility::disableMinMaxCloseButtons(QWidget *window)
    {
        if (!window->windowFlags().testFlag(Qt::CustomizeWindowHint))
        {
            window->setWindowFlag(Qt::CustomizeWindowHint);
            window->setWindowFlag(Qt::WindowTitleHint);
        }
        window->setWindowFlag(Qt::WindowMinMaxButtonsHint, false);
        window->setWindowFlag(Qt::WindowCloseButtonHint, false);
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
        const QSizeF size = fm.size(Qt::TextSingleLine, s);
        return size * scale;
    }

    QSizeF CGuiUtility::fontMetricsLazyDog43Chars(bool withRatio)
    {
        // 43 characters        0123456789012345678901234567890123456789012
        static const QString s("The quick brown fox jumps over the lazy dog");
        const QFontMetricsF fm = CGuiUtility::currentFontMetrics();
        const qreal scale = withRatio ? CGuiUtility::mainApplicationWidgetPixelRatio() : 1.0;
        const QSizeF size = fm.size(Qt::TextSingleLine, s);
        return size * scale;
    }

    QSizeF CGuiUtility::fontMetricsEstimateSize(int xCharacters, int yCharacters, bool withRatio)
    {
        // 1920/1080: 560/16 256/16 => 530/960
        // 3840/2160: 400/10 178/10 => 375/600
        // with ratio we get the physical solution, otherwise logical solution
        const QSizeF s1 = CGuiUtility::fontMetrics80Chars(withRatio);
        const QSizeF s2 = CGuiUtility::fontMetricsLazyDog43Chars(withRatio);
        const QSizeF s = s1 + s2;
        const qreal w = s.width() * xCharacters / 123; // 123 chars
        const qreal h = s.height() * yCharacters / 2; // 2 lines
        return QSizeF(w, h);
    }

    void CGuiUtility::centerWidget(QWidget *widget)
    {
        const QPoint point(widget->width() / 2.0, 0);
        const QScreen *pScreen = QGuiApplication::screenAt(widget->mapToGlobal(point));
        const QRect screenGeometry = pScreen->availableGeometry();
        const int x = (screenGeometry.width() - widget->width()) / 2;
        const int y = (screenGeometry.height() - widget->height()) / 2;
        widget->move(x, y);
    }

    void CGuiUtility::centerWidget(QWidget *widget, QWidget *host)
    {
        if (!host) { host = widget->parentWidget(); }

        if (host)
        {
            const QRect hostRect = host->geometry();
            widget->move(hostRect.center() - widget->rect().center());
        }
        else
        {
            CGuiUtility::centerWidget(widget);
        }
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
            const QSize sd = QGuiApplication::primaryScreen()->geometry().size();
            desktop = QStringLiteral("Desktop w%1 w%2").arg(sd.width()).arg(sd.height());
            ratio = QStringLiteral("ratio: %1").arg(mainWidget->devicePixelRatioF());
        }
        return s.arg(desktop).arg(CGuiUtility::isUsingHighDpiScreenSupport() ? "hi DPI" : "-").arg(ratio).arg(s80.width()).arg(s80.height()).arg(s43.width()).arg(s43.height());
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

    QDialog *CGuiUtility::findParentDialog(QWidget *widget)
    {
        if (CGuiUtility::isDialog(widget)) { return qobject_cast<QDialog *>(widget); }
        while (widget->parent())
        {
            widget = widget->parentWidget();
            if (CGuiUtility::isDialog(widget)) { return qobject_cast<QDialog *>(widget); }
        }
        return nullptr;
    }

    QDialog *CGuiUtility::findParentDialog(QWidget *widget, int maxLevel)
    {
        if (CGuiUtility::isDialog(widget)) { return qobject_cast<QDialog *>(widget); }
        int level = 0;
        while (widget->parent())
        {
            level++;
            if (level > maxLevel) { return nullptr; }
            widget = widget->parentWidget();
            if (CGuiUtility::isDialog(widget)) { return qobject_cast<QDialog *>(widget); }
        }
        return nullptr;
    }

    void CGuiUtility::setElidedText(QLabel *label, const QString &text, Qt::TextElideMode mode)
    {
        if (!label) { return; }

        label->setToolTip(text);
        if (mode == Qt::ElideNone)
        {
            label->setText(text);
            return;
        }

        const QFontMetrics metrics(label->font());
        const int width = qMax(label->width() - 2, 0);
        const QString clippedText = metrics.elidedText(text, mode, width);
        label->setText(clippedText);
    }

    void CGuiUtility::setElidedText(QLabel *label, const QString &shortText, const QString &longText, Qt::TextElideMode mode)
    {
        if (!label) { return; }
        if (shortText.isEmpty())
        {
            CGuiUtility::setElidedText(label, longText, mode);
            return;
        }
        if (longText.isEmpty())
        {
            CGuiUtility::setElidedText(label, shortText, mode);
            return;
        }

        label->setToolTip(longText);
        const QFontMetrics metrics(label->font());
        const int width = qMax(label->width() - 2, 0);
        const int wl = metrics.horizontalAdvance(longText);
        if (wl >= width)
        {
            label->setText(longText);
            return;
        }
        if (qRound(wl * 0.85) > wl)
        {
            const QString clippedText = metrics.elidedText(longText, mode, width);
            label->setText(clippedText);
            return;
        }
        const QString clippedText = metrics.elidedText(shortText, mode, width);
        label->setText(clippedText);
    }

    void CGuiUtility::setWizardButtonWidths(QWizard *wizard)
    {
        if (!wizard) { return; }

        const int minW = qMax(qRound(CGuiUtility::fontMetricsLazyDog43Chars(true).width() * 6.0 / 43.0), 80);
        if (wizard->button(QWizard::BackButton)) { wizard->button(QWizard::BackButton)->setMinimumWidth(minW); }
        if (wizard->button(QWizard::NextButton)) { wizard->button(QWizard::NextButton)->setMinimumWidth(minW); }
        if (wizard->button(QWizard::CancelButton)) { wizard->button(QWizard::CancelButton)->setMinimumWidth(minW); }
        if (wizard->button(QWizard::FinishButton)) { wizard->button(QWizard::FinishButton)->setMinimumWidth(minW); }
        if (wizard->button(QWizard::CustomButton1)) { wizard->button(QWizard::CustomButton1)->setMinimumWidth(minW); }
        if (wizard->button(QWizard::CustomButton2)) { wizard->button(QWizard::CustomButton2)->setMinimumWidth(minW); }
        if (wizard->button(QWizard::CustomButton3)) { wizard->button(QWizard::CustomButton3)->setMinimumWidth(minW); }
    }

    QWidgetList CGuiUtility::getAllModallWidgets()
    {
        const QWidgetList widgets = QApplication::topLevelWidgets();
        QWidgetList openWidgets;
        for (QWidget *w : widgets)
        {
            if (w->isModal()) { openWidgets.push_back(w); }
        }
        return openWidgets;
    }

    QStringList CGuiUtility::getAllWidgetTitles(const QWidgetList &widgets)
    {
        QStringList titles;
        for (const QWidget *w : widgets)
        {
            if (!w) { continue; }
            if (!w->windowTitle().isEmpty())
            {
                titles.push_back(w->windowTitle());
            }
            else
            {
                titles.push_back(QStringLiteral("name: ") % w->objectName());
            }
        }
        return titles;
    }

    QStringList CGuiUtility::getAllWidgetNames(const QWidgetList &widgets)
    {
        QStringList titles;
        for (const QWidget *w : widgets)
        {
            if (!w) { continue; }
            titles.push_back(QStringLiteral("name: ") % w->objectName());
        }
        return titles;
    }

    QList<QDockWidget *> CGuiUtility::getAllDockWidgets(QWidget *parent, bool floatingOnly)
    {
        QList<QDockWidget *> docks;
        if (parent)
        {
            const auto children = parent->findChildren<QDockWidget *>();
            for (QDockWidget *w : children)
            {
                if (!w) { continue; }
                if (!floatingOnly || w->isFloating())
                {
                    docks.push_back(w);
                }
            }
        }
        return docks;
    }

    QList<QDockWidget *> CGuiUtility::getAllDockWidgets(QWindow *parent, bool floatingOnly)
    {
        QList<QDockWidget *> docks;
        if (parent)
        {
            const auto children = parent->findChildren<QDockWidget *>();
            for (QDockWidget *w : children)
            {
                if (!w) { continue; }
                if (!floatingOnly || w->isFloating())
                {
                    docks.push_back(w);
                }
            }
        }
        return docks;
    }

    QWidgetList CGuiUtility::closeAllModalWidgets()
    {
        QWidgetList modals = getAllModallWidgets();
        for (QWidget *w : modals)
        {
            if (!w) { continue; }
            w->close();
        }
        return modals;
    }

    QStringList CGuiUtility::closeAllModalWidgetsGetTitles()
    {
        const QWidgetList modals = getAllModallWidgets();
        QStringList titles;
        for (QWidget *w : modals)
        {
            if (!w) { continue; }
            titles << w->windowTitle();
            w->close();
        }
        return titles;
    }

    QList<QDockWidget *> CGuiUtility::closeAllDockWidgets(QWidget *parent, bool floatingOnly)
    {
        QList<QDockWidget *> dws = getAllDockWidgets(parent, floatingOnly);
        for (QWidget *w : dws)
        {
            if (!w) { continue; }
            w->close();
        }
        return dws;
    }

    QList<QDockWidget *> CGuiUtility::closeAllDockWidgets(QWindow *parent, bool floatingOnly)
    {
        QList<QDockWidget *> dws = getAllDockWidgets(parent, floatingOnly);
        for (QWidget *w : dws)
        {
            if (!w) { continue; }
            w->close();
        }
        return dws;
    }

    QStringList CGuiUtility::closeAllDockWidgetsGetTitles(QWidget *parent, bool floatingOnly)
    {
        const QList<QDockWidget *> dws = getAllDockWidgets(parent, floatingOnly);
        QStringList titles;
        for (QWidget *w : dws)
        {
            if (!w) { continue; }
            titles << w->windowTitle();
            w->close();
        }
        return titles;
    }

    QStringList CGuiUtility::closeAllDockWidgetsGetTitles(QWindow *parent, bool floatingOnly)
    {
        const QList<QDockWidget *> dws = getAllDockWidgets(parent, floatingOnly);
        QStringList titles;
        for (QWidget *w : dws)
        {
            if (!w) { continue; }
            titles << w->windowTitle();
            w->close();
        }
        return titles;
    }

    QStringList CGuiUtility::deleteLaterAllDockWidgetsGetTitles(QWidget *parent, bool floatingOnly)
    {
        const QList<QDockWidget *> dws = getAllDockWidgets(parent, floatingOnly);
        QStringList titles;
        for (QWidget *w : dws)
        {
            if (!w) { continue; }
            titles << w->windowTitle();
            w->deleteLater(); // DANGEROUS
        }
        return titles;
    }

} // ns

// SPDX-FileCopyrightText: Copyright (C) 2013 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

#include "blackconfig/buildconfig.h"
#include "blackgui/stylesheetutility.h"
#include "blackmisc/fileutils.h"
#include "blackmisc/logmessage.h"
#include "blackmisc/swiftdirectories.h"
#include "blackmisc/directoryutils.h"

#include <QAbstractScrollArea>
#include <QCoreApplication>
#include <QDir>
#include <QFile>
#include <QFileInfo>
#include <QFileInfoList>
#include <QFlags>
#include <QFont>
#include <QIODevice>
#include <QRegularExpression>
#include <QStyleOption>
#include <QStylePainter>
#include <QTextStream>
#include <QWidget>
#include <QtGlobal>
#include <QStringBuilder>

using namespace BlackConfig;
using namespace BlackMisc;

namespace BlackGui
{
    CStyleSheetUtility::CStyleSheetUtility(QObject *parent) : QObject(parent)
    {
        this->read();
        connect(&m_fileWatcher, &QFileSystemWatcher::directoryChanged, this, &CStyleSheetUtility::qssDirectoryChanged);
        connect(&m_fileWatcher, &QFileSystemWatcher::fileChanged, this, &CStyleSheetUtility::qssDirectoryChanged);
    }

    const QStringList &CStyleSheetUtility::getLogCategories()
    {
        static const QStringList cats { CLogCategories::guiComponent() };
        return cats;
    }

    const QString &CStyleSheetUtility::fontStyleAsString(const QFont &font)
    {
        static const QString n("normal");
        static const QString i("italic");
        static const QString o("oblique");
        static const QString e;

        switch (font.style())
        {
        case QFont::StyleNormal: return n;
        case QFont::StyleItalic: return i;
        case QFont::StyleOblique: return o;
        default: return e;
        }
    }

    const QString &CStyleSheetUtility::fontWeightAsString(const QFont &font)
    {
        if (font.weight() < static_cast<int>(QFont::Normal))
        {
            static const QString l("light");
            return l;
        }
        else if (font.weight() < static_cast<int>(QFont::DemiBold))
        {
            static const QString n("normal");
            return n;
        }
        else if (font.weight() < static_cast<int>(QFont::Bold))
        {
            static const QString d("demibold");
            return d;
        }
        else if (font.weight() < static_cast<int>(QFont::Black))
        {
            static const QString b("bold");
            return b;
        }
        else
        {
            static const QString b("black");
            return b;
        }
    }

    QString CStyleSheetUtility::fontAsCombinedWeightStyle(const QFont &font)
    {
        QString w = fontWeightAsString(font);
        QString s = fontStyleAsString(font);
        if (w == s) return w; // avoid "normal" "normal"
        if (w.isEmpty() && s.isEmpty()) return "normal";
        if (w.isEmpty()) return s;
        if (s.isEmpty()) return w;
        if (s == "normal") return w;
        return w.append(" ").append(s);
    }

    QString CStyleSheetUtility::asStylesheet(const QString &fontFamily, const QString &fontSize, const QString &fontStyle, const QString &fontWeight, const QString &fontColor)
    {
        static const QString indent("     ");
        static const QString lf("\n");
        static const QString fontStyleSheet("%1font-family: \"%3\";%2%1font-size: %4;%2%1font-style: %5;%2%1font-weight: %6;%2%1color: %7;%2");
        static const QString fontStyleSheetNoColor("%1font-family: \"%3\";%2%1font-size: %4;%2%1font-style: %5;%2%1font-weight: %6;%2");

        return fontColor.isEmpty() ?
                   fontStyleSheetNoColor.arg(indent, lf, fontFamily, fontSize, fontStyle, fontWeight) :
                   fontStyleSheet.arg(indent, lf, fontFamily, fontSize, fontStyle, fontWeight, fontColor);
    }

    QString CStyleSheetUtility::asStylesheet(const QWidget *widget, int pointSize)
    {
        Q_ASSERT_X(widget, Q_FUNC_INFO, "Missing widget");
        const QFont f = widget->font();
        return CStyleSheetUtility::asStylesheet(
            f.family(),
            QStringLiteral("%1pt").arg(pointSize < 0 ? f.pointSize() : pointSize),
            CStyleSheetUtility::fontStyleAsString(f),
            CStyleSheetUtility::fontWeightAsString(f));
    }

    QString CStyleSheetUtility::fontColorString() const
    {
        const QString s = this->style(fileNameFonts()).toLower();
        if (!s.contains("color:")) return "";
        thread_local const QRegularExpression rx("color:\\s*(#*\\w+);");
        const QString c = rx.match(s).captured(1);
        return c.isEmpty() ? "" : c;
    }

    bool CStyleSheetUtility::read()
    {
        QDir directory(CSwiftDirectories::stylesheetsDirectory());
        if (!directory.exists()) { return false; }

        // qss/css files
        const bool needsWatcher = m_fileWatcher.files().isEmpty();
        if (needsWatcher) { m_fileWatcher.addPath(CSwiftDirectories::stylesheetsDirectory()); } // directory to deleted file watching
        directory.setNameFilters({ "*.qss", "*.css" });
        directory.setFilter(QDir::Files | QDir::Hidden | QDir::NoSymLinks);

        QMap<QString, QString> newStyleSheets;
        const QFileInfoList fileInfoList = directory.entryInfoList();

        // here we generate the style sheets
        for (const QFileInfo &fileInfo : fileInfoList)
        {
            const QString absolutePath = fileInfo.absoluteFilePath();
            QFile file(absolutePath);
            if (file.open(QFile::QIODevice::ReadOnly | QIODevice::Text))
            {
                if (needsWatcher) { m_fileWatcher.addPath(absolutePath); }
                QTextStream in(&file);
                const QString c = removeComments(in.readAll(), true, true);
                const QString f = fileInfo.fileName().toLower();

                // save files for debugging
                if (CBuildConfig::isLocalDeveloperDebugBuild())
                {
                    const QString fn = CFileUtils::appendFilePaths(CSwiftDirectories::logDirectory(), f);
                    CFileUtils::writeStringToFile(c, fn);
                }

                // keep even empty files as placeholders
                newStyleSheets.insert(f, c); // set an empty string here to disable all stylesheet
            }
            file.close();
        }

        // ignore redundant re-reads
        if (newStyleSheets != m_styleSheets)
        {
            m_styleSheets = newStyleSheets;
            emit this->styleSheetsChanged();
        }
        return true;
    }

    QString CStyleSheetUtility::style(const QString &fileName) const
    {
        if (!this->containsStyle(fileName)) { return QString(); }
        return m_styleSheets[fileName.toLower()].trimmed();
    }

    QString CStyleSheetUtility::styles(const QStringList &fileNames) const
    {
        const bool hasModifiedFont = this->containsStyle(fileNameFontsModified());
        bool fontAdded = false;

        QString style;
        for (const QString &fileName : fileNames)
        {
            const QString key = fileName.toLower().trimmed();
            if (!this->containsStyle(key)) { continue; }

            QString s;
            if (fileName == fileNameFonts() || fileName == fileNameFontsModified())
            {
                if (fontAdded) { continue; }
                fontAdded = true;
                s = hasModifiedFont ?
                        m_styleSheets[fileNameFontsModified().toLower()] :
                        m_styleSheets[fileNameFonts()];
            }
            else
            {
                s = m_styleSheets[key];
            }
            if (s.isEmpty()) { continue; }

            style +=
                (style.isEmpty() ? QString() : "\n\n") %
                u"/** file: " % fileName % " **/\n" %
                s;
        }
        return style;
    }

    bool CStyleSheetUtility::containsStyle(const QString &fileName) const
    {
        if (fileName.isEmpty()) return false;
        return m_styleSheets.contains(fileName.toLower().trimmed());
    }

    bool CStyleSheetUtility::updateFont(const QFont &font)
    {
        QString fs;
        if (font.pixelSize() >= 0)
        {
            fs.append(QString::number(font.pixelSize())).append("px");
        }
        else
        {
            fs.append(QString::number(font.pointSizeF())).append("pt");
        }
        return updateFont(font.family(), fs, fontStyleAsString(font), fontWeightAsString(font), "white");
    }

    bool CStyleSheetUtility::updateFont(const QString &fontFamily, const QString &fontSize, const QString &fontStyle, const QString &fontWeight, const QString &fontColor)
    {
        const QString qss = CStyleSheetUtility::asStylesheet(fontFamily, fontSize, fontStyle, fontWeight, fontColor);
        return CStyleSheetUtility::updateFont(qss);
    }

    bool CStyleSheetUtility::updateFont(const QString &qss)
    {
        const QString qssWidget(u"QWidget {\n" % qss % u"}\n");
        const QString fn = CFileUtils::appendFilePaths(CSwiftDirectories::stylesheetsDirectory(), fileNameFontsModified());
        QFile fontFile(fn);
        bool ok = fontFile.open(QFile::Text | QFile::WriteOnly);
        if (ok)
        {
            QTextStream out(&fontFile);
            out << qssWidget;
            fontFile.close();
            ok = this->read();
        }
        else
        {
            CLogMessage(static_cast<CStyleSheetUtility *>(nullptr)).warning(u"Cannot open file '%1' for writing") << fn;
        }
        return ok;
    }

    bool CStyleSheetUtility::resetFont()
    {
        QFile fontFile(CFileUtils::appendFilePaths(CSwiftDirectories::stylesheetsDirectory(), fileNameFontsModified()));
        return fontFile.remove();
    }

    QString CStyleSheetUtility::fontStyle(const QString &combinedStyleAndWeight)
    {
        static const QString n("normal");
        const QString c = combinedStyleAndWeight.toLower();
        for (const QString &s : fontStyles())
        {
            if (c.contains(s))
            {
                return s;
            }
        }
        return n;
    }

    QString CStyleSheetUtility::fontWeight(const QString &combinedStyleAndWeight)
    {
        static const QString n("normal");
        const QString c = combinedStyleAndWeight.toLower();
        for (const QString &w : fontWeights())
        {
            if (c.contains(w)) { return w; }
        }
        return n;
    }

    const QString &CStyleSheetUtility::fileNameFonts()
    {
        static const QString f(getQssFileName("fonts"));
        return f;
    }

    const QString &CStyleSheetUtility::fileNameFontsModified()
    {
        static const QString f("fonts.modified.qss");
        return f;
    }

    bool CStyleSheetUtility::deleteModifiedFontFile()
    {
        const QString fn = CFileUtils::appendFilePaths(CSwiftDirectories::stylesheetsDirectory(), fileNameFontsModified());
        QFile file(fn);
        if (!file.exists()) { return false; }
        bool r = file.remove();
        if (!r) { return false; }
        this->read();
        return true;
    }

    const QString &CStyleSheetUtility::fileNameSwiftStandardGui()
    {
        static const QString f(getQssFileName("swiftstdgui"));
        return f;
    }

    const QString &CStyleSheetUtility::fileNameAndPathSwiftStandardGui()
    {
        static const QString fn = CFileUtils::appendFilePaths(CSwiftDirectories::stylesheetsDirectory(), CStyleSheetUtility::fileNameSwiftStandardGui());
        return fn;
    }

    const QString &CStyleSheetUtility::fileNameInfoBar()
    {
        static const QString f(getQssFileName("infobar"));
        return f;
    }

    const QString &CStyleSheetUtility::fileNameNavigator()
    {
        static const QString f(getQssFileName("navigator"));
        return f;
    }

    const QString &CStyleSheetUtility::fileNameDockWidgetTab()
    {
        static const QString f(getQssFileName("dockwidgettab"));
        return f;
    }

    const QString &CStyleSheetUtility::fileNameStandardWidget()
    {
        static const QString f(getQssFileName("stdwidget"));
        return f;
    }

    const QString &CStyleSheetUtility::fileNameAndPathStandardWidget()
    {
        static const QString fn = CFileUtils::appendFilePaths(CSwiftDirectories::stylesheetsDirectory(), CStyleSheetUtility::fileNameStandardWidget());
        return fn;
    }

    const QString &CStyleSheetUtility::fileNameTextMessage()
    {
        static const QString f("textmessage.css");
        return f;
    }

    const QString &CStyleSheetUtility::fileNameFilterDialog()
    {
        static const QString f(getQssFileName("filterdialog"));
        return f;
    }

    const QString &CStyleSheetUtility::fileNameSwiftCore()
    {
        static const QString f(getQssFileName("swiftcore"));
        return f;
    }

    const QString &CStyleSheetUtility::fileNameSwiftData()
    {
        static const QString f(getQssFileName("swiftdata"));
        return f;
    }

    const QString &CStyleSheetUtility::fileNameSwiftLauncher()
    {
        static const QString f(getQssFileName("swiftlauncher"));
        return f;
    }

    const QStringList &CStyleSheetUtility::fontWeights()
    {
        static const QStringList w({ "bold", "semibold", "light", "black", "normal" });
        return w;
    }

    const QStringList &CStyleSheetUtility::fontStyles()
    {
        static const QStringList s({ "italic", "oblique", "normal" });
        return s;
    }

    const QString &CStyleSheetUtility::transparentBackgroundColor()
    {
        static const QString t = "background-color: transparent;";
        return t;
    }

    bool CStyleSheetUtility::useStyleSheetInDerivedWidget(QWidget *usedWidget, QStyle::PrimitiveElement element)
    {
        Q_ASSERT(usedWidget);
        if (!usedWidget) { return false; }

        Q_ASSERT(usedWidget->style());
        QStyle *style = usedWidget->style();
        if (!style) { return false; }

        // 1) QStylePainter: modern version of
        //    usedWidget->style()->drawPrimitive(element, &opt, &p, usedWidget);
        // 2) With viewport based widgets viewport has to be used
        // see http://stackoverflow.com/questions/37952348/enable-own-widget-for-stylesheet
        QAbstractScrollArea *sa = qobject_cast<QAbstractScrollArea *>(usedWidget);
        QStylePainter p(
            sa ? sa->viewport() :
                 usedWidget);
        if (!p.isActive()) { return false; }

        QStyleOption opt;
        opt.initFrom(usedWidget);
        p.drawPrimitive(element, opt);
        return true;
    }

    QString CStyleSheetUtility::styleForIconCheckBox(const QString &checkedIcon, const QString &uncheckedIcon, const QString &width, const QString &height)
    {
        Q_ASSERT(!checkedIcon.isEmpty());
        Q_ASSERT(!uncheckedIcon.isEmpty());

        static const QString st = "QCheckBox::indicator { width: %1; height: %2; } QCheckBox::indicator:checked { image: url(%3); } QCheckBox::indicator:unchecked { image: url(%4); }";
        return st.arg(width, height, checkedIcon, uncheckedIcon);
    }

    QString CStyleSheetUtility::concatStyles(const QString &style1, const QString &style2)
    {
        QString s1(style1.trimmed());
        QString s2(style2.trimmed());
        if (s1.isEmpty()) { return s2; }
        if (s2.isEmpty()) { return s1; }
        if (!s1.endsWith(";")) { s1 = s1.append("; "); }
        s1.append(s2);
        if (!s1.endsWith(";")) { s1 = s1.append(";"); }
        return s1;
    }

    void CStyleSheetUtility::setQSysInfoProperties(QWidget *widget, bool withChildWidgets)
    {
        Q_ASSERT_X(widget, Q_FUNC_INFO, "Missing widget");
        if (!widget->property("qsysKernelType").isValid())
        {
            widget->setProperty("qsysKernelType", QSysInfo::kernelType());
            widget->setProperty("qsysCurrentCpuArchitecture", QSysInfo::currentCpuArchitecture());
            widget->setProperty("qsysBuildCpuArchitecture", QSysInfo::buildCpuArchitecture());
            widget->setProperty("qsysProductType", QSysInfo::productType());
        }

        if (withChildWidgets)
        {
            for (QWidget *w : widget->findChildren<QWidget *>(QString(), Qt::FindDirectChildrenOnly))
            {
                CStyleSheetUtility::setQSysInfoProperties(w, true);
            }
        }
    }

    void CStyleSheetUtility::qssDirectoryChanged(const QString &file)
    {
        Q_UNUSED(file);
        this->read();
    }

    QString CStyleSheetUtility::getQssFileName(const QString &fileName)
    {
        static const QString qss(".qss");
        QString fn(fileName);
        if (fn.endsWith(qss)) { fn.chop(qss.length()); }

        QString specific;
        if (CBuildConfig::isRunningOnWindowsNtPlatform())
        {
            specific = fn % u".win" % qss;
        }
        else if (CBuildConfig::isRunningOnMacOSPlatform())
        {
            specific = fn % u".mac" % qss;
        }
        else if (CBuildConfig::isRunningOnLinuxPlatform())
        {
            specific = fn % u".linux" % qss;
        }
        return qssFileExists(specific) ? specific : fn + qss;
    }

    bool CStyleSheetUtility::qssFileExists(const QString &filename)
    {
        if (filename.isEmpty()) { return false; }
        const QFileInfo f(CFileUtils::appendFilePaths(CSwiftDirectories::stylesheetsDirectory(), filename));
        return f.exists() && f.isReadable();
    }
} // ns

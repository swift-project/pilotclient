/* Copyright (C) 2013
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

#include "stylesheetutility.h"
#include <QCoreApplication>
#include <QDir>
#include <QTextStream>
#include <QDebug>
#include <QRegExp>
#include <QStyleOption>
#include <QPainter>

namespace BlackGui
{

    CStyleSheetUtility::CStyleSheetUtility(QObject *parent) : QObject(parent)
    {
        this->read();
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

    QString CStyleSheetUtility::fontColor()
    {
        QString s = this->style(fileNameFonts()).toLower();
        if (!s.contains("color:")) return "red";
        QRegExp rx("color:\\s*(#*\\w+);");
        rx.indexIn(s);
        QString c = rx.cap(1);
        return c.isEmpty() ? "red" : c;
    }

    bool CStyleSheetUtility::read()
    {
        QDir directory(qssDirectory());
        if (!directory.exists()) return false;
        directory.setNameFilters({"*.qss"});
        directory.setFilter(QDir::Files | QDir::Hidden | QDir::NoSymLinks);

        QFileInfoList fileInfoList = directory.entryInfoList();

        for (int i = 0; i < fileInfoList.size(); ++i)
        {
            QFileInfo fileInfo = fileInfoList.at(i);
            QFile file(fileInfo.absoluteFilePath());
            if (file.open(QFile::QIODevice::ReadOnly | QIODevice::Text))
            {
                QTextStream in(&file);
                QString c = in.readAll();
                QString f = fileInfo.fileName().toLower();

                // keep even empty files as placeholders
                this->m_styleSheets.insert(f, c);
            }
            file.close();
        }
        emit this->styleSheetsChanged();
        return true;
    }

    QString CStyleSheetUtility::style(const QString &fileName) const
    {
        if (!this->containsStyle(fileName)) return QString();
        return this->m_styleSheets[fileName.toLower()].trimmed();
    }

    QString CStyleSheetUtility::styles(const QStringList &fileNames) const
    {
        QString style;
        foreach(QString fileName, fileNames)
        {
            if (!this->containsStyle(fileName)) continue;
            QString s = this->m_styleSheets[fileName.toLower()].trimmed();
            if (s.isEmpty()) continue;
            if (!style.isEmpty()) style.append("\n\n");
            style.append("/** file: ").append(fileName).append(" **/\n");
            style.append(s);
        }
        return style;
    }

    bool CStyleSheetUtility::containsStyle(const QString &fileName) const
    {
        if (fileName.isEmpty()) return false;
        return this->m_styleSheets.contains(fileName.toLower());
    }

    bool CStyleSheetUtility::updateFonts(const QFont &font)
    {
        QString fs;
        if (font.pixelSize() >= 0)
        {
            fs.append(font.pixelSize()).append("px");
        }
        else
        {
            fs.append(QString::number(font.pointSizeF())).append("pt");
        }
        return updateFonts(font.family(), fs, fontStyleAsString(font), fontWeightAsString(font), "white");
    }

    bool CStyleSheetUtility::updateFonts(const QString &fontFamily, const QString &fontSize, const QString &fontStyle, const QString &fontWeight, const QString &fontColor)
    {
        const QString indent("     ");
        QString fontStyleSheet;
        fontStyleSheet.append(indent).append("font-family: \"").append(fontFamily).append("\";\n");
        fontStyleSheet.append(indent).append("font-size: ").append(fontSize).append(";\n");
        fontStyleSheet.append(indent).append("font-style: ").append(fontStyle).append(";\n");
        fontStyleSheet.append(indent).append("font-weight: ").append(fontWeight).append(";\n");
        fontStyleSheet.append(indent).append("color: ").append(fontColor).append(";\n");

        QString qss("QWidget {\n");
        qss.append(fontStyleSheet);
        qss.append("}\n");

        QFile fontFile(qssDirectory().append("/").append(fileNameFonts()));
        bool ok = fontFile.open(QFile::Text | QFile::WriteOnly);
        if (ok)
        {
            QTextStream out(&fontFile);
            out << qss;
            fontFile.close();
            ok = this->read();
        }
        return ok;
    }

    QString CStyleSheetUtility::fontStyle(const QString &combinedStyleAndWeight)
    {
        static const QString n("normal");
        QString c = combinedStyleAndWeight.toLower();
        foreach(QString s, fontStyles())
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
        QString c = combinedStyleAndWeight.toLower();
        foreach(QString w, fontWeights())
        {
            if (c.contains(w))
            {
                return w;
            }
        }
        return n;
    }

    QString CStyleSheetUtility::qssDirectory()
    {
        QString dirPath = QCoreApplication::applicationDirPath();
        if (!dirPath.endsWith('/')) dirPath.append('/');
        dirPath.append("qss");
        return dirPath;
    }

    void CStyleSheetUtility::useStyleSheetInDerivedWidget(QWidget *usedWidget, QStyle::PrimitiveElement element)
    {
        Q_ASSERT(usedWidget);
        if (!usedWidget) { return; }
        Q_ASSERT(usedWidget->style());
        if (!usedWidget->style()) { return; }
        QStyleOption opt;
        opt.initFrom(usedWidget);
        QPainter p(usedWidget);
        usedWidget->style()->drawPrimitive(element, &opt, &p, usedWidget);
    }

    QString CStyleSheetUtility::styleForIconCheckBox(const QString &checkedIcon, const QString &uncheckedIcon, const QString &width, const QString &height)
    {
        Q_ASSERT(!checkedIcon.isEmpty());
        Q_ASSERT(!uncheckedIcon.isEmpty());

        static const QString st = "QCheckBox::indicator { width: %1; height: %2; } QCheckBox::indicator:checked { image: url(%3); } QCheckBox::indicator:unchecked { image: url(%4); }";
        return st.arg(width).arg(height).arg(checkedIcon).arg(uncheckedIcon);
    }
}

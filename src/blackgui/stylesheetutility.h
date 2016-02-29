/* Copyright (C) 2014
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

//! \file

#ifndef BLACKGUI_STYLESHEETUTILITY_H
#define BLACKGUI_STYLESHEETUTILITY_H

#include "blackgui/blackguiexport.h"
#include <QMap>
#include <QObject>
#include <QFont>
#include <QStringList>
#include <QStyle>
#include <QScopedPointer>
#include <QSettings>

namespace BlackGui
{
    class CGuiApplication;

    //! Reads and provides style sheets
    class BLACKGUI_EXPORT CStyleSheetUtility : public QObject
    {
        Q_OBJECT
        friend class CGuiApplication;

    public:
        //! Style for given file name
        QString style(const QString &fileName) const;

        //! Multiple styles concatenated
        QString styles(const QStringList &fileNames) const;

        //! Contains style for name
        bool containsStyle(const QString &fileName) const;

        //! Update the fonts
        bool updateFonts(const QFont &font);

        //! Update the fonts
        bool updateFonts(const QString &fontFamily, const QString &fontSize, const QString &fontStyle, const QString &fontWeight, const QString &fontColor);

        //! Current font color from style sheet
        QString fontColor();

        //! GUI ini file data
        const QSettings *iniFile() const { return m_iniFile.data(); }

        //! Get the font style
        static QString fontStyle(const QString &combinedStyleAndWeight);

        //! Get the font weight
        static QString fontWeight(const QString &combinedStyleAndWeight);

        //! Central reader
        static CStyleSheetUtility &instance();

        //! File name fonts.qss
        static const QString &fileNameFonts();

        //! File name infobar.qss
        static const QString &fileNameInfoBar();

        //! File name navigator.qss
        static const QString &fileNameNavigator();

        //! File name dockwidgettab.qss
        static const QString &fileNameDockWidgetTab();

        //! File name for standard widgets
        static const QString &fileNameStandardWidget();

        //! File name textmessage.qss
        static const QString &fileNameTextMessage();

        //! File name maininfoarea.qss
        static const QString &fileNameFilterDialog();

        //! File name swift standard GUI
        static const QString &fileNameSwiftStandardGui();

        //! File name swiftcore.qss
        static const QString &fileNameSwiftCore();

        //! File name swiftcore.qss
        static const QString &fileNameSwiftData();

        //! File name swiftlauncher.qss
        static const QString &fileNameSwiftLauncher();

        //! File name ini file
        static const QString &fileNameIniFile();

        //! Font weights
        static const QStringList &fontWeights();

        //! Font styles
        static const QStringList &fontStyles();

        //! Transparent background color
        static const QString &transparentBackgroundColor();

        //! qss directory
        static QString qssDirectory();

        //! Font style as string
        static const QString &fontStyleAsString(const QFont &font);

        //! Font weight as string
        static const QString &fontWeightAsString(const QFont &font);

        //! Font as combined weight and style
        static QString fontAsCombinedWeightStyle(const QFont &font);

        //! Use style sheets in derived widgets
        //! \sa QWidget::paintEvent
        static bool useStyleSheetInDerivedWidget(QWidget *derivedWidget, QStyle::PrimitiveElement element = QStyle::PE_Widget);

        //! Stylesheet string for a checkbox displayed as 2 icons
        static QString styleForIconCheckBox(const QString &checkedIcon, const QString &uncheckedIcon, const QString &width = "16px", const QString &height = "16px");

        //! Concatenate 2 styles
        static QString concatStyles(const QString &style1, const QString &style2);

    signals:
        //! Sheets have been changed
        //! \deprecated use BlackGui::CGuiApplication::styleSheetsChanged
        void styleSheetsChanged();

    private:
        //! Read the *.qss files
        bool read();

        QMap<QString, QString> m_styleSheets; //!< filename, stylesheet
        QScopedPointer<QSettings> m_iniFile;

        //! Constructor
        explicit CStyleSheetUtility(QObject *parent = nullptr);
    };

}
#endif // guard

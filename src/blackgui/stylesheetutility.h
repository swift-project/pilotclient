/* Copyright (C) 2014
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution. No part of swift project, including this file, may be copied, modified, propagated,
 * or distributed except according to the terms contained in the LICENSE file.
 */

//! \file

#ifndef BLACKGUI_STYLESHEETUTILITY_H
#define BLACKGUI_STYLESHEETUTILITY_H

#include "blackgui/blackguiexport.h"
#include "blackmisc/logcategories.h"
#include "blackmisc/restricted.h"
#include "blackmisc/digestsignal.h"

#include <QFileSystemWatcher>
#include <QMap>
#include <QObject>
#include <QScopedPointer>
#include <QSettings>
#include <QString>
#include <QStringList>
#include <QStyle>

class QFont;
class QWidget;

namespace BlackGui
{
    class CGuiApplication;

    //! Reads and provides style sheets
    class BLACKGUI_EXPORT CStyleSheetUtility : public QObject
    {
        Q_OBJECT

    public:
        //! Constructor
        explicit CStyleSheetUtility(BlackMisc::Restricted<CGuiApplication>, QObject *parent = nullptr);

        //! Log cats.
        static const QStringList &getLogCategories();

        //! Style for given file name
        QString style(const QString &fileName) const;

        //! Multiple styles concatenated
        QString styles(const QStringList &fileNames) const;

        //! Contains style for name
        bool containsStyle(const QString &fileName) const;

        //! Update the fonts
        bool updateFont(const QFont &font);

        //! Update the fonts
        bool updateFont(const QString &qss);

        //! Update the fonts
        bool updateFont(const QString &fontFamily, const QString &fontSize, const QString &fontStyle, const QString &fontWeight, const QString &fontColorString);

        //! Reset font
        bool resetFont();

        //! Current font color from style sheet
        QString fontColorString() const;

        //! Read the *.qss files
        bool read();

        //! Get the font style
        static QString fontStyle(const QString &combinedStyleAndWeight);

        //! Get the font weight
        static QString fontWeight(const QString &combinedStyleAndWeight);

        //! File name fonts.qss
        static const QString &fileNameFonts();

        //! Name for user modified file
        static const QString &fileNameFontsModified();

        //! Delete the modified file for fonts
        bool deleteModifiedFontFile();

        //! File name infobar.qss
        static const QString &fileNameInfoBar();

        //! File name navigator.qss
        static const QString &fileNameNavigator();

        //! File name dockwidgettab.qss
        static const QString &fileNameDockWidgetTab();

        //! File name for standard widgets
        static const QString &fileNameStandardWidget();

        //! Full file path and name for standard widgets
        static const QString &fileNameAndPathStandardWidget();

        //! File name textmessage.qss
        static const QString &fileNameTextMessage();

        //! File name maininfoarea.qss
        static const QString &fileNameFilterDialog();

        //! File name swift standard GUI
        static const QString &fileNameSwiftStandardGui();

        //! Full file path and name for swift standard GUI
        static const QString &fileNameAndPathSwiftStandardGui();

        //! File name swiftcore.qss
        static const QString &fileNameSwiftCore();

        //! File name swiftcore.qss
        static const QString &fileNameSwiftData();

        //! File name swiftlauncher.qss
        static const QString &fileNameSwiftLauncher();

        //! Font weights
        static const QStringList &fontWeights();

        //! Font styles
        static const QStringList &fontStyles();

        //! Transparent background color
        static const QString &transparentBackgroundColor();

        //! Font style as string
        static const QString &fontStyleAsString(const QFont &font);

        //! Font weight as string
        static const QString &fontWeightAsString(const QFont &font);

        //! Font as combined weight and style
        static QString fontAsCombinedWeightStyle(const QFont &font);

        //! Parameters as stylesheet
        static QString asStylesheet(const QString &fontFamily, const QString &fontSize, const QString &fontStyle,
                                    const QString &fontWeight, const QString &fontColorString = {});

        //! Widget's font as stylesheet
        static QString asStylesheet(const QWidget *widget, int pointSize = -1);

        //! Use style sheets in derived widgets
        //! \sa QWidget::paintEvent
        static bool useStyleSheetInDerivedWidget(QWidget *derivedWidget, QStyle::PrimitiveElement element = QStyle::PE_Widget);

        //! Stylesheet string for a checkbox displayed as 2 icons
        static QString styleForIconCheckBox(const QString &checkedIcon, const QString &uncheckedIcon, const QString &width = "16px", const QString &height = "16px");

        //! Concatenate 2 styles
        static QString concatStyles(const QString &style1, const QString &style2);

        //! Set QSysInfo properties for given widget (which can be used in stylesheet)
        static void setQSysInfoProperties(QWidget *widget, bool withChildWidgets);

    signals:
        //! Sheets have been changed
        //! \deprecated use BlackGui::CGuiApplication::styleSheetsChanged
        void styleSheetsChanged();

    private:
        //! File changed
        void qssDirectoryChanged(const QString &file);

        //! Filename based on OS/existing files
        static QString getQssFileName(const QString &fileName);

        //! Check existance of qss file
        static bool qssFileExists(const QString &filename);

        QMap<QString, QString> m_styleSheets; //!< filename, stylesheet
        QFileSystemWatcher m_fileWatcher { this }; //!< monitor my qss files
    };
}
#endif // guard

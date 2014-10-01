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

#include <QMap>
#include <QObject>
#include <QFont>
#include <QStringList>

namespace BlackGui
{
    //! Reads and provides style sheets
    class CStyleSheetUtility : public QObject
    {
        Q_OBJECT

    public:

        //! Read the *.qss files
        bool read();

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

        //! Get the font style
        static QString fontStyle(const QString &combinedStyleAndWeight);

        //! Get the font weight
        static QString fontWeight(const QString &combinedStyleAndWeight);

        //! Central reader
        static CStyleSheetUtility &instance()
        {
            static CStyleSheetUtility r;
            return r;
        }

        //! File name fonts.qss
        static const QString &fileNameFonts()
        {
            static const QString f("fonts.qss");
            return f;
        }

        //! File name mainwindow.qss
        static const QString &fileNameMainWindow()
        {
            static const QString f("mainwindow.qss");
            return f;
        }

        //! File name infobar.qss
        static const QString &fileNameInfoBar()
        {
            static const QString f("infobar.qss");
            return f;
        }

        //! File name dockwidgettab.qss
        static const QString &fileNameDockWidgetTab()
        {
            static const QString f("dockwidgettab.qss");
            return f;
        }

        //! File name maininfoarea.qss
        static const QString &fileNameInfoWindow()
        {
            static const QString f("stylesheetinfo.qss");
            return f;
        }

        //! Font weights
        static const QStringList &fontWeights()
        {
            static const QStringList w( {"bold", "semibold", "light", "black", "normal"});
            return w;
        }

        //! Font styles
        static const QStringList &fontStyles()
        {
            static const QStringList s( {"italic", "oblique", "normal"});
            return s;
        }

        //! Transparent background color
        static const QString &transparentBackgroundColor()
        {
            static const QString t = "background-color: transparent;";
            return t;
        }

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
        static void useStyleSheetInDerivedWidget(QWidget *derivedWidget);

    signals:
        //! Sheets have been changed
        void styleSheetsChanged();

    private:
        QMap<QString, QString> m_styleSheets; //!< filename, stylesheet

        //! Constructor
        explicit CStyleSheetUtility(QObject *parent = nullptr);
    };

}
#endif // guard

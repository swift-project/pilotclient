/* Copyright (C) 2014
 * swift Project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of Swift Project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

#ifndef BLACKGUI_STYLESHEETUTILITY_H
#define BLACKGUI_STYLESHEETUTILITY_H

//! \file

#include <QMap>
#include <QObject>
#include <QFont>

namespace BlackGui
{
    //! Reads and provides style sheets
    class CStyleSheetUtility : public QObject
    {
        Q_OBJECT

    public:

        //! Read the *.qss files
        bool read();

        //! Style for file name
        QString style(const QString &fileName) const;

        //! Multiple styles concatenated
        QString styles(const QStringList &fileNames) const;

        //! Contains style for name
        bool containsStyle(const QString &fileName) const;

        //! Update the fonts
        bool updateFonts(const QFont &font);

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
        static const QString &fileNameMainInfoArea()
        {
            static const QString f("maininfoarea.qss");
            return f;
        }

        //! qss directory
        static QString qssDirectory();

    signals:
        //! Sheets have been changed
        void styleSheetsChanged();

    private:
        QMap<QString, QString> m_styleSheets; //!< filename, stylesheet

        //! Constructor
        explicit CStyleSheetUtility(QObject *parent = nullptr);

        //! Font style as string
        static const QString &fontStyleAsString(const QFont &font);

        //! Font weight as string
        static const QString &fontWeightAsString(const QFont &font);
    };

}
#endif // guard

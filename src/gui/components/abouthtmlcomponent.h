// SPDX-FileCopyrightText: Copyright (C) 2018 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

//! \file

#ifndef SWIFT_GUI_COMPONENTS_ABOUTHTMLCOMPONENT_H
#define SWIFT_GUI_COMPONENTS_ABOUTHTMLCOMPONENT_H

#include <QFrame>
#include <QScopedPointer>
#include <QUrl>

#include "gui/swiftguiexport.h"

namespace Ui
{
    class CAboutHtmlComponent;
}
namespace swift::gui::components
{
    /*!
     * Display the HTML info "about swift"
     */
    class SWIFT_GUI_EXPORT CAboutHtmlComponent : public QFrame
    {
        Q_OBJECT

    public:
        //! Constructor
        explicit CAboutHtmlComponent(QWidget *parent = nullptr);

        //! Destructor
        virtual ~CAboutHtmlComponent() override;

    private:
        //! Load credits and legal info
        void loadAbout();

        //! Anchor has been clicked
        void onAnchorClicked(const QUrl &url);

        QScopedPointer<Ui::CAboutHtmlComponent> ui;
    };
} // namespace swift::gui::components

#endif // SWIFT_GUI_COMPONENTS_ABOUTHTMLCOMPONENT_H

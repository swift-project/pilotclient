// SPDX-FileCopyrightText: Copyright (C) 2018 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

//! \file

#ifndef BLACKGUI_COMPONENTS_ABOUTHTMLCOMPONENT_H
#define BLACKGUI_COMPONENTS_ABOUTHTMLCOMPONENT_H

#include "blackgui/blackguiexport.h"
#include <QUrl>
#include <QFrame>
#include <QScopedPointer>

namespace Ui
{
    class CAboutHtmlComponent;
}
namespace BlackGui::Components
{
    /*!
     * Display the HTML info "about swift"
     */
    class BLACKGUI_EXPORT CAboutHtmlComponent : public QFrame
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
} // ns

#endif // guard

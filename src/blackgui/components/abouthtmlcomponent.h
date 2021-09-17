/* Copyright (C) 2018
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution. No part of swift project, including this file, may be copied, modified, propagated,
 * or distributed except according to the terms contained in the LICENSE file.
 */

//! \file

#ifndef BLACKGUI_COMPONENTS_ABOUTHTMLCOMPONENT_H
#define BLACKGUI_COMPONENTS_ABOUTHTMLCOMPONENT_H

#include "blackgui/blackguiexport.h"
#include <QUrl>
#include <QFrame>
#include <QScopedPointer>

namespace Ui { class CAboutHtmlComponent; }
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

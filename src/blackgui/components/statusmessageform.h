// SPDX-FileCopyrightText: Copyright (C) 2015 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

//! \file

#ifndef BLACKGUI_COMPONENTS_STATUSMESSAGEFORM_H
#define BLACKGUI_COMPONENTS_STATUSMESSAGEFORM_H

#include "blackgui/blackguiexport.h"
#include "blackmisc/statusmessage.h"
#include "blackmisc/variant.h"

#include <QFrame>
#include <QObject>
#include <QScopedPointer>

class QWidget;

namespace Ui
{
    class CStatusMessageForm;
}
namespace BlackGui::Components
{
    /*!
     * Display details about a single status message
     */
    class BLACKGUI_EXPORT CStatusMessageForm : public QFrame
    {
        Q_OBJECT

    public:
        //! Constructor
        explicit CStatusMessageForm(QWidget *parent = nullptr);

        //! Destructor
        virtual ~CStatusMessageForm();

        //! Set message
        void setVariant(const BlackMisc::CVariant &messageVariant);

        //! Set message
        void setValue(const BlackMisc::CStatusMessage &message);

        //! Clear
        void clear();

        //! Toggle visibility
        void toggleVisibility();

        //! Minimize by hiding status ...
        void setReducedInfo(bool reduced);

    private:
        QScopedPointer<Ui::CStatusMessageForm> ui;
    };
} // ns
#endif // guard

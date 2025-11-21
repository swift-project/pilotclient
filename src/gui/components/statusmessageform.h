// SPDX-FileCopyrightText: Copyright (C) 2015 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

//! \file

#ifndef SWIFT_GUI_COMPONENTS_STATUSMESSAGEFORM_H
#define SWIFT_GUI_COMPONENTS_STATUSMESSAGEFORM_H

#include <QFrame>
#include <QObject>
#include <QScopedPointer>

#include "gui/swiftguiexport.h"
#include "misc/statusmessage.h"
#include "misc/variant.h"

class QWidget;

namespace Ui
{
    class CStatusMessageForm;
}
namespace swift::gui::components
{
    /*!
     * Display details about a single status message
     */
    class SWIFT_GUI_EXPORT CStatusMessageForm : public QFrame
    {
        Q_OBJECT

    public:
        //! Constructor
        explicit CStatusMessageForm(QWidget *parent = nullptr);

        //! Destructor
        ~CStatusMessageForm() override;

        //! Set message
        void setVariant(const swift::misc::CVariant &messageVariant);

        //! Set message
        void setValue(const swift::misc::CStatusMessage &message);

        //! Clear
        void clear();

        //! Toggle visibility
        void toggleVisibility();

        //! Minimize by hiding status ...
        void setReducedInfo(bool reduced);

    private:
        QScopedPointer<Ui::CStatusMessageForm> ui;
    };
} // namespace swift::gui::components
#endif // SWIFT_GUI_COMPONENTS_STATUSMESSAGEFORM_H

// SPDX-FileCopyrightText: Copyright (C) 2015 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

//! \file

#ifndef SWIFT_GUI_COMPONENTS_STATUSMESSAGEFORMSMALL_H
#define SWIFT_GUI_COMPONENTS_STATUSMESSAGEFORMSMALL_H

#include <QFrame>
#include <QObject>
#include <QScopedPointer>

#include "gui/swiftguiexport.h"
#include "misc/statusmessage.h"
#include "misc/variant.h"

namespace Ui
{
    class CStatusMessageFormSmall;
}
namespace swift::gui::components
{
    /*!
     * Display details about a single status message
     */
    class SWIFT_GUI_EXPORT CStatusMessageFormSmall : public QFrame
    {
        Q_OBJECT

    public:
        //! Constructor
        explicit CStatusMessageFormSmall(QWidget *parent = nullptr);

        //! Destructor
        ~CStatusMessageFormSmall() override;

        //! Set message
        void setVariant(const swift::misc::CVariant &messageVariant);

        //! Set message
        void setValue(const swift::misc::CStatusMessage &message);

        //! Toggle visibility
        void toggleVisibility();

        //! Minimize by hiding status ...
        void setReducedInfo(bool minimized);

    private:
        QScopedPointer<Ui::CStatusMessageFormSmall> ui;
    };
} // namespace swift::gui::components
#endif // SWIFT_GUI_COMPONENTS_STATUSMESSAGEFORMSMALL_H

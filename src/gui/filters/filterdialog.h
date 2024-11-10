// SPDX-FileCopyrightText: Copyright (C) 2013 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

//! \file

#ifndef SWIFT_GUI_FILTERS_FILTERDIALOG_H
#define SWIFT_GUI_FILTERS_FILTERDIALOG_H

#include "gui/swiftguiexport.h"

#include <QDialog>
#include <QObject>

class QWidget;

namespace swift::gui::filters
{
    //! Base for filter dialog
    class SWIFT_GUI_EXPORT CFilterDialog : public QDialog
    {
        Q_OBJECT

    public:
        //! Constructor
        CFilterDialog(QWidget *parent = nullptr);

        //! Destructor
        virtual ~CFilterDialog();

    private:
        //! Stylesheet changed
        void onStyleSheetChanged();
    };

} // namespace

#endif // guard

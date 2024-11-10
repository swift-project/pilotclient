// SPDX-FileCopyrightText: Copyright (C) 2016 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

//! \file

#ifndef SWIFT_GUI_FILTERS_STATUSMESSAGEFILTERDIALOG_H
#define SWIFT_GUI_FILTERS_STATUSMESSAGEFILTERDIALOG_H

#include "gui/swiftguiexport.h"
#include "gui/filters/filterdialog.h"
#include "gui/models/modelfilter.h"

#include <QObject>
#include <QScopedPointer>
#include <memory>

class QWidget;

namespace swift::misc
{
    class CStatusMessageList;
}
namespace Ui
{
    class CStatusMessageFilterDialog;
}

namespace swift::gui::filters
{
    //! Form for a status message filter
    class SWIFT_GUI_EXPORT CStatusMessageFilterDialog :
        public CFilterDialog,
        public swift::gui::models::IModelFilterProvider<swift::misc::CStatusMessageList>
    {
        Q_OBJECT

    public:
        //! Constructor
        explicit CStatusMessageFilterDialog(QWidget *parent = nullptr);

        //! Destructor
        virtual ~CStatusMessageFilterDialog() override;

        //! \copydoc models::IModelFilterProvider::createModelFilter
        virtual std::unique_ptr<swift::gui::models::IModelFilter<swift::misc::CStatusMessageList>> createModelFilter() const override;

    private:
        QScopedPointer<Ui::CStatusMessageFilterDialog> ui;
    };

} // namespace

#endif // guard

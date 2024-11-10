// SPDX-FileCopyrightText: Copyright (C) 2016 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

//! \file

#ifndef BLACKGUI_FILTERS_STATUSMESSAGEFILTERDIALOG_H
#define BLACKGUI_FILTERS_STATUSMESSAGEFILTERDIALOG_H

#include "blackgui/blackguiexport.h"
#include "blackgui/filters/filterdialog.h"
#include "blackgui/models/modelfilter.h"

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

namespace BlackGui::Filters
{
    //! Form for a status message filter
    class BLACKGUI_EXPORT CStatusMessageFilterDialog :
        public CFilterDialog,
        public BlackGui::Models::IModelFilterProvider<swift::misc::CStatusMessageList>
    {
        Q_OBJECT

    public:
        //! Constructor
        explicit CStatusMessageFilterDialog(QWidget *parent = nullptr);

        //! Destructor
        virtual ~CStatusMessageFilterDialog() override;

        //! \copydoc Models::IModelFilterProvider::createModelFilter
        virtual std::unique_ptr<BlackGui::Models::IModelFilter<swift::misc::CStatusMessageList>> createModelFilter() const override;

    private:
        QScopedPointer<Ui::CStatusMessageFilterDialog> ui;
    };

} // namespace

#endif // guard

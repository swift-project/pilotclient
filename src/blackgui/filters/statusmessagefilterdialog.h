/* Copyright (C) 2016
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution. No part of swift project, including this file, may be copied, modified, propagated,
 * or distributed except according to the terms contained in the LICENSE file.
 */

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

namespace BlackMisc { class CStatusMessageList; }
namespace Ui { class CStatusMessageFilterDialog; }

namespace BlackGui::Filters
{
    //! Form for a status message filter
    class BLACKGUI_EXPORT CStatusMessageFilterDialog :
        public CFilterDialog,
        public BlackGui::Models::IModelFilterProvider<BlackMisc::CStatusMessageList>
    {
        Q_OBJECT

    public:
        //! Constructor
        explicit CStatusMessageFilterDialog(QWidget *parent = nullptr);

        //! Destructor
        virtual ~CStatusMessageFilterDialog() override;

        //! \copydoc Models::IModelFilterProvider::createModelFilter
        virtual std::unique_ptr<BlackGui::Models::IModelFilter<BlackMisc::CStatusMessageList>> createModelFilter() const override;

    private:
        QScopedPointer<Ui::CStatusMessageFilterDialog> ui;
    };

} // namespace

#endif // guard

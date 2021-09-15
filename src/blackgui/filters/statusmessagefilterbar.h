/* Copyright (C) 2016
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution. No part of swift project, including this file, may be copied, modified, propagated,
 * or distributed except according to the terms contained in the LICENSE file.
 */

//! \file

#ifndef BLACKGUI_FILTERS_STATUSMESSAGEFILTERBAR_H
#define BLACKGUI_FILTERS_STATUSMESSAGEFILTERBAR_H

#include "blackgui/filters/filterwidget.h"
#include "blackgui/models/modelfilter.h"
#include "blackmisc/statusmessagelist.h"
#include <QFrame>

namespace Ui { class CStatusMessageFilterBar; }
namespace BlackGui::Filters
{
    /*!
     * Filter status messages
     */
    class CStatusMessageFilterBar :
        public CFilterWidget,
        public BlackGui::Models::IModelFilterProvider<BlackMisc::CStatusMessageList>
    {
        Q_OBJECT

    public:
        //! Constructor
        explicit CStatusMessageFilterBar(QWidget *parent = nullptr);

        //! Show count
        void displayCount(bool show);

        //! Destructor
        virtual ~CStatusMessageFilterBar() override;

        //! Use icons with radio buttons
        void useRadioButtonDescriptiveIcons(bool oneCharacterText);

        //! \copydoc Models::IModelFilterProvider::createModelFilter
        virtual std::unique_ptr<BlackGui::Models::IModelFilter<BlackMisc::CStatusMessageList>> createModelFilter() const override;

    public slots:
        //! \copydoc CFilterWidget::onRowCountChanged
        virtual void onRowCountChanged(int count, bool withFilter) override;

    protected:
        //! \copydoc CFilterWidget::clearForm
        virtual void clearForm() override;

    private:
        QScopedPointer<Ui::CStatusMessageFilterBar> ui;

        //! Radio button was changed
        void radioButtonChanged();

        //! Get the selected severity
        BlackMisc::CStatusMessage::StatusSeverity getSelectedSeverity() const;
    };
} // ns
#endif // guard

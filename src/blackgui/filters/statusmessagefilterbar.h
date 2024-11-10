// SPDX-FileCopyrightText: Copyright (C) 2016 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

//! \file

#ifndef BLACKGUI_FILTERS_STATUSMESSAGEFILTERBAR_H
#define BLACKGUI_FILTERS_STATUSMESSAGEFILTERBAR_H

#include "blackgui/filters/filterwidget.h"
#include "blackgui/models/modelfilter.h"
#include "misc/statusmessagelist.h"
#include <QFrame>

namespace Ui
{
    class CStatusMessageFilterBar;
}
namespace BlackGui::Filters
{
    /*!
     * Filter status messages
     */
    class CStatusMessageFilterBar :
        public CFilterWidget,
        public BlackGui::Models::IModelFilterProvider<swift::misc::CStatusMessageList>
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
        virtual std::unique_ptr<BlackGui::Models::IModelFilter<swift::misc::CStatusMessageList>> createModelFilter() const override;

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
        swift::misc::CStatusMessage::StatusSeverity getSelectedSeverity() const;
    };
} // ns
#endif // guard

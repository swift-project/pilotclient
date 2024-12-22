// SPDX-FileCopyrightText: Copyright (C) 2016 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

//! \file

#ifndef SWIFT_GUI_FILTERS_STATUSMESSAGEFILTERBAR_H
#define SWIFT_GUI_FILTERS_STATUSMESSAGEFILTERBAR_H

#include "gui/filters/filterwidget.h"
#include "gui/models/modelfilter.h"
#include "misc/statusmessagelist.h"

namespace Ui
{
    class CStatusMessageFilterBar;
}
namespace swift::gui::filters
{
    /*!
     * Filter status messages
     */
    class CStatusMessageFilterBar :
        public CFilterWidget,
        public swift::gui::models::IModelFilterProvider<swift::misc::CStatusMessageList>
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

        //! \copydoc models::IModelFilterProvider::createModelFilter
        virtual std::unique_ptr<swift::gui::models::IModelFilter<swift::misc::CStatusMessageList>>
        createModelFilter() const override;

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
} // namespace swift::gui::filters
#endif // SWIFT_GUI_FILTERS_STATUSMESSAGEFILTERBAR_H

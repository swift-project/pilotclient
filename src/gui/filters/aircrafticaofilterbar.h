// SPDX-FileCopyrightText: Copyright (C) 2015 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

//! \file

#ifndef SWIFT_GUI_FILTERS_AIRCRAFTICAOFILTERBAR_H
#define SWIFT_GUI_FILTERS_AIRCRAFTICAOFILTERBAR_H

#include <memory>

#include <QObject>
#include <QScopedPointer>

#include "gui/filters/filterwidget.h"
#include "gui/models/modelfilter.h"
#include "gui/swiftguiexport.h"

namespace Ui
{
    class CAircraftIcaoFilterBar;
}
namespace swift::misc::aviation
{
    class CAircraftIcaoCode;
    class CAircraftIcaoCodeList;
} // namespace swift::misc::aviation
namespace swift::gui::filters
{
    /*!
     * Aircraft ICAO filter bar
     */
    class SWIFT_GUI_EXPORT CAircraftIcaoFilterBar :
        public CFilterWidget,
        public models::IModelFilterProvider<swift::misc::aviation::CAircraftIcaoCodeList>
    {
        Q_OBJECT

    public:
        //! Constructor
        explicit CAircraftIcaoFilterBar(QWidget *parent = nullptr);

        //! Destructor
        virtual ~CAircraftIcaoFilterBar() override;

        //! \copydoc models::IModelFilterProvider::createModelFilter
        std::unique_ptr<swift::gui::models::IModelFilter<swift::misc::aviation::CAircraftIcaoCodeList>> createModelFilter() const override;

        //! Filter by ICAO object as default values
        void filter(const swift::misc::aviation::CAircraftIcaoCode &icao);

        //! Hide the description
        void hideDescriptionField(bool hide);

    public slots:
        //! \copydoc CFilterWidget::onRowCountChanged
        virtual void onRowCountChanged(int count, bool withFilter) override;

    protected:
        //! \copydoc CFilterWidget::clearForm
        void clearForm() override;

    private:
        QScopedPointer<Ui::CAircraftIcaoFilterBar> ui;
        bool m_hasCompleters = false;

        //! Set the completers
        void initCompleters();
    };
} // namespace swift::gui::filters

#endif // guard

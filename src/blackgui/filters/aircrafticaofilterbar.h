// SPDX-FileCopyrightText: Copyright (C) 2015 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

//! \file

#ifndef BLACKGUI_FILTERS_AIRCRAFTICAOFILTERBAR_H
#define BLACKGUI_FILTERS_AIRCRAFTICAOFILTERBAR_H

#include "blackgui/blackguiexport.h"
#include "blackgui/filters/filterwidget.h"
#include "blackgui/models/modelfilter.h"

#include <QObject>
#include <QScopedPointer>
#include <memory>

namespace Ui
{
    class CAircraftIcaoFilterBar;
}
namespace swift::misc::aviation
{
    class CAircraftIcaoCode;
    class CAircraftIcaoCodeList;
}
namespace BlackGui::Filters
{
    /*!
     * Aircraft ICAO filter bar
     */
    class BLACKGUI_EXPORT CAircraftIcaoFilterBar :
        public CFilterWidget,
        public Models::IModelFilterProvider<swift::misc::aviation::CAircraftIcaoCodeList>
    {
        Q_OBJECT

    public:
        //! Constructor
        explicit CAircraftIcaoFilterBar(QWidget *parent = nullptr);

        //! Destructor
        virtual ~CAircraftIcaoFilterBar() override;

        //! \copydoc Models::IModelFilterProvider::createModelFilter
        std::unique_ptr<BlackGui::Models::IModelFilter<swift::misc::aviation::CAircraftIcaoCodeList>> createModelFilter() const override;

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
} // ns

#endif // guard

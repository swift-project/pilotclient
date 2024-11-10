// SPDX-FileCopyrightText: Copyright (C) 2015 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

//! \file

#ifndef BLACKGUI_FILTERS_LIVERYFILTERBAR_H
#define BLACKGUI_FILTERS_LIVERYFILTERBAR_H

#include "blackgui/blackguiexport.h"
#include "blackgui/filters/filterwidget.h"
#include "blackgui/models/modelfilter.h"
#include "misc/rgbcolor.h"

#include <QObject>
#include <QScopedPointer>
#include <memory>

namespace Ui
{
    class CLiveryFilterBar;
}
namespace swift::misc::aviation
{
    class CLivery;
    class CLiveryList;
}
namespace BlackGui::Filters
{
    /*!
     * Livery filter bar
     */
    class BLACKGUI_EXPORT CLiveryFilterBar :
        public CFilterWidget,
        public Models::IModelFilterProvider<swift::misc::aviation::CLiveryList>
    {
        Q_OBJECT

    public:
        //! Constructor
        explicit CLiveryFilterBar(QWidget *parent = nullptr);

        //! Destructor
        virtual ~CLiveryFilterBar() override;

        //! \copydoc Models::IModelFilterProvider::createModelFilter
        virtual std::unique_ptr<BlackGui::Models::IModelFilter<swift::misc::aviation::CLiveryList>> createModelFilter() const override;

        //! Filter by livery values
        void filter(const swift::misc::aviation::CLivery &livery);

        //! Filter by airline values
        void filter(const swift::misc::aviation::CAirlineIcaoCode &airlineIcao);

    public slots:
        //! \copydoc CFilterWidget::onRowCountChanged
        virtual void onRowCountChanged(int count, bool withFilter) override;

    protected:
        //! Clear form
        virtual void clearForm() override;

    private:
        //! Color changed
        void onColorChanged(const swift::misc::CRgbColor &color);

        //! Color distance changed
        void onColorDistanceChanged(int distance);

        QScopedPointer<Ui::CLiveryFilterBar> ui;
    };
} // ns

#endif // guard

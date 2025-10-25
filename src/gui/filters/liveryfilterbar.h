// SPDX-FileCopyrightText: Copyright (C) 2015 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

//! \file

#ifndef SWIFT_GUI_FILTERS_LIVERYFILTERBAR_H
#define SWIFT_GUI_FILTERS_LIVERYFILTERBAR_H

#include <memory>

#include <QObject>
#include <QScopedPointer>

#include "gui/filters/filterwidget.h"
#include "gui/models/modelfilter.h"
#include "gui/swiftguiexport.h"
#include "misc/aviation/liverylist.h"
#include "misc/rgbcolor.h"

namespace Ui
{
    class CLiveryFilterBar;
}
namespace swift::misc::aviation
{
    class CLivery;
} // namespace swift::misc::aviation
namespace swift::gui::filters
{
    /*!
     * Livery filter bar
     */
    class SWIFT_GUI_EXPORT CLiveryFilterBar :
        public CFilterWidget,
        public models::IModelFilterProvider<swift::misc::aviation::CLiveryList>
    {
        Q_OBJECT

    public:
        //! Constructor
        explicit CLiveryFilterBar(QWidget *parent = nullptr);

        //! Destructor
        virtual ~CLiveryFilterBar() override;

        //! \copydoc models::IModelFilterProvider::createModelFilter
        virtual std::unique_ptr<swift::gui::models::IModelFilter<swift::misc::aviation::CLiveryList>>
        createModelFilter() const override;

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
} // namespace swift::gui::filters

#endif // SWIFT_GUI_FILTERS_LIVERYFILTERBAR_H

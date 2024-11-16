// SPDX-FileCopyrightText: Copyright (C) 2014 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

//! \file

#ifndef SWIFT_GUI_DATAINFOAREACOMPONENT_H
#define SWIFT_GUI_DATAINFOAREACOMPONENT_H

#include <QObject>
#include <QScopedPointer>
#include <QSize>

#include "gui/infoarea.h"
#include "gui/swiftguiexport.h"
#include "misc/network/entityflags.h"

class QPixmap;
class QWidget;

namespace Ui
{
    class CDataInfoAreaComponent;
}
namespace swift::gui::components
{
    class CDbAircraftIcaoComponent;
    class CDbAirlineIcaoComponent;
    class CDbCountryComponent;
    class CDbDistributorComponent;
    class CDbLiveryComponent;
    class CDbModelComponent;
    class CDbAircraftCategoryComponent;

    /*!
     * Info area containing the DB data (models, liveries ...)
     */
    class SWIFT_GUI_EXPORT CDataInfoAreaComponent : public swift::gui::CInfoArea
    {
        Q_OBJECT

    public:
        //! Info areas
        enum InfoArea
        {
            // index must match tab index!
            InfoAreaModels = 0,
            InfoAreaLiveries = 1,
            InfoAreaDistributors = 2,
            InfoAreaAircraftIcao = 3,
            InfoAreaAirlineIcao = 4,
            InfoAreaCountries = 5,
            InfoAreaAircraftCategories = 6,
            InfoAreaNone = -1
        };

        //! Constructor
        explicit CDataInfoAreaComponent(QWidget *parent = nullptr);

        //! Destructor
        virtual ~CDataInfoAreaComponent() override;

        //! DB model component
        CDbModelComponent *getModelComponent() const;

        //! DB livery component
        CDbLiveryComponent *getLiveryComponent() const;

        //! DB distributor component
        CDbDistributorComponent *getDistributorComponent() const;

        //! DB aircraft ICAO component
        CDbAircraftIcaoComponent *getAircraftComponent() const;

        //! DB airline ICAO component
        CDbAirlineIcaoComponent *getAirlineComponent() const;

        //! DB country component
        CDbCountryComponent *getCountryComponent() const;

        //! DB aircraft category componentxs
        CDbAircraftCategoryComponent *getAircraftCategoryComponent() const;

    public slots:
        //! Write to resource dir
        bool writeDbDataToResourceDir() const;

        //! Load from resource dir
        bool readDbDataFromResourceDir();

        //! Request update of all DB data
        void requestUpdateOfAllDbData();

        //! Load new data (based on timestamp, incremental)
        void requestUpdatedData(swift::misc::network::CEntityFlags::Entity entity);

    protected:
        //! \copydoc CInfoArea::getPreferredSizeWhenFloating
        virtual QSize getPreferredSizeWhenFloating(int areaIndex) const override;

        //! \copydoc CInfoArea::indexToPixmap
        virtual const QPixmap &indexToPixmap(int areaIndex) const override;

    private:
        QScopedPointer<Ui::CDataInfoAreaComponent> ui;
    };
} // namespace swift::gui::components

#endif // guard

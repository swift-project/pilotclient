// SPDX-FileCopyrightText: Copyright (C) 2014 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

//! \file

#ifndef BLACKGUI_DATAMAININFOAREACOMPONENT_H
#define BLACKGUI_DATAMAININFOAREACOMPONENT_H

#include "blackgui/blackguiexport.h"
#include "blackgui/infoarea.h"

#include <QObject>
#include <QScopedPointer>
#include <QSize>

class QPixmap;
class QWidget;

namespace Ui
{
    class CDataMainInfoAreaComponent;
}
namespace BlackGui::Components
{
    class CDataInfoAreaComponent;
    class CDbMappingComponent;
    class CLogComponent;
    class CDataSettingsComponent;

    /*!
     * Main info area for data entry tool
     */
    class BLACKGUI_EXPORT CDataMainInfoAreaComponent :
        public BlackGui::CInfoArea
    {
        Q_OBJECT

    public:
        //! Info areas
        enum InfoArea
        {
            // index must match tab index!
            InfoAreaMapping = 0,
            InfoAreaData = 1,
            InfoAreaSettings = 2,
            InfoAreaLog = 3,
            InfoAreaNone = -1
        };

        //! Constructor
        explicit CDataMainInfoAreaComponent(QWidget *parent = nullptr);

        //! Destructor
        virtual ~CDataMainInfoAreaComponent() override;

        //! Log component
        CLogComponent *getLogComponent() const;

        //! Mapping component
        CDbMappingComponent *getMappingComponent() const;

        //! Info area component
        CDataInfoAreaComponent *getDataInfoAreaComponent() const;

        //! Settings component
        BlackGui::Components::CDataSettingsComponent *getDataSettingsComponent() const;

        //! Display the log
        void displayLog();

    public slots:
        //! Select log area
        void selectLog();

    protected:
        //! \copydoc CInfoArea::getPreferredSizeWhenFloating
        virtual QSize getPreferredSizeWhenFloating(int areaIndex) const override;

        //! \copydoc CInfoArea::indexToPixmap
        virtual const QPixmap &indexToPixmap(int areaIndex) const override;

    private:
        QScopedPointer<Ui::CDataMainInfoAreaComponent> ui;
    };
} // ns

#endif // guard

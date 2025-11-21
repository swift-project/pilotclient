// SPDX-FileCopyrightText: Copyright (C) 2014 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

//! \file

#ifndef SWIFT_GUI_DATAMAININFOAREACOMPONENT_H
#define SWIFT_GUI_DATAMAININFOAREACOMPONENT_H

#include <QObject>
#include <QScopedPointer>
#include <QSize>

#include "gui/infoarea.h"
#include "gui/swiftguiexport.h"

class QPixmap;
class QWidget;

namespace Ui
{
    class CDataMainInfoAreaComponent;
}
namespace swift::gui::components
{
    class CDataInfoAreaComponent;
    class CDbMappingComponent;
    class CLogComponent;
    class CDataSettingsComponent;

    /*!
     * Main info area for data entry tool
     */
    class SWIFT_GUI_EXPORT CDataMainInfoAreaComponent : public swift::gui::CInfoArea
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
        ~CDataMainInfoAreaComponent() override;

        //! Log component
        CLogComponent *getLogComponent() const;

        //! Mapping component
        CDbMappingComponent *getMappingComponent() const;

        //! Info area component
        CDataInfoAreaComponent *getDataInfoAreaComponent() const;

        //! Settings component
        swift::gui::components::CDataSettingsComponent *getDataSettingsComponent() const;

        //! Display the log
        void displayLog();

    public slots:
        //! Select log area
        void selectLog();

    protected:
        //! \copydoc CInfoArea::getPreferredSizeWhenFloating
        QSize getPreferredSizeWhenFloating(int areaIndex) const override;

        //! \copydoc CInfoArea::indexToPixmap
        const QPixmap &indexToPixmap(int areaIndex) const override;

    private:
        QScopedPointer<Ui::CDataMainInfoAreaComponent> ui;
    };
} // namespace swift::gui::components

#endif // SWIFT_GUI_DATAMAININFOAREACOMPONENT_H

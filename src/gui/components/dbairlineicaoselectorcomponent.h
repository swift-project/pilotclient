// SPDX-FileCopyrightText: Copyright (C) 2015 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

//! \file

#ifndef SWIFT_GUI_COMPONENTS_DBAIRLINEICAOSELECTORCOMPONENT_H
#define SWIFT_GUI_COMPONENTS_DBAIRLINEICAOSELECTORCOMPONENT_H

#include <QObject>
#include <QScopedPointer>
#include <QString>

#include "gui/components/dbairlineicaoselectorbase.h"
#include "gui/swiftguiexport.h"
#include "misc/aviation/airlineicaocode.h"

class QCompleter;
class QWidget;

namespace Ui
{
    class CDbAirlineIcaoSelectorComponent;
}
namespace swift::gui::components
{
    /*!
     * Airline ICAO selector
     */
    class SWIFT_GUI_EXPORT CDbAirlineIcaoSelectorComponent : public CDbAirlineIcaoSelectorBase
    {
        Q_OBJECT

    public:
        //! How to display the info
        enum Display
        {
            DisplayVDesignatorAndId,
            DisplayCompleterString
        };

        //! Constructor
        explicit CDbAirlineIcaoSelectorComponent(QWidget *parent = nullptr);

        //! Destructor
        //! \note needed for forward declared QScopedPointer and needs to be in .cpp
        ~CDbAirlineIcaoSelectorComponent() override;

        //! \name Base class overrides
        //! @{
        void setReadOnly(bool readOnly) override;
        bool setAirlineIcao(const swift::misc::aviation::CAirlineIcaoCode &icao) override;
        void clear() override;
        //! @}

        //! Raw text designator
        QString getRawDesignator() const;

        //! Display ICAO description
        void displayWithIcaoDescription(bool description);

        //! Display mode
        void displayMode(Display mode) { this->m_display = mode; }

    protected:
        //! \copydoc CDbAirlineIcaoSelectorBase::createCompleter
        QCompleter *createCompleter() override;

    private:
        //! Get the completer strings
        //! \remark shared for performance reasons
        static const QStringList &completerStrings();

        //! Data have been changed
        void onDataChanged();

        QScopedPointer<Ui::CDbAirlineIcaoSelectorComponent> ui;
        Display m_display = DisplayVDesignatorAndId;
    };
} // namespace swift::gui::components
#endif // SWIFT_GUI_COMPONENTS_DBAIRLINEICAOSELECTORCOMPONENT_H

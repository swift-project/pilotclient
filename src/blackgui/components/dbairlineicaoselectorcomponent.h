// SPDX-FileCopyrightText: Copyright (C) 2015 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

//! \file

#ifndef BLACKGUI_COMPONENTS_DBAIRLINEICAOSELECTORCOMPONENT_H
#define BLACKGUI_COMPONENTS_DBAIRLINEICAOSELECTORCOMPONENT_H

#include "blackgui/blackguiexport.h"
#include "blackgui/components/dbairlineicaoselectorbase.h"
#include "misc/aviation/airlineicaocode.h"

#include <QObject>
#include <QScopedPointer>
#include <QString>

class QCompleter;
class QWidget;

namespace Ui
{
    class CDbAirlineIcaoSelectorComponent;
}
namespace BlackGui::Components
{
    /*!
     * Airline ICAO selector
     */
    class BLACKGUI_EXPORT CDbAirlineIcaoSelectorComponent : public CDbAirlineIcaoSelectorBase
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
        virtual ~CDbAirlineIcaoSelectorComponent() override;

        //! \name Base class overrides
        //! @{
        virtual void setReadOnly(bool readOnly) override;
        virtual bool setAirlineIcao(const swift::misc::aviation::CAirlineIcaoCode &icao) override;
        virtual void clear() override;
        //! @}

        //! Raw text designator
        QString getRawDesignator() const;

        //! Display ICAO description
        void displayWithIcaoDescription(bool description);

        //! Display mode
        void displayMode(Display mode) { this->m_display = mode; }

    protected:
        //! \copydoc CDbAirlineIcaoSelectorBase::createCompleter
        virtual QCompleter *createCompleter() override;

    private:
        //! Get the completer strings
        //! \remark shared for performance reasons
        static const QStringList &completerStrings();

        //! Data have been changed
        void onDataChanged();

        QScopedPointer<Ui::CDbAirlineIcaoSelectorComponent> ui;
        Display m_display = DisplayVDesignatorAndId;
    };
} // ns
#endif // guard

// SPDX-FileCopyrightText: Copyright (C) 2017 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

//! \file

#ifndef SWIFT_GUI_COMPONENTS_DBUSSERVERADDRESSSELECTOR_H
#define SWIFT_GUI_COMPONENTS_DBUSSERVERADDRESSSELECTOR_H

#include <QFrame>
#include <QScopedPointer>

#include "gui/swiftguiexport.h"

namespace Ui
{
    class CDBusServerAddressSelector;
}
namespace swift::gui::components
{
    //! Select DBus address such as session P2P, ...
    class SWIFT_GUI_EXPORT CDBusServerAddressSelector : public QFrame
    {
        Q_OBJECT

    public:
        //! Ctor
        explicit CDBusServerAddressSelector(QWidget *parent = nullptr);

        //! Dtor
        ~CDBusServerAddressSelector() override;

        //! DBus address for P2P or empty
        QString getP2PAddress() const;

        //! DBus address for all 3 options
        QString getDBusAddress() const;

        //! Get DBus cmd.line arguments
        QStringList getDBusCmdLineArgs() const;

        //! P2P DBus address
        bool isP2P() const;

        //! Set default P2P address
        void setDefaultP2PAddress(const QString &address);

        //! Set values
        void set(const QString &dBus);

        //! Set system DBus radio button visible/invisible
        void setSystemDBusVisible(bool visible);

        //! P2P visible
        void setP2PDBusVisible(bool visible);

        //! Set to be used for xswiftbus
        void setForXSwiftBus();

    signals:
        //! Is being edited
        void editingFinished();

    private:
        QScopedPointer<Ui::CDBusServerAddressSelector> ui;

        //! Radio button clicked
        void onRadioButtonReleased();
    };
} // namespace swift::gui::components
#endif // SWIFT_GUI_COMPONENTS_DBUSSERVERADDRESSSELECTOR_H

/* Copyright (C) 2017
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution. No part of swift project, including this file, may be copied, modified, propagated,
 * or distributed except according to the terms contained in the LICENSE file.
 */

//! \file

#ifndef BLACKGUI_COMPONENTS_DBUSSERVERADDRESSSELECTOR_H
#define BLACKGUI_COMPONENTS_DBUSSERVERADDRESSSELECTOR_H

#include "blackgui/blackguiexport.h"
#include <QFrame>
#include <QScopedPointer>

namespace Ui { class CDBusServerAddressSelector; }
namespace BlackGui::Components
{
    //! Select DBus address such as session P2P, ...
    class BLACKGUI_EXPORT CDBusServerAddressSelector : public QFrame
    {
        Q_OBJECT

    public:
        //! Ctor
        explicit CDBusServerAddressSelector(QWidget *parent = nullptr);

        //! Dtor
        virtual ~CDBusServerAddressSelector() override;

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

        //! Set to be used for XSwiftBus
        void setForXSwiftBus();

    signals:
        //! Is being edited
        void editingFinished();

    private:
        QScopedPointer<Ui::CDBusServerAddressSelector> ui;

        //! Radio button clicked
        void onRadioButtonReleased();
    };
} // ns
#endif // guard

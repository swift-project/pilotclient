/* Copyright (C) 2015
 * swift project Community / Contributors
 *
 * This file is part of swift Project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution. No part of swift project, including this file, may be copied, modified, propagated,
 * or distributed except according to the terms contained in the LICENSE file.
 */

//! \file

#ifndef BLACKGUI_PLUGINDETAILSWINDOW_H
#define BLACKGUI_PLUGINDETAILSWINDOW_H

#include "blackgui/blackguiexport.h"

#include <QObject>
#include <QScopedPointer>
#include <QString>
#include <QWidget>

class QShowEvent;

namespace Ui
{
    class CPluginDetailsWindow;
}
namespace BlackGui
{
    /*!
     * A window that shows plugin details.
     */
    class BLACKGUI_EXPORT CPluginDetailsWindow : public QWidget
    {
        Q_OBJECT

    public:
        //! Ctor
        explicit CPluginDetailsWindow(QWidget *parent = nullptr);

        //! Destructor
        virtual ~CPluginDetailsWindow() override;

        //! Sets the plugin name
        void setPluginName(const QString &name);

        //! Sets the plugin id
        void setPluginIdentifier(const QString &id);

        //! Sets the plugin description
        void setPluginDescription(const QString &description);

        //! Sets the plugin authors
        void setPluginAuthors(const QString &authors);

    protected:
        //! \copydoc QWidget::showEvent()
        virtual void showEvent(QShowEvent *event) override;

    private:
        QScopedPointer<Ui::CPluginDetailsWindow> ui;
    };
} // namespace

#endif // guard

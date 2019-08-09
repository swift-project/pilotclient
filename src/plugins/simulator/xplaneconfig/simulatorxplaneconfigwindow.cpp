/* Copyright (C) 2015
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution. No part of swift project, including this file, may be copied, modified, propagated,
 * or distributed except according to the terms contained in the LICENSE file.
 */

#include "simulatorxplaneconfigwindow.h"
#include "blackcore/application.h"
#include "ui_simulatorxplaneconfigwindow.h"
#include "blackmisc/simulation/xplane/xswiftbusconfigwriter.h"

#include <QComboBox>
#include <QDialogButtonBox>

using namespace BlackGui;
using namespace BlackMisc;
using namespace BlackMisc::Simulation::Settings;
using namespace BlackMisc::Simulation::XPlane;

namespace BlackSimPlugin
{
    namespace XPlane
    {
        CSimulatorXPlaneConfigWindow::CSimulatorXPlaneConfigWindow(QWidget *parent) :
            CPluginConfigWindow(parent),
            ui(new Ui::CSimulatorXPlaneConfigWindow)
        {
            ui->setupUi(this);
            ui->comp_SettingsXSwiftBus->setDefaultP2PAddress(m_xSwiftBusServerSettings.getDefault().getDBusServerAddressQt());

            const CXSwiftBusSettings defaultSettings = TXSwiftBusSettings::defaultValue();
            ui->sb_MaxAircraft->setMaximum(defaultSettings.getMaxPlanes() * 2);
            ui->sb_FollowAircraftDistanceM->setMaximum(defaultSettings.getFollowAircraftDistanceM() * 5);
            ui->ds_MaxDrawDistanceNM->setMaximum(qRound(defaultSettings.getMaxDrawDistanceNM() * 3));

            const CXSwiftBusSettings s = m_xSwiftBusServerSettings.getThreadLocal();
            this->setUiValues(s);

            connect(ui->bb_OkCancel, &QDialogButtonBox::accepted, this, &CSimulatorXPlaneConfigWindow::onSettingsAccepted);
            connect(ui->bb_OkCancel, &QDialogButtonBox::rejected, this, &CSimulatorXPlaneConfigWindow::close);
        }

        CSimulatorXPlaneConfigWindow::~CSimulatorXPlaneConfigWindow()
        { }

        void CSimulatorXPlaneConfigWindow::onSettingsAccepted()
        {
            const CXSwiftBusSettings s = m_xSwiftBusServerSettings.getThreadLocal();
            const CXSwiftBusSettings changed = this->getSettingsFromUI();
            if (s != changed)
            {
                m_xSwiftBusServerSettings.set(changed);
                CXSwiftBusConfigWriter xswiftbusConfigWriter;
                xswiftbusConfigWriter.setDBusAddress(changed.getDBusServerAddressQt());
                xswiftbusConfigWriter.updateInAllXPlaneVersions();
            }
            this->close();
        }

        CXSwiftBusSettings CSimulatorXPlaneConfigWindow::getSettingsFromUI() const
        {
            CXSwiftBusSettings s = m_xSwiftBusServerSettings.getThreadLocal();
            s.setDBusServerAddressQt(ui->comp_SettingsXSwiftBus->getDBusAddress());
            s.setMaxDrawDistanceNM(ui->ds_MaxDrawDistanceNM->value());
            s.setMaxPlanes(ui->sb_MaxAircraft->value());
            s.setFollowAircraftDistanceM(ui->sb_FollowAircraftDistanceM->value());
            s.setDrawingLabels(ui->cb_DrawLabels->isChecked());
            s.setNightTextureModeQt(ui->cb_NightTextureMode->currentText());
            return s;
        }

        void CSimulatorXPlaneConfigWindow::setUiValues(const CXSwiftBusSettings &settings)
        {
            ui->comp_SettingsXSwiftBus->set(settings.getDBusServerAddressQt());
            ui->sb_MaxAircraft->setValue(settings.getMaxPlanes());
            ui->sb_FollowAircraftDistanceM->setValue(settings.getFollowAircraftDistanceM());
            ui->cb_DrawLabels->setChecked(settings.isDrawingLabels());
            ui->ds_MaxDrawDistanceNM->setValue(settings.getMaxDrawDistanceNM());

            const QString s = settings.getNightTextureModeQt().left(1);
            if (!s.isEmpty())
            {
                for (int i = 0; i < ui->cb_NightTextureMode->count(); i++)
                {
                    if (ui->cb_NightTextureMode->itemText(i).startsWith(s, Qt::CaseInsensitive))
                    {
                        ui->cb_NightTextureMode->setCurrentIndex(i);
                        break;
                    }
                }
            }
        }

        void CSimulatorXPlaneConfigWindow::onSettingsChanged()
        {
            this->setUiValues(m_xSwiftBusServerSettings.get());
        }
    } // ns
} // ns

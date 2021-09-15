/* Copyright (C) 2017
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution. No part of swift project, including this file, may be copied, modified, propagated,
 * or distributed except according to the terms contained in the LICENSE file.
 */

#include "blackconfig/buildconfig.h"
#include "blackmisc/simulation/aircraftmodel.h"
#include "blackmisc/logmessage.h"
#include "modelconverterx.h"
#include <QFile>
#include <QDir>

using namespace BlackConfig;
using namespace BlackMisc;
using namespace BlackMisc::Simulation::Settings;

namespace BlackMisc::Simulation
{
    QProcess *CModelConverterX::s_proccess = nullptr;

    bool CModelConverterX::supportsModelConverterX()
    {
        if (!CBuildConfig::isRunningOnWindowsNtPlatform()) { return false; }
        return !getBinary().isEmpty();
    }

    QProcess *CModelConverterX::startModelConverterX(const CAircraftModel &model, QObject *parent)
    {
        // checks
        if (model.getFileName().isEmpty()) { return nullptr; }
        const QString modelConverterX = getBinary();
        if (modelConverterX.isEmpty()) { return nullptr; }

        // delete other MCX
        if (s_proccess)
        {
            QProcess *old = s_proccess;
            s_proccess = nullptr;
            if (old->state() == QProcess::Running)
            {
                // if still running, terminate and then delete
                QObject::connect(old, qOverload<int, QProcess::ExitStatus>(&QProcess::finished), old, &QObject::deleteLater);
                old->terminate();
            }
            else
            {
                old->deleteLater();
            }
        }

        QProcess *process = new QProcess(parent);
        const QString argument = QDir::toNativeSeparators(model.getFileName());
        process->setProgram(modelConverterX);
        process->setArguments({argument});
        process->start();
        s_proccess = process;
        return process;
    }

    QString CModelConverterX::getBinary()
    {
        static const BlackMisc::CSettingReadOnly<TModelConverterXBinary> setting(new QObject());
        const QString mcx = setting.get();
        if (mcx.isEmpty()) return {};
        const QFile f(mcx);
        return (f.exists()) ? mcx : QString();
    }
} // ns

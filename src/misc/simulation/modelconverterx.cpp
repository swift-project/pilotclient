// SPDX-FileCopyrightText: Copyright (C) 2017 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

#include "misc/simulation/modelconverterx.h"

#include <QDir>
#include <QFile>

#include "config/buildconfig.h"
#include "misc/logmessage.h"
#include "misc/simulation/aircraftmodel.h"

using namespace swift::config;
using namespace swift::misc;
using namespace swift::misc::simulation::settings;

namespace swift::misc::simulation
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
                QObject::connect(old, qOverload<int, QProcess::ExitStatus>(&QProcess::finished), old,
                                 &QObject::deleteLater);
                old->terminate();
            }
            else { old->deleteLater(); }
        }

        QProcess *process = new QProcess(parent);
        const QString argument = QDir::toNativeSeparators(model.getFileName());
        process->setProgram(modelConverterX);
        process->setArguments({ argument });
        process->start();
        s_proccess = process;
        return process;
    }

    QString CModelConverterX::getBinary()
    {
        static const swift::misc::CSettingReadOnly<TModelConverterXBinary> setting(new QObject());
        const QString mcx = setting.get();
        if (mcx.isEmpty()) return {};
        const QFile f(mcx);
        return (f.exists()) ? mcx : QString();
    }
} // namespace swift::misc::simulation

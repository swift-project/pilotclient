// SPDX-FileCopyrightText: Copyright (C) 2017 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

//! \file

#ifndef SWIFT_MISC_SIMULATION_MODELCONVERTERX_H
#define SWIFT_MISC_SIMULATION_MODELCONVERTERX_H

#include <QFile>
#include <QProcess>
#include <QString>

#include "misc/settingscache.h"
#include "misc/swiftmiscexport.h"

namespace swift::misc::simulation
{
    class CAircraftModel;

    //! Simple utility class to support ModelConverterX integration.
    class SWIFT_MISC_EXPORT CModelConverterX
    {
    public:
        //! Default constructor
        CModelConverterX() = delete;

        //! Guess a default simulator based on installation
        static bool supportsModelConverterX();

        //! Start ModelConverterX for given model
        static QProcess *startModelConverterX(const CAircraftModel &model, QObject *parent);

    private:
        //! Get the binary
        static QString getBinary();

        static QProcess *s_proccess; //!< 0..1 process running
    };

    namespace settings
    {
        //! Binary of MCX
        struct TModelConverterXBinary : public swift::misc::TSettingTrait<QString>
        {
            //! \copydoc swift::misc::TSettingTrait::key
            static const char *key() { return "mapping/modelconverterxbin"; }

            //! \copydoc swift::misc::TSettingTrait::isValid
            static bool isValid(const QString &value, QString &)
            {
                if (value.isEmpty()) { return true; }
                const QFile f(value);
                return f.exists();
            }
        };
    } // namespace settings
} // namespace swift::misc::simulation

#endif // guard

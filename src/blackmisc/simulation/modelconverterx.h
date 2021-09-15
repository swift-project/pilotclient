/* Copyright (C) 2017
 * swift Project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution. No part of swift project, including this file, may be copied, modified, propagated,
 * or distributed except according to the terms contained in the LICENSE file.
 */

//! \file

#ifndef BLACKMISC_SIMULATION_MODELCONVERTERX_H
#define BLACKMISC_SIMULATION_MODELCONVERTERX_H

#include "blackmisc/blackmiscexport.h"
#include "blackmisc/settingscache.h"
#include <QProcess>
#include <QString>
#include <QFile>

namespace BlackMisc::Simulation
{
    class CAircraftModel;

    //! Simple utility class to support ModelConverterX integration.
    class BLACKMISC_EXPORT CModelConverterX
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

    namespace Settings
    {
        //! Binary of MCX
        struct TModelConverterXBinary : public BlackMisc::TSettingTrait<QString>
        {
            //! \copydoc BlackMisc::TSettingTrait::key
            static const char *key() { return "mapping/modelconverterxbin"; }

            //! \copydoc BlackMisc::TSettingTrait::isValid
            static bool isValid(const QString &value, QString &)
            {
                if (value.isEmpty()) { return true; }
                const QFile f(value);
                return f.exists();
            }
        };
    } // ns
} // ns

#endif // guard

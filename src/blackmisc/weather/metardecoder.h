// SPDX-FileCopyrightText: Copyright (C) 2015 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

//! \file

#ifndef BLACKMISC_WEATHER_METARDECODER_H
#define BLACKMISC_WEATHER_METARDECODER_H

#include "blackmisc/blackmiscexport.h"
#include "blackmisc/weather/metar.h"

#include <QObject>
#include <QString>
#include <memory>
#include <vector>

namespace BlackMisc::Weather
{
    class IMetarDecoderPart;

    //! Metar Decoder
    class BLACKMISC_EXPORT CMetarDecoder : public QObject
    {
        Q_OBJECT

    public:
        //! Default constructor
        CMetarDecoder();

        //! Default destructor
        virtual ~CMetarDecoder() override;

        //! Decode metar
        CMetar decode(const QString &metarString) const;

    private:
        void allocateDecoders();
        std::vector<std::unique_ptr<IMetarDecoderPart>> m_decoders;
    };

} // namespace

#endif // guard

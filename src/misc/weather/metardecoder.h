// SPDX-FileCopyrightText: Copyright (C) 2015 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

//! \file

#ifndef SWIFT_MISC_WEATHER_METARDECODER_H
#define SWIFT_MISC_WEATHER_METARDECODER_H

#include <memory>
#include <vector>

#include <QObject>
#include <QString>

#include "misc/swiftmiscexport.h"
#include "misc/weather/metar.h"

namespace swift::misc::weather
{
    class IMetarDecoderPart;

    //! Metar Decoder
    class SWIFT_MISC_EXPORT CMetarDecoder : public QObject
    {
        Q_OBJECT

    public:
        //! Default constructor
        CMetarDecoder();

        //! Default destructor
        ~CMetarDecoder() override;

        //! Decode metar
        CMetar decode(const QString &metarString) const;

    private:
        void allocateDecoders();
        std::vector<std::unique_ptr<IMetarDecoderPart>> m_decoders;
    };

} // namespace swift::misc::weather

#endif // SWIFT_MISC_WEATHER_METARDECODER_H

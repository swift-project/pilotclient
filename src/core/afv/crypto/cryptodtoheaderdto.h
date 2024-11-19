// SPDX-FileCopyrightText: Copyright (C) 2019 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

//! \file

#ifndef SWIFT_CORE_AFV_CRYPTO_CRYPTODTOHEADERDTO_H
#define SWIFT_CORE_AFV_CRYPTO_CRYPTODTOHEADERDTO_H

#include <cstdint>

#include <QString>

#include "msgpack.hpp"

#include "core/afv/crypto/cryptodtomode.h"

namespace swift::core::afv::crypto
{
    //! DTO header
    struct CryptoDtoHeaderDto
    {
        std::string ChannelTag; //!< channel
        uint64_t Sequence; //!< sequence
        CryptoDtoMode Mode; //!< mode
        MSGPACK_DEFINE(ChannelTag, Sequence, Mode)
    };
} // namespace swift::core::afv::crypto

#endif // SWIFT_CORE_AFV_CRYPTO_CRYPTODTOHEADERDTO_H

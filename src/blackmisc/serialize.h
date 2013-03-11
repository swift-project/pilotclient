//! Copyright (C) 2013 Roland Winklmeier
//! This Source Code Form is subject to the terms of the Mozilla Public
//! License, v. 2.0. If a copy of the MPL was not distributed with this
//! file, You can obtain one at http://mozilla.org/MPL/2.0/

#ifndef SERIALIZE_H
#define SERIALIZE_H

class QDataStream;

class ISerialize
{
public:
    ISerialize();
    virtual ~ISerialize() {};

    virtual QDataStream& operator<< (QDataStream& in) = 0;
    virtual QDataStream& operator>> (QDataStream& out) const = 0;
};

#endif // SERIALIZE_H

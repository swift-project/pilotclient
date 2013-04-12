//! Copyright (C) 2013 Roland Winklmeier
//! This Source Code Form is subject to the terms of the Mozilla Public
//! License, v. 2.0. If a copy of the MPL was not distributed with this
//! file, You can obtain one at http://mozilla.org/MPL/2.0/

#ifndef MESSAGE_H
#define MESSAGE_H

#include "blackmisc/serialize.h"
#include <QtGlobal>
#include <QDataStream>
#include <QTextStream>

namespace BlackMisc
{

class IMessage : public ISerialize
{
public:
    IMessage(QString &id);

    QString getID() const;

    virtual QDataStream &operator<< (QDataStream &in) = 0;
    virtual QDataStream &operator>> (QDataStream &out) const = 0;

    virtual QTextStream &operator<< (QTextStream &in) = 0;
    virtual QTextStream &operator>> (QTextStream &out) const = 0;


protected:
    QString m_message_id;
};

class TestMessage : public IMessage
{
public:
    TestMessage() :  IMessage(QString("MSG_ID_TestMessage"))
    {
        testString = "This is a test message!";
    }

    QString getTestString() const { return testString; }

    //QDataStream &operator>>(qint8 &i);

    virtual QDataStream &operator<< (QDataStream &in)
    {
        in >> m_message_id;
        in >> testString;
        return in;
    }

    virtual QDataStream &operator>> (QDataStream &out) const
    {
        out << m_message_id;
        out << testString;
        return out;
    }

    virtual QTextStream &operator<< (QTextStream &in) { return in; }
    virtual QTextStream &operator>> (QTextStream &out) const { return out; }

protected:

private:
    QString testString;
};

} // namespace BlackMisc

#endif // MESSAGE_H

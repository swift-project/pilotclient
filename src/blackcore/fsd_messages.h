//! Copyright (C) 2013 Roland Winklmeier
//! This Source Code Form is subject to the terms of the Mozilla Public
//! License, v. 2.0. If a copy of the MPL was not distributed with this
//! file, You can obtain one at http://mozilla.org/MPL/2.0/

#ifndef FSD_MESSAGES_H
#define FSD_MESSAGES_H

#include "blackmisc/message_system.h"

#define VATSIM_PROTOCOL_REV 9

// Qt includes
#include <QtGlobal>
#include <QVector>

typedef QVector<QString> QStringVector;

namespace FSD
{
    class FSD_MSG : public BlackMisc::IMessage
    {
    public:
        FSD_MSG(QString id) : IMessage(id)
        {

        }

		void setSource(const QString &source) {m_source = source; }
		void setDest(const QString &destination) {m_destination = destination; }

    protected:
		qint32 unpack(const QString &line, QStringVector &tokens);
		QString         pack(const QStringVector &tokens) const;

        QString m_destination;
        QString m_source;
    };

    class FSD_MSG_AddPilot : public FSD_MSG
    {
    public:
        FSD_MSG_AddPilot() :  FSD_MSG("#AP"), m_revision(VATSIM_PROTOCOL_REV),
							  m_rating(1)
        {
        }

		void setUserID(const QString &userID) { m_userID = userID; }
		void setPassword(const QString &password) { m_password = password; }
		void setSimulator(const quint16 &simulator) {  m_simulator = simulator; }
		void setRealName(const QString &name) { m_realName = name; }

        virtual QDataStream& operator<< (QDataStream& in) { return in; }
        virtual QDataStream& operator>> (QDataStream& out) const { return out; }

        virtual QTextStream& operator<< ( QTextStream& in) 
		{	return in; }
        virtual QTextStream& operator>> (QTextStream& out) const 
		{
			QStringVector tokens;
			tokens << m_source << m_destination << m_userID << m_password;
			tokens << QString("%1").arg(m_rating) <<  QString("%1").arg(m_revision);
			tokens << QString("%1").arg(m_simulator) << m_realName;
			out << pack(tokens);
			return out; 
		}

	private:

		QString m_userID;
		QString m_password;
		quint16	m_rating;
		quint16	m_revision;
		quint16 m_simulator;
		QString m_realName;
    };

    class FSD_MSG_TextMessage : public FSD_MSG
    {
    public:
        FSD_MSG_TextMessage() :  FSD_MSG(QString("#TM"))
        {
        }

        virtual QDataStream& operator<< (QDataStream& in) { return in; }
        virtual QDataStream& operator>> (QDataStream& out) const { return out;}

        virtual QTextStream& operator<< ( QTextStream& in)
        {
            QString message = in.readAll();
            QStringVector tokens;

            //tokens.resize(3);
            unpack(message, tokens);
            m_source = tokens.at(0);
            m_destination = tokens.at(1);
            int size = tokens.size();
			QString m_textmessage;
			for ( int ii = 2; ii < tokens.size(); ++ii)
				m_textmessage += tokens.at(ii);
            bInfo << m_textmessage;
            return in;
        }

         virtual QTextStream& operator>> (QTextStream& out) const
        {
            QStringVector tokens;
            tokens << m_source << m_destination;
            out << pack(tokens);
            return out;
        }
    };

    class FSD_MSG_Plane_Position : public FSD_MSG
    {
    public:

        FSD_MSG_Plane_Position() :  FSD_MSG(QString("@"))
        {
			m_message_tokens.resize(10);
        }

        inline QString SquawkMode() const   { return m_message_tokens.at(0); }
        inline QString Callsign() const     { return m_message_tokens.at(1); }
        inline QString Squawk() const       { return m_message_tokens.at(2); }
        inline quint16 Rating() const       { return m_message_tokens.at(3).toUInt(); }
        inline double Latitude() const      { return m_message_tokens.at(4).toDouble(); }
        inline double Longitude() const     { return m_message_tokens.at(5).toDouble(); }
        inline double Altitude() const      { return m_message_tokens.at(6).toDouble(); }
        inline qint32 Speed() const         { return m_message_tokens.at(7).toInt(); }
        inline quint32 PBH() const          { return m_message_tokens.at(8).toUInt(); }
        inline qint32 AltDiff() const       { return m_message_tokens.at(9).toInt(); }

        inline void setSquawkMode( const QString &squawk_mode)    { m_message_tokens.replace(0, squawk_mode); }
        inline void setCallsign  ( const QString &callsign)       { m_message_tokens.replace(1, callsign); }
        inline void setSquawk    ( const QString &squawk)         { m_message_tokens.replace(2, squawk); }
        inline void setRating    ( const quint16 rating)          { m_message_tokens.replace(3, QString("%1").arg(rating)); }
        inline void setLatitude  ( const double latitude)         { m_message_tokens.replace(4, QString("%1").arg(latitude)); }
        inline void setLongitude ( const double longitude)        { m_message_tokens.replace(5, QString("%1").arg(longitude)); }
        inline void setAltitude  ( const double altitude)         { m_message_tokens.replace(6, QString("%1").arg(altitude)); }
        inline void setSpeed     ( const qint32 speed)            { m_message_tokens.replace(7, QString("%1").arg(speed)); }
        inline void setPBH       ( const quint32 pbh)             { m_message_tokens.replace(8, QString("%1").arg(pbh)); }
        inline void setAltDiff   ( const qint32 altdiff)          { m_message_tokens.replace(9, QString("%1").arg(altdiff)); }

        virtual QTextStream& operator<< ( QTextStream& in);
        virtual QTextStream& operator>> (QTextStream& out) const;

        virtual QDataStream& operator<< (QDataStream& in) { return in; }
        virtual QDataStream& operator>> (QDataStream& out) const { return out;}

    private:
        QStringVector   m_message_tokens;
    };

} // namespace FSD

#endif // FSD_MESSAGES_H

//! Copyright (C) 2013 Roland Winklmeier
//! This Source Code Form is subject to the terms of the Mozilla Public
//! License, v. 2.0. If a copy of the MPL was not distributed with this
//! file, You can obtain one at http://mozilla.org/MPL/2.0/

#ifndef MULTIPLAYER_H
#define MULTIPLAYER_H

#include <QHash>
#include <QString>
#include <qglobal.h>
#include <blackmisc/message_system.h>
#include <blackcore/fsd_messages.h>

namespace BlackCore {

	class CPlane;
	class ISimulator;

	union FS_PBH {
		quint32 pbh;
		struct {
			quint32 unused   : 1;
			quint32 onground : 1;
			quint32 hdg      : 10;
			quint32 bank     : 10;
			quint32 pitch    : 10;
		};
	};

	class CMultiPlayer : public BlackMisc::CMessageHandler
	{
	public:
		CMultiPlayer();

		//! Starts the multiplayer mode.
		void start ();

		//! Stops the multiplayer mode.
		void stop ();

		//! Returns true if the multiplayer mode has been started. Otherwise false. \sa start()
		/*!
		  \return Return true if running, otherwise false.
		*/
		inline bool isRunning() const { return m_isRunning; }

		//! This is the trigger, to do all frequent calculations and send everything to the simulator.
		//! CMultiPlayer does nothing by itself, the parent has to call this.
		void run();

		//! Returns true the plane is known, othwewise false.
		/*!
		  \return Return true if the given plane is in the hash map, otherwise false.
		*/
		bool isKnown(const QString &callsign) const;
	
		//! Enables or disables the injection of AI planes
		/*!
		  \param enable If enable is true, injection is enabled, otherwise it will be disabled
		*/
		void enableAIPlanes(bool enable);
	
		//! Returns true if rendering of AI planes is enabled
		/*!
		  \return Return true if enabled, otherwise false.
		*/
		bool areAIPlanesEnabled() const;


		//! Use this method if you need a specific plane object.
		/*!
		  \param callsign Callsign of the players plane
		  \return Returns the pointer to the Plane object.
		*/
		CPlane *getPlane ( const QString &callsign);

	private:

		void onPositionUpdate(const FSD::FSD_MSG_Plane_Position *plane_position);

		void addPlane(CPlane *plane);

		void removePlane(CPlane *plane);

		bool needsToRemoved(CPlane *plane);

		typedef QHash<QString, CPlane*> TPlaneManager;
		TPlaneManager   m_multiplayer_planes;

		bool            m_isRunning;
	
		bool			m_enableAIPlanes;

		ISimulator      *m_simulator;
	}; 

} //! namespace BlackCore

#endif // MULTIPLAYER_H

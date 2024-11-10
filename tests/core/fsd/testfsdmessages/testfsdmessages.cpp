// SPDX-FileCopyrightText: Copyright (C) 2018 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

//! \cond PRIVATE_TESTS

/*!
 * \file
 * \ingroup testblackfsd
 */

#include "config/buildconfig.h"
#include "core/fsd/addatc.h"
#include "core/fsd/addpilot.h"
#include "core/fsd/atcdataupdate.h"
#include "core/fsd/authchallenge.h"
#include "core/fsd/authresponse.h"
#include "core/fsd/clientidentification.h"
#include "core/fsd/deleteatc.h"
#include "core/fsd/deletepilot.h"
#include "core/fsd/euroscopesimdata.h"
#include "core/fsd/pbh.h"
#include "core/fsd/pilotdataupdate.h"
#include "core/fsd/ping.h"
#include "core/fsd/pong.h"
#include "core/fsd/killrequest.h"
#include "core/fsd/textmessage.h"
#include "core/fsd/clientquery.h"
#include "core/fsd/clientresponse.h"
#include "core/fsd/flightplan.h"
#include "core/fsd/fsdidentification.h"
#include "core/fsd/serializer.h"
#include "core/fsd/servererror.h"
#include "core/fsd/interimpilotdataupdate.h"
#include "core/fsd/visualpilotdataupdate.h"
#include "core/fsd/visualpilotdatatoggle.h"
#include "core/fsd/planeinforequest.h"
#include "core/fsd/planeinformation.h"
#include "core/fsd/planeinforequestfsinn.h"
#include "core/fsd/planeinformationfsinn.h"
#include "core/fsd/enums.h"
#include "test.h"

#include <QObject>
#include <QTest>

using namespace swift::misc::aviation;
using namespace swift::misc::network;
using namespace swift::config;
using namespace swift::core::fsd;

namespace MiscTest
{
    //! Testing FSD Messages
    class CTestFsdMessages : public QObject
    {
        Q_OBJECT

    public:
        //! Constructor
        explicit CTestFsdMessages(QObject *parent = nullptr) : QObject(parent) {}

        //! Destructor
        virtual ~CTestFsdMessages() {}

    private slots:
        void testAddAtc();
        void testAddPilot();
        void testAtcDataUpdate();
        void testAuthChallenge();
        void testAuthResponse();
        void testClientIdentification();
        void testClientQuery();
        void testClientResponse();
        void testDeleteAtc();
        void testDeletePilot();
        void testEuroscopeSimData();
        void testFlightPlan();
        void testFSDIdentification();
        void testInterimPilotDataUpdate();
        void testKillRequest();
        void testPBH();
        void testPilotDataUpdate();
        void testVisualPilotDataUpdate();
        void testVisualPilotDataToggle();
        void testPing();
        void testPlaneInfoRequest();
        void testPlaneInformation();
        void testPlaneInfoRequestFsinn();
        void testPlaneInformationFsinn();
        void testPong();
        void testServerError();
        void testTextMessage();
    };

    void CTestFsdMessages::testAddAtc()
    {
        const AddAtc message("ABCD", "Jon Doe", "1234567", "1234567", AtcRating::Student3, 100);

        QCOMPARE(message.sender(), "ABCD");
        QCOMPARE(message.receiver(), "SERVER");
        QCOMPARE(message.cid(), "1234567");
        QCOMPARE(message.password(), "1234567");
        QCOMPARE(message.rating(), AtcRating::Student3);
        QCOMPARE(message.protocolRevision(), 100);
        QCOMPARE(message.realName(), "Jon Doe");

        QString stringRef("ABCD:SERVER:Jon Doe:1234567:1234567:4:100");
        QString str = message.toTokens().join(":");
        QCOMPARE(str, stringRef);

        QStringList tokens = QString("ABCD:SERVER:Jon Doe:1234567:1234567:4:100").split(':');
        const AddAtc messageFromTokens = AddAtc::fromTokens(tokens);
        QCOMPARE(messageFromTokens, message);
    }

    void CTestFsdMessages::testAddPilot()
    {
        const AddPilot message("ABCD", "1234567", "1234567", PilotRating::Student, 100, SimType::MSFS95, "Jon Doe");
        QCOMPARE(message.sender(), QString("ABCD"));
        QCOMPARE(message.receiver(), QString("SERVER"));
        QCOMPARE(message.cid(), QString("1234567"));
        QCOMPARE(message.password(), QString("1234567"));
        QCOMPARE(message.rating(), PilotRating::Student);
        QCOMPARE(message.protocolVersion(), 100);
        QCOMPARE(message.realName(), QString("Jon Doe"));
        QCOMPARE(message.simType(), SimType::MSFS95);

        QString stringRef("ABCD:SERVER:1234567:1234567:1:100:1:Jon Doe");
        QString str = message.toTokens().join(":");
        QCOMPARE(str, stringRef);

        QStringList tokens = QString("ABCD:SERVER:1234567:1234567:1:100:1:Jon Doe").split(':');
        const AddPilot messageFromTokens = AddPilot::fromTokens(tokens);
        QCOMPARE(messageFromTokens, message);
    }

    void CTestFsdMessages::testAtcDataUpdate()
    {
        const AtcDataUpdate message("ABCD", 128200, CFacilityType::APP, 145, AtcRating::Controller1, 48.11028, 8.56972, 100);
        QCOMPARE(message.sender(), QString("ABCD"));
        QCOMPARE(message.receiver(), QString(""));
        QCOMPARE(message.m_frequencykHz, 128200);
        QCOMPARE(message.m_facility, CFacilityType::APP);
        QCOMPARE(message.m_visibleRange, 145);
        QCOMPARE(message.m_rating, AtcRating::Controller1);
        QCOMPARE(message.m_latitude, 48.11028);
        QCOMPARE(message.m_longitude, 8.56972);
        QCOMPARE(message.m_elevation, 100);

        QString stringRef("ABCD:28200:5:145:5:48.11028:8.56972:100");
        QString str = message.toTokens().join(":");
        QCOMPARE(str, stringRef);

        const AtcDataUpdate reference("ABCD", 128200, CFacilityType::APP, 145, AtcRating::Controller1, 48.11028, 8.56972, 100);

        QStringList tokens = QString("ABCD:28200:5:145:5:48.11028:8.56972:100").split(':');
        const AtcDataUpdate messageFromTokens = AtcDataUpdate::fromTokens(tokens);
        QCOMPARE(reference, messageFromTokens);
    }

    void CTestFsdMessages::testAuthChallenge()
    {
        const AuthChallenge message("ABCD", "SERVER", "7a57f2dd9d360d347b");
        QCOMPARE(message.sender(), QString("ABCD"));
        QCOMPARE(message.receiver(), QString("SERVER"));
        QCOMPARE(QString("7a57f2dd9d360d347b"), message.m_challengeKey);

        const QString stringRef("ABCD:SERVER:7a57f2dd9d360d347b");
        const QString str = message.toTokens().join(":");
        QCOMPARE(str, stringRef);

        const QStringList tokens = QString("ABCD:SERVER:7a57f2dd9d360d347b").split(':');
        const AuthChallenge messageFromTokens = AuthChallenge::fromTokens(tokens);
        QCOMPARE(messageFromTokens, message);
    }

    void CTestFsdMessages::testAuthResponse()
    {
        const AuthResponse message("ABCD", "SERVER", "7a57f2dd9d360d347b");
        QCOMPARE(message.sender(), QString("ABCD"));
        QCOMPARE(message.receiver(), QString("SERVER"));
        QCOMPARE(QString("7a57f2dd9d360d347b"), message.m_response);

        const QString stringRef("ABCD:SERVER:7a57f2dd9d360d347b");
        const QString str = message.toTokens().join(":");
        QCOMPARE(str, stringRef);

        const AuthResponse reference("ABCD", "SERVER", "7a57f2dd9d360d347b");

        const QStringList tokens = QString("ABCD:SERVER:7a57f2dd9d360d347b").split(':');
        const AuthResponse messageFromTokens = AuthResponse::fromTokens(tokens);
        QCOMPARE(messageFromTokens, message);
    }

    void CTestFsdMessages::testClientIdentification()
    {
        const ClientIdentification message("ABCD", 0xe410, "Client", 1, 5, "1234567", "1108540872", "29bbc8b1398eb38e0139");
        QCOMPARE(message.sender(), QString("ABCD"));
        QCOMPARE(message.receiver(), QString("SERVER"));
        QCOMPARE(0xe410, message.m_clientId);
        QCOMPARE("Client", message.m_clientName);
        QCOMPARE(1, message.m_clientVersionMajor);
        QCOMPARE(5, message.m_clientVersionMinor);
        QCOMPARE(QString("1234567"), message.m_userCid);
        QCOMPARE(QString("1108540872"), message.m_sysUid);
        QCOMPARE(QString("29bbc8b1398eb38e0139"), message.m_initialChallenge);

        QString stringRef("ABCD:SERVER:e410:Client:1:5:1234567:1108540872:29bbc8b1398eb38e0139");
        QString str = message.toTokens().join(":");
        QCOMPARE(str, stringRef);

        const ClientIdentification reference("ABCD", 0xe410, "Client", 1, 5, "1234567", "1108540872", "29bbc8b1398eb38e0139");

        const QStringList tokens = QString("ABCD:SERVER:e410:Client:1:5:1234567:1108540872:29bbc8b1398eb38e0139").split(':');
        const ClientIdentification messageFromTokens = ClientIdentification::fromTokens(tokens);
        QCOMPARE(messageFromTokens, message);
    }

    void CTestFsdMessages::testClientQuery()
    {
        //        const ClientQuery message("ABCD", "@94835", Client::WhoIsTracking);
        //        QCOMPARE(message.sender(), QString("ABCD"));
        //        QCOMPARE(QString("@94835"), message.receiver());
        //        QCOMPARE(ClientQueryType::WhoIsTracking, message.m_queryType);
        //        QCOMPARE(QStringList(), message.m_payload);

        //        const ClientQuery pdu2("ABCD", "@94835", ClientQueryType::WhoIsTracking, {"LHA449"});
        //        QCOMPARE(QString("ABCD"), pdu2.sender());
        //        QCOMPARE(QString("@94835"), pdu2.receiver());
        //        QCOMPARE(ClientQueryType::WhoIsTracking, pdu2.m_queryType);
        //        QCOMPARE(QStringList {"LHA449"}, pdu2.m_payload);

        //        const ClientQuery message("ABCD", "@94835", ClientQueryType::WhoIsTracking, {"LHA449"});
        //        QString stringRef("ABCD:@94835:WH:LHA449");
        //        QString str = message.toTokens().join(":");
        //        QCOMPARE(str, stringRef);

        //        const ClientQuery reference("ABCD", "@94835", ClientQueryType::WhoIsTracking, {"LHA449"});

        //        QStringList tokens = QString("ABCD:@94835:WH:LHA449").split(':');
        //        const ClientQuery messageFromTokens = ClientQuery::fromTokens(tokens);
        //        QCOMPARE(messageFromTokens, message);
    }

    void CTestFsdMessages::testClientResponse()
    {
        const ClientResponse message("ABCD", "SERVER", ClientQueryType::Capabilities, { "MODELDESC=1", "ATCINFO=1" });
        QCOMPARE(message.sender(), QString("ABCD"));
        QCOMPARE(message.receiver(), QString("SERVER"));
        QCOMPARE(ClientQueryType::Capabilities, message.m_queryType);
        QStringList reference { "MODELDESC=1", "ATCINFO=1" };
        QCOMPARE(reference, message.m_responseData);

        QString stringRef("ABCD:SERVER:CAPS:MODELDESC=1:ATCINFO=1");
        auto str = message.toTokens().join(":");
        QCOMPARE(str, stringRef);

        QStringList tokens = QString("ABCD:SERVER:CAPS:MODELDESC=1:ATCINFO=1").split(':');
        auto messageFromTokens = ClientResponse::fromTokens(tokens);
        QCOMPARE(messageFromTokens, message);
    }

    void CTestFsdMessages::testDeleteAtc()
    {
        const DeleteAtc message("ABCD", "1234567");

        QCOMPARE(message.sender(), QString("ABCD"));
        QCOMPARE(QString(""), message.receiver());
        QCOMPARE(QString("1234567"), message.m_cid);

        QString stringRef("ABCD:1234567");
        QString str = message.toTokens().join(":");
        QCOMPARE(str, stringRef);

        QStringList tokens = QString("ABCD:1234567").split(':');
        auto messageFromTokens = DeleteAtc::fromTokens(tokens);
        QCOMPARE(messageFromTokens, message);
    }

    void CTestFsdMessages::testDeletePilot()
    {
        const DeletePilot message("ABCD", "1234567");

        QCOMPARE(message.sender(), QString("ABCD"));
        QCOMPARE(QString(""), message.receiver());
        QCOMPARE(QString("1234567"), message.m_cid);

        QString stringRef("ABCD:1234567");
        QString str = message.toTokens().join(":");
        QCOMPARE(str, stringRef);

        QStringList tokens = QString("ABCD:1234567").split(':');
        auto messageFromTokens = DeletePilot::fromTokens(tokens);
        QCOMPARE(messageFromTokens, message);
    }

    void CTestFsdMessages::testEuroscopeSimData()
    {
        const EuroscopeSimData message("ABCD", "A320", "DLH", 0, 43.1257800, -72.1584100, 12000, 180, 10, -10, 250, false, 0, 50, {});

        QCOMPARE(message.sender(), QString("ABCD"));
        QCOMPARE(QString(""), message.receiver());
        QCOMPARE(43.12578, message.m_latitude);
        QCOMPARE(-72.15841, message.m_longitude);
        QCOMPARE(12000, message.m_altitude);
        QCOMPARE(180, message.m_heading);
        QCOMPARE(10, message.m_bank);
        QCOMPARE(-10, message.m_pitch);
        QCOMPARE(250, message.m_groundSpeed);
        QCOMPARE(false, message.m_onGround);
        QCOMPARE(0, message.m_gearPercent);
        QCOMPARE(50, message.m_thrustPercent);
        QCOMPARE(CAircraftLights(), message.m_lights);

        QString stringRef(":ABCD:A320:DLH:0:43.1257800:-72.1584100:12000.0:180.00:10:-10:250:0:0:50:0:0.0:0");
        QString str = message.toTokens().join(":");
        QCOMPARE(str, stringRef);

        QStringList tokens = QString(":ABCD:A320:DLH:0:43.1257800:-72.1584100:12000:180.00:10:-10:250:0:0:50:0:0.0:0").split(':');
        auto messageFromTokens = EuroscopeSimData::fromTokens(tokens);
        QCOMPARE(QString("ABCD"), messageFromTokens.sender());
        QCOMPARE(QString(""), messageFromTokens.receiver());
        QCOMPARE(43.12578, messageFromTokens.m_latitude);
        QCOMPARE(-72.15841, messageFromTokens.m_longitude);
        QCOMPARE(12000, messageFromTokens.m_altitude);
        QCOMPARE(180, messageFromTokens.m_heading);
        QCOMPARE(10, messageFromTokens.m_bank);
        QCOMPARE(-10, messageFromTokens.m_pitch);
        QCOMPARE(250, messageFromTokens.m_groundSpeed);
        QCOMPARE(false, messageFromTokens.m_onGround);
        QCOMPARE(0, messageFromTokens.m_gearPercent);
        QCOMPARE(50, messageFromTokens.m_thrustPercent);
        QCOMPARE(CAircraftLights(), messageFromTokens.m_lights);
    }

    void CTestFsdMessages::testFlightPlan()
    {
        const FlightPlan message("ABCD", "SERVER", FlightType::VFR, "B744", 420, "EGLL", 1530, 1535, "FL350", "KORD", 8, 15,
                                 9, 30, "NONE", "Unit Test", "EGLL.KORD");

        QCOMPARE(message.sender(), QString("ABCD"));
        QCOMPARE(message.receiver(), QString("SERVER"));
        QCOMPARE(FlightType::VFR, message.m_flightType);
        QCOMPARE(QString("B744"), message.m_aircraftIcaoType);
        QCOMPARE(420, message.m_trueCruisingSpeed);
        QCOMPARE(QString("EGLL"), message.m_depAirport);
        QCOMPARE(1530, message.m_estimatedDepTime);
        QCOMPARE(1535, message.m_actualDepTime);
        QCOMPARE(QString("FL350"), message.m_cruiseAlt);
        QCOMPARE(QString("KORD"), message.m_destAirport);
        QCOMPARE(8, message.m_hoursEnroute);
        QCOMPARE(15, message.m_minutesEnroute);
        QCOMPARE(9, message.m_fuelAvailHours);
        QCOMPARE(30, message.m_fuelAvailMinutes);
        QCOMPARE(QString("NONE"), message.m_altAirport);
        QCOMPARE(QString("Unit Test"), message.m_remarks);
        QCOMPARE(QString("EGLL.KORD"), message.m_route);

        QString stringRef("ABCD:SERVER:V:B744:420:EGLL:1530:1535:FL350:KORD:8:15:9:30:NONE:Unit Test:EGLL.KORD");
        QString str = message.toTokens().join(":");
        QCOMPARE(str, stringRef);

        QStringList tokens = QString("ABCD:SERVER:V:B744:420:EGLL:1530:1535:FL350:KORD:8:15:9:30:NONE:Unit Test:EGLL.KORD").split(':');
        auto messageFromTokens = FlightPlan::fromTokens(tokens);
        QCOMPARE(messageFromTokens, message);
    }

    void CTestFsdMessages::testFSDIdentification()
    {
    }

    void CTestFsdMessages::testInterimPilotDataUpdate()
    {
        const InterimPilotDataUpdate message("ABCD", "XYZ", 43.12578, -72.15841, 12008, 400, -2, 3, 280, true);

        QCOMPARE(message.sender(), QString("ABCD"));
        QCOMPARE(QString("XYZ"), message.receiver());
        QCOMPARE(43.12578, message.m_latitude);
        QCOMPARE(-72.15841, message.m_longitude);
        QCOMPARE(12008, message.m_altitudeTrue);
        QCOMPARE(400, message.m_groundSpeed);
        QCOMPARE(-2, message.m_pitch);
        QCOMPARE(3, message.m_bank);
        QCOMPARE(280, message.m_heading);
        QCOMPARE(true, message.m_onGround);

        QString stringRef("ABCD:XYZ:VI:43.12578:-72.15841:12008:400:25132146");
        QString str = message.toTokens().join(":");
        QCOMPARE(str, stringRef);

        QStringList tokens = QString("ABCD:XYZ:VI:43.12578:-72.15841:12008:400:25132146").split(':');
        auto messageFromTokens = InterimPilotDataUpdate::fromTokens(tokens);
        QCOMPARE(QString("ABCD"), messageFromTokens.sender());
        QCOMPARE(QString("XYZ"), messageFromTokens.receiver());
        QCOMPARE(43.12578, messageFromTokens.m_latitude);
        QCOMPARE(-72.15841, messageFromTokens.m_longitude);
        QCOMPARE(12008, messageFromTokens.m_altitudeTrue);
        QCOMPARE(400, messageFromTokens.m_groundSpeed);
        QVERIFY(message.m_pitch - messageFromTokens.m_pitch < 1.0);
        QVERIFY(message.m_bank - messageFromTokens.m_bank < 1.0);
        QVERIFY(message.m_heading - messageFromTokens.m_heading < 1.0);
        QCOMPARE(messageFromTokens.m_onGround, true);
    }

    void CTestFsdMessages::testKillRequest()
    {
        const KillRequest message("SUP", "ABCD", "I don't like you!");

        QCOMPARE(QString("SUP"), message.sender());
        QCOMPARE(QString("ABCD"), message.receiver());
        QCOMPARE(QString("I don't like you!"), message.m_reason);

        QString stringRef("SUP:ABCD:I don't like you!");
        QString str = message.toTokens().join(":");
        QCOMPARE(str, stringRef);

        QStringList tokens = QString("SUP:ABCD:I don't like you!").split(':');
        auto messageFromTokens = KillRequest::fromTokens(tokens);
        QCOMPARE(messageFromTokens, message);
    }

    void CTestFsdMessages::testPBH()
    {
        struct PBH
        {
            int pitch, bank, heading;
        };
        QVector<PBH> testData;
        for (int pitch = -90; pitch < 90; pitch++) { testData.push_back({ pitch, 0, 0 }); }
        for (int bank = -179; bank < 180; bank++) { testData.push_back({ 0, bank, 0 }); }
        for (int heading = 0; heading < 360; heading++) { testData.push_back({ 0, 0, heading }); }
        for (const auto &input : testData)
        {
            std::uint32_t pbh = 0;
            packPBH(input.pitch, input.bank, input.heading, true, pbh);

            double pitch2 = 0;
            double bank2 = 0;
            double heading2 = 0;
            bool onGround2 = false;
            unpackPBH(pbh, pitch2, bank2, heading2, onGround2);
            QVERIFY(pitch2 >= -90);
            QVERIFY(pitch2 < 90);
            QVERIFY(bank2 >= -180);
            QVERIFY(bank2 < 180);
            QVERIFY(heading2 >= 0);
            QVERIFY(heading2 < 360);
            QCOMPARE(input.pitch, pitch2);
            QCOMPARE(input.bank, bank2);
            QCOMPARE(true, onGround2);

            if (input.heading < 0) { heading2 -= 360; }
            QVERIFY(qAbs(input.heading - heading2) < 1);
        }
    }

    void CTestFsdMessages::testPilotDataUpdate()
    {
        const PilotDataUpdate message(CTransponder::ModeC, "ABCD", 7000, PilotRating::Student, 43.12578, -72.15841, 12000, 12008,
                                      125, -2, 3, 280, true);

        QCOMPARE(message.sender(), QString("ABCD"));
        QCOMPARE(QString(""), message.receiver());
        QCOMPARE(CTransponder::ModeC, message.m_transponderMode);
        QCOMPARE(7000, message.m_transponderCode);
        QCOMPARE(PilotRating::Student, message.m_rating);
        QCOMPARE(43.12578, message.m_latitude);
        QCOMPARE(-72.15841, message.m_longitude);
        QCOMPARE(12000, message.m_altitudeTrue);
        QCOMPARE(12008, message.m_altitudePressure);
        QCOMPARE(125, message.m_groundSpeed);
        QCOMPARE(-2, message.m_pitch);
        QCOMPARE(3, message.m_bank);
        QCOMPARE(280, message.m_heading);
        QCOMPARE(true, message.m_onGround);

        QString stringRef("N:ABCD:7000:1:43.12578:-72.15841:12000:125:25132146:8");
        QString str = message.toTokens().join(":");
        QCOMPARE(str, stringRef);

        QStringList tokens = QString("N:ABCD:7000:1:43.12578:-72.15841:12000:125:25132146:8").split(':');
        auto messageFromTokens = PilotDataUpdate::fromTokens(tokens);
        QCOMPARE(QString("ABCD"), messageFromTokens.sender());
        QCOMPARE(QString(""), messageFromTokens.receiver());
        QCOMPARE(CTransponder::ModeC, messageFromTokens.m_transponderMode);
        QCOMPARE(7000, messageFromTokens.m_transponderCode);
        QCOMPARE(PilotRating::Student, messageFromTokens.m_rating);
        QCOMPARE(43.12578, messageFromTokens.m_latitude);
        QCOMPARE(-72.15841, messageFromTokens.m_longitude);
        QCOMPARE(12000, messageFromTokens.m_altitudeTrue);
        QCOMPARE(12008, messageFromTokens.m_altitudePressure);
        QCOMPARE(125, messageFromTokens.m_groundSpeed);
        QVERIFY(message.m_pitch - messageFromTokens.m_pitch < 1.0);
        QVERIFY(message.m_bank - messageFromTokens.m_bank < 1.0);
        QVERIFY(message.m_heading - messageFromTokens.m_heading < 1.0);
        QCOMPARE(messageFromTokens.m_onGround, true);
    }

    void CTestFsdMessages::testVisualPilotDataUpdate()
    {
        const VisualPilotDataUpdate message("ABCD", 43.1257891, -72.1584142, 12000.12, 1404.00, -2, 3, 280, -1.0001, 2.0001, 3.0001, -0.0349, 0.0524, 0.0175);

        QCOMPARE(QString("ABCD"), message.sender());
        QCOMPARE(43.1257891, message.m_latitude);
        QCOMPARE(-72.1584142, message.m_longitude);
        QCOMPARE(12000.12, message.m_altitudeTrue);
        QCOMPARE(1404.00, message.m_heightAgl);
        QCOMPARE(-2, message.m_pitch);
        QCOMPARE(3, message.m_bank);
        QCOMPARE(280, message.m_heading);
        QCOMPARE(-1.0001, message.m_xVelocity);
        QCOMPARE(2.0001, message.m_yVelocity);
        QCOMPARE(3.0001, message.m_zVelocity);
        QCOMPARE(-0.0349, message.m_pitchRadPerSec);
        QCOMPARE(0.0524, message.m_bankRadPerSec);
        QCOMPARE(0.0175, message.m_headingRadPerSec);
        QCOMPARE(0.0, message.m_noseGearAngle);

        QString stringRef("ABCD:43.1257891:-72.1584142:12000.12:1404.00:25132144:-1.0001:2.0001:3.0001:-0.0349:0.0175:0.0524:0.00");
        QString str = message.toTokens().join(":");
        QCOMPARE(str, stringRef);

        QStringList tokens = QString("ABCD:43.1257891:-72.1584142:12000.12:1404.00:25132144:-1.0001:2.0001:3.0001:-0.0349:0.0175:0.0524:0.00").split(':');
        auto messageFromTokens = VisualPilotDataUpdate::fromTokens(tokens);
        QCOMPARE(QString("ABCD"), messageFromTokens.sender());
        QCOMPARE(43.1257891, messageFromTokens.m_latitude);
        QCOMPARE(-72.1584142, messageFromTokens.m_longitude);
        QCOMPARE(12000.12, messageFromTokens.m_altitudeTrue);
        QCOMPARE(1404.00, messageFromTokens.m_heightAgl);
        QVERIFY(message.m_pitch - messageFromTokens.m_pitch < 1.0);
        QVERIFY(message.m_bank - messageFromTokens.m_bank < 1.0);
        QVERIFY(message.m_heading - messageFromTokens.m_heading < 1.0);
        QCOMPARE(-1.0001, messageFromTokens.m_xVelocity);
        QCOMPARE(2.0001, messageFromTokens.m_yVelocity);
        QCOMPARE(3.0001, messageFromTokens.m_zVelocity);
        QCOMPARE(-0.0349, messageFromTokens.m_pitchRadPerSec);
        QCOMPARE(0.0524, messageFromTokens.m_bankRadPerSec);
        QCOMPARE(0.0175, messageFromTokens.m_headingRadPerSec);
        QCOMPARE(0.0, messageFromTokens.m_noseGearAngle);
    }

    void CTestFsdMessages::testVisualPilotDataToggle()
    {
        const VisualPilotDataToggle message("SERVER", "ABCD", true);

        QCOMPARE(QString("SERVER"), message.sender());
        QCOMPARE(QString("ABCD"), message.m_client);
        QCOMPARE(true, message.m_active);

        QString stringRef("SERVER:ABCD:1");
        QString str = message.toTokens().join(":");
        QCOMPARE(str, stringRef);

        QStringList tokens = QString("SERVER:ABCD:1").split(':');
        auto messageFromTokens = VisualPilotDataToggle::fromTokens(tokens);
        QCOMPARE(QString("SERVER"), messageFromTokens.sender());
        QCOMPARE(QString("ABCD"), messageFromTokens.m_client);
        QCOMPARE(true, messageFromTokens.m_active);
    }

    void CTestFsdMessages::testPing()
    {
        const Ping message("ABCD", "SERVER", "85275222");
        QCOMPARE(message.sender(), QString("ABCD"));
        QCOMPARE(message.receiver(), QString("SERVER"));
        QCOMPARE("85275222", message.m_timestamp);

        QString stringRef("ABCD:SERVER:85275222");
        QString str = message.toTokens().join(":");
        QCOMPARE(str, stringRef);

        QStringList tokens = QString("ABCD:SERVER:85275222").split(':');
        auto messageFromTokens = Ping::fromTokens(tokens);
        QCOMPARE(messageFromTokens, message);
    }

    void CTestFsdMessages::testPlaneInfoRequest()
    {
        const PlaneInfoRequest message("ABCD", "XYZ");
        QCOMPARE(message.sender(), QString("ABCD"));
        QCOMPARE(QString("XYZ"), message.receiver());

        QString stringRef("ABCD:XYZ:PIR");
        QString str = message.toTokens().join(":");
        QCOMPARE(str, stringRef);

        QStringList tokens = QString("ABCD:XYZ:PIR").split(':');
        auto messageFromTokens = PlaneInfoRequest::fromTokens(tokens);
        QCOMPARE(messageFromTokens, message);
    }

    void CTestFsdMessages::testPlaneInformation()
    {
        const PlaneInformation message("ABCD", "XYZ", "B744", "BAW", "UNION");
        QCOMPARE(message.sender(), QString("ABCD"));
        QCOMPARE(QString("XYZ"), message.receiver());
        QCOMPARE(QString("B744"), message.m_aircraft);
        QCOMPARE(QString("BAW"), message.m_airline);
        QCOMPARE(QString("UNION"), message.m_livery);

        const PlaneInformation message1("ABCD", "XYZ", "B744", "", "");
        QString stringRef1("ABCD:XYZ:PI:GEN:EQUIPMENT=B744");
        QString str1 = message1.toTokens().join(":");
        QCOMPARE(str1, stringRef1);

        const PlaneInformation message2("ABCD", "XYZ", "B744", "BAW", "");
        QString stringRef2("ABCD:XYZ:PI:GEN:EQUIPMENT=B744:AIRLINE=BAW");
        QString str2 = message2.toTokens().join(":");
        QCOMPARE(str2, stringRef2);

        const PlaneInformation message3("ABCD", "XYZ", "B744", "BAW", "UNION");
        QString stringRef3("ABCD:XYZ:PI:GEN:EQUIPMENT=B744:AIRLINE=BAW:LIVERY=UNION");
        QString str3 = message3.toTokens().join(":");
        QCOMPARE(str3, stringRef3);

        const PlaneInformation reference1("ABCD", "XYZ", "B744", "", "");

        QStringList tokens1 = QString("ABCD:XYZ:PI:GEN:EQUIPMENT=B744").split(':');
        auto messageFromTokens1 = PlaneInformation::fromTokens(tokens1);
        QCOMPARE(reference1, messageFromTokens1);

        const PlaneInformation reference2("ABCD", "XYZ", "B744", "BAW", "");

        QStringList tokens2 = QString("ABCD:XYZ:PI:GEN:EQUIPMENT=B744:AIRLINE=BAW").split(':');
        auto messageFromTokens2 = PlaneInformation::fromTokens(tokens2);
        QCOMPARE(reference2, messageFromTokens2);

        const PlaneInformation reference3("ABCD", "XYZ", "B744", "BAW", "UNION");

        QStringList tokens3 = QString("ABCD:XYZ:PI:GEN:EQUIPMENT=B744:AIRLINE=BAW:LIVERY=UNION").split(':');
        auto messageFromTokens3 = PlaneInformation::fromTokens(tokens3);
        QCOMPARE(reference3, messageFromTokens3);
    }

    void CTestFsdMessages::testPlaneInfoRequestFsinn()
    {
        const PlaneInfoRequestFsinn message("ABCD", "XYZ", "DLH", "A320", "L2J", "FLIGHTFACTOR A320 LUFTHANSA D-AIPC");
        QCOMPARE(message.sender(), QString("ABCD"));
        QCOMPARE(QString("XYZ"), message.receiver());
        QCOMPARE(QString("DLH"), message.m_airlineIcao);
        QCOMPARE(QString("A320"), message.m_aircraftIcao);
        QCOMPARE(QString("L2J"), message.m_aircraftIcaoCombinedType);
        QCOMPARE(QString("FLIGHTFACTOR A320 LUFTHANSA D-AIPC"), message.m_sendMModelString);

        QString stringRef("ABCD:XYZ:FSIPIR:0:DLH:A320:::::L2J:FLIGHTFACTOR A320 LUFTHANSA D-AIPC");
        QString str = message.toTokens().join(":");
        QCOMPARE(str, stringRef);
    }

    void CTestFsdMessages::testPlaneInformationFsinn()
    {
        const PlaneInformationFsinn message("ABCD", "XYZ", "DLH", "A320", "L2J", "FLIGHTFACTOR A320 LUFTHANSA D-AIPC");
        QCOMPARE(message.sender(), QString("ABCD"));
        QCOMPARE(QString("XYZ"), message.receiver());
        QCOMPARE(QString("DLH"), message.m_airlineIcao);
        QCOMPARE(QString("A320"), message.m_aircraftIcao);
        QCOMPARE(QString("L2J"), message.m_aircraftIcaoCombinedType);
        QCOMPARE(QString("FLIGHTFACTOR A320 LUFTHANSA D-AIPC"), message.m_sendMModelString);

        QString stringRef("ABCD:XYZ:FSIPI:0:DLH:A320:::::L2J:FLIGHTFACTOR A320 LUFTHANSA D-AIPC");
        QString str = message.toTokens().join(":");
        QCOMPARE(str, stringRef);
    }

    void CTestFsdMessages::testPong()
    {
        const Pong message("ABCD", "SERVER", "85275222");
        QCOMPARE(message.sender(), QString("ABCD"));
        QCOMPARE(message.receiver(), QString("SERVER"));
        QCOMPARE("85275222", message.m_timestamp);

        QString stringRef("ABCD:SERVER:85275222");
        QString str = message.toTokens().join(":");
        QCOMPARE(str, stringRef);

        const Pong reference("ABCD", "SERVER", "85275222");

        QStringList tokens = QString("ABCD:SERVER:85275222").split(':');
        auto messageFromTokens = Pong::fromTokens(tokens);
        QCOMPARE(messageFromTokens, message);
    }

    void CTestFsdMessages::testServerError()
    {
        const ServerError message("SERVER", "ABCD", ServerErrorCode::NoWeatherProfile, "EGLL", "No such weather profile");
        QCOMPARE(QString("SERVER"), message.sender());
        QCOMPARE(QString("ABCD"), message.receiver());
        QCOMPARE(ServerErrorCode::NoWeatherProfile, message.m_errorNumber);
        QCOMPARE("EGLL", message.m_causingParameter);
        QCOMPARE("No such weather profile", message.m_description);

        QString stringRef("SERVER:ABCD:9:EGLL:No such weather profile");
        QString str = message.toTokens().join(":");
        QCOMPARE(str, stringRef);

        QStringList tokens = QString("SERVER:ABCD:009:EGLL:No such weather profile").split(':');
        auto messageFromTokens = ServerError::fromTokens(tokens);
        QCOMPARE(messageFromTokens, message);
    }

    void CTestFsdMessages::testTextMessage()
    {
    }
}

//! main
BLACKTEST_APPLESS_MAIN(MiscTest::CTestFsdMessages);

#include "testfsdmessages.moc"

//! \endcond

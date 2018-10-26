/* Copyright (C) 2013
 * swift Project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

#include "matchingutils.h"
#include "aircraftmodel.h"
#include "blackmisc/compare.h"
#include "blackmisc/comparefunctions.h"
#include "blackmisc/db/datastoreutility.h"
#include "blackmisc/fileutils.h"
#include "blackmisc/logcategory.h"
#include "blackmisc/logcategorylist.h"
#include "blackmisc/statusmessage.h"
#include "blackmisc/verify.h"

#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonValue>
#include <QList>
#include <QString>
#include <QtGlobal>
#include <QStringBuilder>
#include <QFileInfo>

using namespace BlackMisc::Aviation;
using namespace BlackMisc::Db;

namespace BlackMisc
{
    namespace Simulation
    {
        void CAircraftModel::registerMetadata()
        {
            CValueObject<CAircraftModel>::registerMetadata();
            qRegisterMetaType<ModelType>();
        }

        CAircraftModel::CAircraftModel(const QString &model, CAircraftModel::ModelType type) :
            m_modelString(model.trimmed().toUpper()), m_modelType(type)
        {}

        CAircraftModel::CAircraftModel(const QString &model, CAircraftModel::ModelType type, const CAircraftIcaoCode &icao, const CLivery &livery) :
            m_aircraftIcao(icao), m_livery(livery), m_modelString(model.trimmed().toUpper()), m_modelType(type)
        {}

        CAircraftModel::CAircraftModel(const QString &model, CAircraftModel::ModelType type, const QString &description, const CAircraftIcaoCode &icao, const Aviation::CLivery &livery) :
            m_aircraftIcao(icao), m_livery(livery), m_modelString(model.trimmed().toUpper()), m_description(description.trimmed()), m_modelType(type)
        {}

        CAircraftModel::CAircraftModel(const QString &model, CAircraftModel::ModelType type, const CSimulatorInfo &simulator, const QString &name, const QString &description, const CAircraftIcaoCode &icao, const CLivery &livery) :
            m_aircraftIcao(icao), m_livery(livery), m_simulator(simulator), m_modelString(model.trimmed().toUpper()), m_name(name.trimmed()), m_description(description.trimmed()), m_modelType(type)
        { }

        QString CAircraftModel::convertToQString(bool i18n) const
        {
            const QString s =
                (this->hasModelString() ? inApostrophes(m_modelString, true) % QStringLiteral(" ") : QStringLiteral("")) %
                QStringLiteral(" type: '") % this->getModelTypeAsString() %
                QStringLiteral("' ICAO: '") % this->getAircraftIcaoCode().toQString(i18n) %
                QStringLiteral(" CG: ") % this->getCG().valueRoundedWithUnit(1) %
                QStringLiteral("' {") % m_livery.toQString(i18n) %
                QStringLiteral("} file: '") % m_fileName % QStringLiteral("'");
            return s;
        }

        QJsonObject CAircraftModel::toDatabaseJson() const
        {
            QJsonObject obj;

            // filename not in DB
            obj.insert("id", this->getDbKeyAsJsonValue());
            obj.insert("name", this->getName());
            obj.insert("modelstring", QJsonValue(m_modelString));
            obj.insert("description", QJsonValue(m_description));
            obj.insert("mode", QJsonValue(getModelModeAsString().left(1).toUpper())); // clazy:exclude=qstring-left

            // sims
            const CSimulatorInfo sim(getSimulator());
            QString flag = CDatastoreUtility::boolToDbYN(sim.isFSX());
            obj.insert("simfsx", QJsonValue(flag));
            flag = CDatastoreUtility::boolToDbYN(sim.isP3D());
            obj.insert("simp3d", QJsonValue(flag));
            flag = CDatastoreUtility::boolToDbYN(sim.isFS9());
            obj.insert("simfs9", QJsonValue(flag));
            flag = CDatastoreUtility::boolToDbYN(sim.isXPlane());
            obj.insert("simxplane", QJsonValue(flag));

            // foreign keys
            obj.insert("iddistributor", this->getDistributor().getDbKeyAsJsonValue());
            obj.insert("idaircrafticao", this->getAircraftIcaoCode().getDbKeyAsJsonValue());
            obj.insert("idlivery", this->getLivery().getDbKeyAsJsonValue());
            obj.insert("idairlineicao", this->getLivery().getAirlineIcaoCode().getDbKeyAsJsonValue()); // not really needed if livery is complete

            return obj;
        }

        QString CAircraftModel::toDatabaseJsonString(QJsonDocument::JsonFormat format) const
        {
            return QJsonDocument(this->toDatabaseJson()).toJson(format);
        }

        QJsonObject CAircraftModel::toMemoizedJson(MemoHelper::CMemoizer &helper) const
        {
            QJsonObject json;
            const auto meta = introspect<CAircraftModel>().without(MetaFlags<DisabledForJson>());
            meta.forEachMember([ &, this ](auto member)
            {
                auto &&maybeMemo = helper.maybeMemoize(member.in(*this));
                json << std::make_pair(CExplicitLatin1String(member.latin1Name()), std::cref(maybeMemo));
            });
            return json;
        }

        void CAircraftModel::convertFromMemoizedJson(const QJsonObject &json, const MemoHelper::CUnmemoizer &helper)
        {
            const auto meta = introspect<CAircraftModel>().without(MetaFlags<DisabledForJson>());
            meta.forEachMember([ &, this ](auto member)
            {
                auto it = json.find(CExplicitLatin1String(member.latin1Name()));
                if (it != json.end()) { it.value() >> helper.maybeUnmemoize(member.in(*this)).get(); }
            });
        }

        QString CAircraftModel::asHtmlSummary(const QString &separator) const
        {
            static const QString html = "Model: %1 changed: %2%3Simulator: %4 Mode: %5 Distributor: %6%7Aircraft ICAO: %8%9Livery: %10";
            return html
                   .arg(this->getModelStringAndDbKey(), this->getFormattedUtcTimestampYmdhms(), separator,
                        this->getSimulator().toQString(true), this->getModelModeAsString(), this->getDistributor().getIdAndDescription(), separator,
                        this->getAircraftIcaoCode().asHtmlSummary(), separator)
                   .arg(this->getLivery().asHtmlSummary("&nbsp;")).replace(" ", "&nbsp;");
        }

        CStatusMessageList CAircraftModel::verifyModelData() const
        {
            CStatusMessageList msgs;
            const ModelType t = this->getModelType();
            if (t == TypeOwnSimulatorModel || t == TypeManuallySet || t == TypeDatabaseEntry)
            {
                if (!this->hasExistingCorrespondingFile())
                {
                    const CStatusMessage m = CStatusMessage(this).validationError("File '%1' not readable") << this->getFileName();
                    msgs.push_back(m);
                }
            }
            else
            {
                const CStatusMessage m = CStatusMessage(this).validationError("Invalid model type to check: '%1'") << this->getModelTypeAsString();
                msgs.push_back(m);
            }
            return msgs;
        }

        bool CAircraftModel::canInitializeFromFsd() const
        {
            const bool nw = this->getModelType() == CAircraftModel::TypeQueriedFromNetwork ||
                            this->getModelType() == CAircraftModel::TypeFSInnData ||
                            this->getModelType() == CAircraftModel::TypeUnknown;
            return nw;
        }

        void CAircraftModel::setCallsign(const CCallsign &callsign)
        {
            m_callsign = callsign;
            m_callsign.setTypeHint(CCallsign::Aircraft);
        }

        QString CAircraftModel::getModelStringAndDbKey() const
        {
            if (this->hasValidDbKey())
            {
                return this->hasModelString() ?
                       QString(this->getModelString() + this->getDbKeyAsStringInParentheses(" ")) :
                       this->getDbKeyAsString();
            }
            else
            {
                return this->getModelString();
            }
        }

        bool CAircraftModel::isVtol() const
        {
            return this->getAircraftIcaoCode().isVtol();
        }

        CVariant CAircraftModel::propertyByIndex(const BlackMisc::CPropertyIndex &index) const
        {
            if (index.isMyself()) { return CVariant::from(*this); }
            if (IDatastoreObjectWithIntegerKey::canHandleIndex(index)) { return IDatastoreObjectWithIntegerKey::propertyByIndex(index); }
            if (IOrderable::canHandleIndex(index)) { return IOrderable::propertyByIndex(index);}

            const ColumnIndex i = index.frontCasted<ColumnIndex>();
            switch (i)
            {
            case IndexModelString: return CVariant(m_modelString);
            case IndexHasQueriedModelString: return CVariant::fromValue(this->hasQueriedModelString());
            case IndexModelType: return CVariant::fromValue(m_modelType);
            case IndexModelTypeAsString: return CVariant(this->getModelTypeAsString());
            case IndexModelMode: return CVariant::fromValue(m_modelMode);
            case IndexModelModeAsString: return CVariant::fromValue(this->getModelModeAsString());
            case IndexModelModeAsIcon: return CVariant::fromValue(this->getModelModeAsIcon());
            case IndexDistributor: return m_distributor.propertyByIndex(index.copyFrontRemoved());
            case IndexSimulatorInfo: return m_simulator.propertyByIndex(index.copyFrontRemoved());
            case IndexSimulatorInfoAsString: return CVariant(m_simulator.toQString());
            case IndexDescription: return CVariant(m_description);
            case IndexName: return CVariant(m_name);
            case IndexFileName: return CVariant(m_fileName);
            case IndexCG: return m_cg.propertyByIndex(index.copyFrontRemoved());
            case IndexFileTimestamp: return CVariant::fromValue(this->getFileTimestamp());
            case IndexFileTimestampFormattedYmdhms: return CVariant::fromValue(this->getFormattedFileTimestampYmdhms());
            case IndexIconPath: return CVariant(m_iconPath);
            case IndexAircraftIcaoCode: return m_aircraftIcao.propertyByIndex(index.copyFrontRemoved());
            case IndexLivery: return m_livery.propertyByIndex(index.copyFrontRemoved());
            case IndexCallsign: return m_callsign.propertyByIndex(index.copyFrontRemoved());
            case IndexMembersDbStatus: return this->getMembersDbStatus();
            default: return CValueObject::propertyByIndex(index);
            }
        }

        void CAircraftModel::setPropertyByIndex(const CPropertyIndex &index, const CVariant &variant)
        {
            if (index.isMyself()) { (*this) = variant.to<CAircraftModel>(); return; }
            if (IOrderable::canHandleIndex(index)) { IOrderable::setPropertyByIndex(index, variant); return; }
            if (IDatastoreObjectWithIntegerKey::canHandleIndex(index)) { IDatastoreObjectWithIntegerKey::setPropertyByIndex(index, variant); return; }

            const ColumnIndex i = index.frontCasted<ColumnIndex>();
            switch (i)
            {
            case IndexModelString: m_modelString = variant.toQString(); break;
            case IndexAircraftIcaoCode: m_aircraftIcao.setPropertyByIndex(index.copyFrontRemoved(), variant); break;
            case IndexLivery: m_livery.setPropertyByIndex(index.copyFrontRemoved(), variant); break;
            case IndexDistributor: m_distributor.setPropertyByIndex(index.copyFrontRemoved(), variant); break;
            case IndexDescription: m_description = variant.toQString(); break;
            case IndexSimulatorInfo: m_simulator.setPropertyByIndex(index.copyFrontRemoved(), variant); break;
            case IndexName: m_name = variant.toQString(); break;
            case IndexIconPath: m_iconPath = variant.toQString(); break;
            case IndexCG: m_cg.setPropertyByIndex(index.copyFrontRemoved(), variant); break;
            case IndexModelType: m_modelType = variant.value<ModelType>(); break;
            case IndexFileName: m_fileName = variant.toQString(); break;
            case IndexCallsign:
                m_callsign.setPropertyByIndex(index.copyFrontRemoved(), variant);
                m_callsign.setTypeHint(CCallsign::Aircraft);
                break;
            case IndexFileTimestamp:
                if (variant.canConvert<QDateTime>())
                {
                    this->setFileTimestamp(variant.value<QDateTime>());
                }
                else if (variant.canConvert<qint64>())
                {
                    m_fileTimestamp = variant.value<qint64>();
                }
                break;
            case IndexModelMode:
                if (variant.type() == QMetaType::QString)
                {
                    this->setModelModeAsString(variant.toQString());
                }
                else
                {
                    m_modelMode = variant.value<ModelMode>();
                }
                break;
            // no setter indexes ignored
            case IndexHasQueriedModelString: break;
            case IndexModelTypeAsString: break;
            case IndexModelModeAsString: break;
            case IndexModelModeAsIcon: break;
            case IndexFileTimestampFormattedYmdhms: break;
            case IndexSimulatorInfoAsString: break;
            case IndexMembersDbStatus: break;

            default: CValueObject::setPropertyByIndex(index, variant); break;
            }
        }

        int CAircraftModel::comparePropertyByIndex(const CPropertyIndex &index, const CAircraftModel &compareValue) const
        {
            if (IDatastoreObjectWithIntegerKey::canHandleIndex(index)) { return IDatastoreObjectWithIntegerKey::comparePropertyByIndex(index, compareValue);}
            if (IOrderable::canHandleIndex(index)) { return IOrderable::comparePropertyByIndex(index, compareValue);}
            if (index.isMyself()) { return m_modelString.compare(compareValue.getModelString(), Qt::CaseInsensitive); }
            const ColumnIndex i = index.frontCasted<ColumnIndex>();
            switch (i)
            {
            case IndexModelString: return m_modelString.compare(compareValue.getModelString(), Qt::CaseInsensitive);
            case IndexHasQueriedModelString: return Compare::compare(this->hasQueriedModelString(), compareValue.hasQueriedModelString());
            case IndexAircraftIcaoCode: return m_aircraftIcao.comparePropertyByIndex(index.copyFrontRemoved(), compareValue.getAircraftIcaoCode());
            case IndexLivery: return m_livery.comparePropertyByIndex(index.copyFrontRemoved(), compareValue.getLivery());
            case IndexDistributor: return m_distributor.comparePropertyByIndex(index.copyFrontRemoved(), compareValue.getDistributor());
            case IndexDescription: return m_description.compare(compareValue.getDescription(), Qt::CaseInsensitive);
            case IndexName: return m_name.compare(compareValue.getName(), Qt::CaseInsensitive);
            case IndexCallsign: return m_callsign.comparePropertyByIndex(index.copyFrontRemoved(), compareValue.getCallsign());
            case IndexFileName: return m_fileName.compare(compareValue.getFileName(), Qt::CaseInsensitive);
            case IndexIconPath: return m_iconPath.compare(compareValue.getIconPath(), Qt::CaseInsensitive);
            case IndexCG: return m_cg.comparePropertyByIndex(index.copyFrontRemoved(), compareValue.getCG());
            case IndexModelTypeAsString:
            case IndexModelType: return Compare::compare(m_modelType, compareValue.getModelType());
            case IndexSimulatorInfoAsString:
            case IndexSimulatorInfo: return m_simulator.comparePropertyByIndex(index.copyFrontRemoved(), compareValue.getSimulator());
            case IndexFileTimestamp:
            case IndexFileTimestampFormattedYmdhms: return Compare::compare(m_fileTimestamp, compareValue.m_fileTimestamp);
            case IndexModelMode:
            case IndexModelModeAsString:
            case IndexModelModeAsIcon: return Compare::compare(m_modelMode, compareValue.getModelMode());
            case IndexMembersDbStatus: return getMembersDbStatus().compare(compareValue.getMembersDbStatus());
            default: break;
            }
            Q_ASSERT_X(false, Q_FUNC_INFO, "No comparison");
            return 0;
        }

        bool CAircraftModel::setAircraftIcaoCode(const CAircraftIcaoCode &aircraftIcaoCode)
        {
            if (m_aircraftIcao == aircraftIcaoCode) { return false; }
            m_aircraftIcao = aircraftIcaoCode;
            return true;
        }

        void CAircraftModel::setAircraftIcaoDesignator(const QString &designator)
        {
            m_aircraftIcao.setDesignator(designator);
        }

        void CAircraftModel::setAircraftIcaoCodes(const CAircraftIcaoCode &aircraftIcaoCode, const CAirlineIcaoCode &airlineIcaoCode)
        {
            m_aircraftIcao = aircraftIcaoCode;
            m_livery.setAirlineIcaoCode(airlineIcaoCode);
        }

        bool CAircraftModel::hasValidAircraftAndAirlineDesignator() const
        {
            return this->hasKnownAircraftDesignator() && m_livery.hasValidAirlineDesignator();
        }

        bool CAircraftModel::hasAircraftDesignator() const
        {
            return m_aircraftIcao.hasDesignator();
        }

        bool CAircraftModel::hasKnownAircraftDesignator() const
        {
            return m_aircraftIcao.hasKnownDesignator();
        }

        bool CAircraftModel::hasAirlineDesignator() const
        {
            return m_livery.hasValidAirlineDesignator();
        }

        bool CAircraftModel::hasAircraftAndAirlineDesignator() const
        {
            return this->hasAircraftDesignator() && this->hasAirlineDesignator();
        }

        bool CAircraftModel::isMilitary() const
        {
            return this->getAircraftIcaoCode().isMilitary() ||
                   this->getLivery().isMilitary();
        }

        bool CAircraftModel::isCivilian() const
        {
            return !this->isMilitary();
        }

        bool CAircraftModel::setDistributorOrder(int order)
        {
            if (order < 0) { return false; }
            if (!m_distributor.isLoadedFromDb()) { return false; }
            m_distributor.setOrder(order);
            return true;
        }

        bool CAircraftModel::setDistributorOrder(const CDistributorList &distributors)
        {
            if (distributors.isEmpty()) { return false; }
            bool found = false;
            const int noDistributorOrder = distributors.size();
            if (this->hasDbDistributor())
            {
                const CDistributor d = distributors.findByKeyOrAlias(m_distributor.getDbKey());
                if (d.hasValidDbKey())
                {
                    m_distributor.setOrder(d.getOrder());
                    found = true;
                }
                else
                {
                    m_distributor.setOrder(noDistributorOrder);
                }
            }
            else
            {
                m_distributor.setOrder(noDistributorOrder);
            }
            return found;
        }

        bool CAircraftModel::hasDbDistributor() const
        {
            return m_distributor.isLoadedFromDb();
        }

        bool CAircraftModel::hasDistributor() const
        {
            return m_distributor.hasValidDbKey(); // key is valid, but not guaranteed from DB
        }

        bool CAircraftModel::matchesDbDistributor(const CDistributor &distributor) const
        {
            if (!distributor.isLoadedFromDb()) { return false; }
            if (!this->hasDbDistributor()) { return false; }
            return m_distributor.getDbKey() == distributor.getDbKey();
        }

        bool CAircraftModel::matchesAnyDbDistributor(const CDistributorList &distributors) const
        {
            if (distributors.isEmpty()) { return false; }
            if (!this->hasDbDistributor()) { return false; }
            return distributors.matchesAnyKeyOrAlias(m_distributor.getDbKey());
        }

        bool CAircraftModel::matchesMode(ModelModeFilter mode) const
        {
            if (mode == All) { return true; }
            return (mode & m_modelMode) > 0;
        }

        const CIcon &CAircraftModel::getModelModeAsIcon() const
        {
            switch (this->getModelMode())
            {
            case Include: return CIcon::iconByIndex(CIcons::ModelInclude);
            case Exclude: return CIcon::iconByIndex(CIcons::ModelExclude);
            case Undefined: return CIcon::iconByIndex(CIcons::StandardIconUnknown16);
            default:
                Q_ASSERT_X(false, Q_FUNC_INFO, "wrong mode");
                break;
            }
            return CIcon::iconByIndex(CIcons::ModelInclude);
        }

        void CAircraftModel::setModelModeAsString(const QString &mode)
        {
            this->setModelMode(CAircraftModel::modelModeFromString(mode));
        }

        bool CAircraftModel::matchesSimulator(const CSimulatorInfo &simulator) const
        {
            return (static_cast<int>(simulator.getSimulator()) & static_cast<int>(this->getSimulator().getSimulator())) > 0;
        }

        bool CAircraftModel::matchesSimulatorFlag(CSimulatorInfo::Simulator simulator) const
        {
            return (static_cast<int>(simulator) & static_cast<int>(this->getSimulator().getSimulator())) > 0;
        }

        QDateTime CAircraftModel::getFileTimestamp() const
        {
            return this->hasValidFileTimestamp() ? QDateTime::fromMSecsSinceEpoch(m_fileTimestamp, Qt::UTC) : QDateTime();
        }

        QString CAircraftModel::getFormattedFileTimestampYmdhms() const
        {
            return this->hasValidFileTimestamp() ?
                   this->getFileTimestamp().toString("yyyy-MM-dd HH:mm:ss") :
                   "";
        }

        bool CAircraftModel::hasValidFileTimestamp() const
        {
            return m_fileTimestamp >= 0;
        }

        void CAircraftModel::setFileTimestamp(const QDateTime &timestampUtc)
        {
            m_fileTimestamp = timestampUtc.isValid() ? timestampUtc.toMSecsSinceEpoch() : -1;
        }

        void CAircraftModel::setFileTimestamp(qint64 timestamp)
        {
            m_fileTimestamp = (timestamp < 0) ? -1 : timestamp;
        }

        void CAircraftModel::setFileDetailsAndTimestamp(const QFileInfo &fileInfo)
        {
            this->setFileName(fileInfo.absoluteFilePath());
            const QDateTime modified = fileInfo.lastModified();
            if (modified.isValid())
            {
                this->setFileTimestamp(modified);
                this->setUtcTimestamp(modified);
            }
            else
            {
                const QDateTime created = fileInfo.lastModified();
                this->setFileTimestamp(created);
                this->setUtcTimestamp(created);
            }
        }

        CPixmap CAircraftModel::loadIcon(CStatusMessage &success) const
        {
            static const CStatusMessage noIcon(this, CStatusMessage::SeverityInfo, "no icon");
            if (m_iconPath.isEmpty()) { success = noIcon; return CPixmap(); }

            // load from file
            const CPixmap pm(CPixmap::loadFromFile(m_iconPath, success));
            return pm;
        }

        const QString &CAircraftModel::liveryStringPrefix()
        {
            static const QString p("swift_");
            return p;
        }

        bool CAircraftModel::isSwiftLiyeryString(const QString &liveryString)
        {
            return (liveryString.length() > liveryStringPrefix().length() && liveryString.startsWith(liveryStringPrefix(), Qt::CaseInsensitive));
        }

        QString CAircraftModel::getSwiftLiveryString() const
        {
            const QString l =
                (this->getLivery().hasValidDbKey() ? QStringLiteral("l") % this->getLivery().getDbKeyAsString() : QStringLiteral("")) %
                (this->getAircraftIcaoCode().hasValidDbKey() ? QStringLiteral("a") % this->getAircraftIcaoCode().getDbKeyAsString() : QStringLiteral("")) %
                (this->hasValidDbKey() ? QStringLiteral("m") % this->getDbKeyAsString() : QStringLiteral(""));

            return l.isEmpty() ? QStringLiteral("") : liveryStringPrefix() % l;
        }

        DBTripleIds CAircraftModel::parseNetworkLiveryString(const QString &liveryString)
        {
            // "swift_m22l33a11"
            if (!CAircraftModel::isSwiftLiyeryString(liveryString)) {return DBTripleIds(); }

            DBTripleIds ids;
            const QString ls = liveryString.mid(liveryStringPrefix().length()).toLower();
            for (int c = 0; c < ls.length(); c++)
            {
                const QChar m = ls[c];
                if ((m == 'm' || m == 'a' || m == 'l') && (c + 1) < ls.length())
                {
                    const int cs = c + 1;
                    int cc = cs;
                    while (cc < ls.length() && ls[cc].isDigit()) { cc++; } // find end of id
                    if (cc > cs)
                    {
                        const QString idString = ls.mid(cs, cc - cs);
                        const int id = idString.toInt();
                        c = cc - 1; // +1 again in for

                        if (m == 'm')      { ids.model    = id; }
                        else if (m == 'a') { ids.aircraft = id; }
                        else if (m == 'l') { ids.livery   = id; }
                    }
                }
            }
            return ids;
        }

        void CAircraftModel::updateMissingParts(const CAircraftModel &otherModel, bool dbModelPriority)
        {
            if (dbModelPriority && !this->hasValidDbKey() && otherModel.hasValidDbKey())
            {
                // we have no DB data, but the other one has
                // so we change roles. We take the DB object as base, and update our parts
                CAircraftModel copy(otherModel);
                copy.updateMissingParts(*this);
                *this = copy;
                return;
            }

            this->updateLocalFileNames(otherModel);
            if (this->hasValidDbKey() && otherModel.hasValidDbKey()) { return; } // both are DB data, treat as being the same except for filename maybe

            if (m_callsign.isEmpty())       { this->setCallsign(otherModel.getCallsign()); }
            if (m_modelString.isEmpty())    { this->setModelString(otherModel.getModelString()); }
            if (m_name.isEmpty())           { this->setName(otherModel.getName()); }
            if (m_modelType == TypeUnknown) { m_modelType = otherModel.getModelType(); }
            if (m_modelMode == Undefined)   { m_modelType = otherModel.getModelType(); }
            if (m_fileTimestamp < 0)        { this->setFileTimestamp(otherModel.getFileTimestamp()); }
            if (m_description.isEmpty() || m_description.startsWith(CAircraftModel::autoGenerated(), Qt::CaseInsensitive)) { this->setDescription(otherModel.getDescription()); }
            if (this->getSimulator().isUnspecified())
            {
                // simulator can only be overridden as simulators can also be removed
                this->setSimulator(otherModel.getSimulator());
            }

            ITimestampBased::updateMissingParts(otherModel);
            m_livery.updateMissingParts(otherModel.getLivery());
            m_aircraftIcao.updateMissingParts(otherModel.getAircraftIcaoCode());
            m_distributor.updateMissingParts(otherModel.getDistributor());
        }

        bool CAircraftModel::hasQueriedModelString() const
        {
            return m_modelType == TypeQueriedFromNetwork && this->hasModelString();
        }

        bool CAircraftModel::hasManuallySetString() const
        {
            return m_modelType == TypeManuallySet && this->hasModelString();
        }

        bool CAircraftModel::hasDescription(bool ignoreAutoGenerated) const
        {
            if (m_description.isEmpty()) { return false; }
            if (!ignoreAutoGenerated) { return true; }
            return (!this->getDescription().startsWith(autoGenerated(), Qt::CaseInsensitive));
        }

        bool CAircraftModel::hasValidSimulator() const
        {
            return m_simulator.isAnySimulator();
        }

        QString CAircraftModel::getMembersDbStatus() const
        {
            return
                (this->isLoadedFromDb() ? QStringLiteral("M") : QStringLiteral("m")) %
                (this->getDistributor().isLoadedFromDb() ? QStringLiteral("D") : QStringLiteral("d")) %
                (this->getAircraftIcaoCode().isLoadedFromDb() ? QStringLiteral("A") : QStringLiteral("a")) %
                (this->getLivery().isLoadedFromDb() && getLivery().isColorLivery() ?
                 QStringLiteral("C-") :
                 (this->getLivery().isLoadedFromDb() ? QStringLiteral("L") : QStringLiteral("l")) %
                 (this->getLivery().getAirlineIcaoCode().isLoadedFromDb() ? QStringLiteral("A") : QStringLiteral("a")));
        }

        void CAircraftModel::normalizeFileNameForDb()
        {
            m_fileName = CAircraftModel::normalizeFileNameForDb(m_fileName);
        }

        void CAircraftModel::updateLocalFileNames(const CAircraftModel &model)
        {
            if (this->getModelType() == CAircraftModel::TypeOwnSimulatorModel)
            {
                // this is already a local model, ignore
                return;
            }

            if (model.getModelType() == CAircraftModel::TypeOwnSimulatorModel)
            {
                // other local, priority
                this->setFileName(model.getFileName());
                this->setIconPath(model.getIconPath());
                return;
            }

            // both not local, override empty values
            if (m_fileName.isEmpty()) { this->setFileName(model.getFileName()); }
            if (m_iconPath.isEmpty()) { this->setIconPath(model.getIconPath()); }
        }

        bool CAircraftModel::adjustLocalFileNames(const QString &newModelDir, const QString &stripModelDirIndicator)
        {
            if (!this->hasFileName()) { return false; }
            const QString md = CFileUtils::normalizeFilePathToQtStandard(newModelDir);
            int i = -1;
            if (stripModelDirIndicator.isEmpty())
            {
                QString strip = md.mid(md.lastIndexOf('/'));
                i = m_fileName.lastIndexOf(strip);
            }
            else
            {
                i = m_fileName.lastIndexOf(stripModelDirIndicator);
            }
            if (i < 0) { return false; }
            m_fileName = CFileUtils::appendFilePaths(newModelDir, m_fileName.mid(i));
            return true;
        }

        bool CAircraftModel::hasExistingCorrespondingFile() const
        {
            if (!this->hasFileName()) { return false; }
            const QFileInfo fi(this->getFileName());
            return (fi.exists() && fi.isReadable());
        }

        bool CAircraftModel::matchesModelString(const QString &modelString, Qt::CaseSensitivity sensitivity) const
        {
            return m_modelString.length() == modelString.length() &&
                   m_modelString.startsWith(modelString, sensitivity);
        }

        int CAircraftModel::calculateScore(const CAircraftModel &compareModel, bool preferColorLiveries, CStatusMessageList *log) const
        {
            const int icaoScore = this->getAircraftIcaoCode().calculateScore(compareModel.getAircraftIcaoCode(), log);
            const int liveryScore = this->getLivery().calculateScore(compareModel.getLivery(), preferColorLiveries, log);
            CMatchingUtils::addLogDetailsToList(log, this->getCallsign(), QString("ICAO score: %1 | livery score: %2").arg(icaoScore).arg(liveryScore));
            return qRound(0.5 * (icaoScore + liveryScore));
        }

        CStatusMessageList CAircraftModel::validate(bool withNestedObjects) const
        {
            static const CLogCategoryList cats(CLogCategoryList(this).join({ CLogCategory::validation() }));
            CStatusMessageList msgs;
            if (!hasModelString()) { msgs.push_back(CStatusMessage(cats, CStatusMessage::SeverityError, "Model: missing model string (aka key)")); }
            if (!hasValidSimulator()) {msgs.push_back(CStatusMessage(cats, CStatusMessage::SeverityError, "Model: no simulator set")); }
            // as of T34 made description optional, lines can be removed after 6/2017
            // if (!hasDescription()) {msgs.push_back(CStatusMessage(cats, CStatusMessage::SeverityWarning, "Model: no description")); }
            if (withNestedObjects)
            {
                msgs.push_back(m_aircraftIcao.validate());
                msgs.push_back(m_livery.validate());
                msgs.push_back(m_distributor.validate());
            }
            return msgs;
        }

        //! Create an appropriate status message for an attribute comparison
        CStatusMessage equalMessage(bool same, const CAircraftModel &model, const QString &description, const QString &oldValue, const QString &newValue)
        {
            if (same)
            {
                static const CStatusMessage msgSame({ CLogCategory::validation() }, CStatusMessage::SeverityWarning, "Model '%1' same %2 '%3'");
                return CStatusMessage(msgSame) << model.getModelStringAndDbKey() << description << newValue;
            }
            else
            {
                static const CStatusMessage msgDiff({ CLogCategory::validation() }, CStatusMessage::SeverityInfo, "Model '%1' changed %2 '%3'->'%4'");
                return CStatusMessage(msgDiff) << model.getModelStringAndDbKey() << description << oldValue << newValue;
            }
        }

        bool CAircraftModel::isEqualForPublishing(const CAircraftModel &dbModel, CStatusMessageList *details) const
        {
            if (!dbModel.isLoadedFromDb())
            {
                static const CStatusMessage msgNoDbModel({ CLogCategory::validation() }, CStatusMessage::SeverityInfo, "No DB model yet");
                if (details) { details->push_back(msgNoDbModel); }
                return false;
            }

            CStatusMessageList validationMsgs;
            bool changed = false;
            bool equal = dbModel.getLivery().isLoadedFromDb() && dbModel.getLivery().isDbEqual(this->getLivery());
            if (details) { validationMsgs.push_back(equalMessage(equal, *this, QStringLiteral("livery"), dbModel.getLivery().getCombinedCodePlusInfoAndId(), this->getLivery().getCombinedCodePlusInfoAndId())); }
            changed |= !equal;

            equal = dbModel.getAircraftIcaoCode().isLoadedFromDb() && dbModel.getAircraftIcaoCode().isDbEqual(this->getAircraftIcaoCode());
            if (details) { validationMsgs.push_back(equalMessage(equal, *this, QStringLiteral("aircraft ICAO"), dbModel.getAircraftIcaoCode().getDesignatorDbKey(), this->getAircraftIcaoCode().getDesignatorDbKey())); }
            changed |= !equal;

            equal = dbModel.getDistributor().isLoadedFromDb() && dbModel.getDistributor().isDbEqual(this->getDistributor());
            if (details) { validationMsgs.push_back(equalMessage(equal, *this, QStringLiteral("distributor"), dbModel.getDistributor().getDescription(), this->getDistributor().getDescription())); }
            changed |= !equal;

            equal = dbModel.getSimulator() == this->getSimulator();
            if (details) { validationMsgs.push_back(equalMessage(equal, *this, QStringLiteral("simulator"), dbModel.getSimulator().toQString(), this->getSimulator().toQString())); }
            changed |= !equal;

            equal = dbModel.getDescription() == this->getDescription();
            if (details) { validationMsgs.push_back(equalMessage(equal, *this, QStringLiteral("description"), dbModel.getDescription(), this->getDescription())); }
            changed |= !equal;

            equal = dbModel.getName() == this->getName();
            if (details) { validationMsgs.push_back(equalMessage(equal, *this, QStringLiteral("name"), dbModel.getName(), this->getName())); }
            changed |= !equal;

            equal = dbModel.getModelMode() == this->getModelMode();
            if (details) { validationMsgs.push_back(equalMessage(equal, *this, QStringLiteral("mode"), dbModel.getModelModeAsString(), this->getModelModeAsString())); }
            changed |= !equal;

            // clean messages
            if (changed && details)
            {
                // we have a changed entity, remove the warnings as they are just noise
                validationMsgs.removeSeverity(CStatusMessage::SeverityWarning);
            }

            if (details) { details->push_back(validationMsgs); }
            return !changed;
        }

        const QString &CAircraftModel::modelTypeToString(CAircraftModel::ModelType type)
        {
            static const QString queried("queried");
            static const QString matching("matching");
            static const QString db("database");
            static const QString def("map.default");
            static const QString ownSim("own simulator");
            static const QString set("manually set");
            static const QString fsinn("FSInn");
            static const QString probe("probe");
            static const QString unknown("unknown");

            switch (type)
            {
            case TypeQueriedFromNetwork: return queried;
            case TypeModelMatching: return matching;
            case TypeDatabaseEntry: return db;
            case TypeModelMatchingDefaultModel: return def;
            case TypeOwnSimulatorModel: return ownSim;
            case TypeManuallySet: return set;
            case TypeFSInnData: return fsinn;
            case TypeTerrainProbe: return probe;
            case TypeUnknown:
            default: return unknown;
            }
        }

        QString CAircraftModel::normalizeFileNameForDb(const QString &filePath)
        {
            const QString n = CFileUtils::normalizeFilePathToQtStandard(filePath).toUpper();
            if (n.count('/') < 2) { return n; }
            return n.section('/', -2, -1);
        }

        CAircraftModel::ModelMode CAircraftModel::modelModeFromString(const QString &mode)
        {
            if (mode.isEmpty() || mode.startsWith('I', Qt::CaseInsensitive)) { return Include;}
            if (mode.startsWith('E', Qt::CaseInsensitive)) { return Exclude; }
            BLACK_VERIFY_X(false, Q_FUNC_INFO, "wrong mode");
            return Include; // default
        }

        const QString &CAircraftModel::modelModeToString(CAircraftModel::ModelMode mode)
        {
            static const QString i("Include");
            static const QString e("Exclude");

            switch (mode)
            {
            case Include: return i;
            case Exclude: return e;
            default: Q_ASSERT_X(false, Q_FUNC_INFO, "wrong mode");
            }
            return i; // default
        }

        CAircraftModel CAircraftModel::fromDatabaseJson(const QJsonObject &json, const QString &prefix)
        {
            static const QString prefixAircraftIcao("ac_");
            static const QString prefixLivery("liv_");
            static const QString prefixDistributor("dist_");

            const QString modelString(json.value(prefix % QStringLiteral("modelstring")).toString());
            const QString modelDescription(json.value(prefix % QStringLiteral("description")).toString());
            const QString modelName(json.value(prefix % QStringLiteral("name")).toString());
            const QString modelMode(json.value(prefix % QStringLiteral("mode")).toString());

            const CSimulatorInfo simInfo = CSimulatorInfo::fromDatabaseJson(json, prefix);
            CDistributor distributor(CDistributor::fromDatabaseJson(json, prefixDistributor));
            CAircraftIcaoCode aircraftIcao(CAircraftIcaoCode::fromDatabaseJson(json, prefixAircraftIcao));
            CLivery livery(CLivery::fromDatabaseJson(json, prefixLivery));

            //! \todo KB 2018-08 "idaircrafticao" seem not to be used anymore (remove???) in "vAircraftModelDenormalized", but was used in "vAircraftModelComplete"
            if (!aircraftIcao.isLoadedFromDb())
            {
                const int idAircraftIcao = json.value(prefix % QStringLiteral("idaircrafticao")).toInt(-1);
                if (idAircraftIcao >= 0)
                {
                    aircraftIcao.setDbKey(idAircraftIcao);
                }
            }

            if (!livery.isLoadedFromDb())
            {
                const int idLivery = json.value(prefix % QStringLiteral("idlivery")).toInt(-1);
                if (idLivery >= 0)
                {
                    livery.setDbKey(idLivery);
                }
            }

            if (!distributor.isLoadedFromDb())
            {
                const QString idDistributor = json.value(prefix % QStringLiteral("iddistributor")).toString();
                if (!idDistributor.isEmpty())
                {
                    distributor.setDbKey(idDistributor);
                }
            }

            CAircraftModel model(
                modelString, CAircraftModel::TypeDatabaseEntry, simInfo, modelName, modelDescription, aircraftIcao, livery
            );
            model.setDistributor(distributor);
            model.setModelModeAsString(modelMode);
            model.setKeyAndTimestampFromDatabaseJson(json, prefix);
            return model;
        }

        CAircraftModel CAircraftModel::fromDatabaseJsonCaching(
            const QJsonObject &json,
            AircraftIcaoIdMap &aircraftIcaos, LiveryIdMap &liveries, DistributorIdMap &distributors,
            const QString &prefix)
        {
            static const QString prefixAircraftIcao("ac_");
            static const QString prefixLivery("liv_");
            static const QString prefixDistributor("dist_");

            const QString modelString(json.value(prefix % QStringLiteral("modelstring")).toString());
            const QString modelDescription(json.value(prefix % QStringLiteral("description")).toString());
            const QString modelName(json.value(prefix % QStringLiteral("name")).toString());
            const QString modelMode(json.value(prefix % QStringLiteral("mode")).toString());

            const CSimulatorInfo simInfo = CSimulatorInfo::fromDatabaseJson(json, prefix);
            const int idAircraftIcao = json.value(prefixAircraftIcao % QStringLiteral("id")).toInt(-1);
            const int idLivery = json.value(prefixLivery % QStringLiteral("id")).toInt(-1);
            const QString idDistributor = json.value(prefixDistributor % QStringLiteral("id")).toString();
            const bool cachedAircraftIcao = (idAircraftIcao >= 0) && aircraftIcaos.contains(idAircraftIcao);
            const bool cachedLivery = (idLivery >= 0) && liveries.contains(idLivery);
            const bool cachedDistributor = !idDistributor.isEmpty() && distributors.contains(idDistributor);

            CAircraftIcaoCode aircraftIcao(cachedAircraftIcao ?
                                           aircraftIcaos[idAircraftIcao] :
                                           CAircraftIcaoCode::fromDatabaseJson(json, prefixAircraftIcao));

            CLivery livery(cachedLivery ?
                           liveries[idLivery] :
                           CLivery::fromDatabaseJson(json, prefixLivery));

            CDistributor distributor(cachedDistributor ?
                                     distributors[idDistributor] :
                                     CDistributor::fromDatabaseJson(json, prefixDistributor));

            if (!aircraftIcao.isLoadedFromDb())
            {
                if (idAircraftIcao >= 0)
                {
                    aircraftIcao.setDbKey(idAircraftIcao);
                }
            }

            if (!livery.isLoadedFromDb())
            {
                const int idLivery = json.value(prefix % QStringLiteral("idlivery")).toInt(-1);
                if (idLivery >= 0)
                {
                    livery.setDbKey(idLivery);
                }
            }

            if (!distributor.isLoadedFromDb())
            {
                const QString idDistributor = json.value(prefix % QStringLiteral("iddistributor")).toString();
                if (!idDistributor.isEmpty())
                {
                    distributor.setDbKey(idDistributor);
                }
            }

            if (!cachedAircraftIcao && aircraftIcao.isLoadedFromDb()) { aircraftIcaos[aircraftIcao.getDbKey()] = aircraftIcao; }
            if (!cachedLivery && livery.isLoadedFromDb()) { liveries[livery.getDbKey()] = livery; }
            if (!cachedDistributor && distributor.isLoadedFromDb()) { distributors[distributor.getDbKey()] = distributor; }

            CAircraftModel model(
                modelString, CAircraftModel::TypeDatabaseEntry, simInfo, modelName, modelDescription, aircraftIcao, livery
            );
            model.setDistributor(distributor);
            model.setModelModeAsString(modelMode);
            model.setKeyAndTimestampFromDatabaseJson(json, prefix);
            return model;
        }

        const QString &CAircraftModel::autoGenerated()
        {
            static const QString ag("swift auto generated");
            return ag;
        }
    } // namespace
} // namespace

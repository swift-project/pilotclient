// SPDX-FileCopyrightText: Copyright (C) swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

// This is a demo file demonstrating some capabilities of the
// matching script engine
// This script runs after(!) the matching has been completed
//
// inObject      values as from network
// outObject     to be returned back to matching engine
// matchedObject the result so far, same as inObject for reverse lookup
// modelSet      user's current model set, not available in reverse lookup
// webServices   wraps access to swift web services
//
// outObject.rerun    = true => rerun matching or reverse lookup after the value has been changed again, like a 2nd pass
// outObject.modified = true => tell  matching or reverse lookup that something has been changed
//
(function () {
	// avoid breaking matching/reverse lookup
	try {
		// the logic we implement
		var lm = "";
		if (inObject.hasAirlineIcao) {
			if (matchedObject.hasUnmodifiedDesignators) {
				lm = "Model with exact aircraft " + inObject.aircraftIcao + " airline " + inObject.virtualAirlineIcao;
			} else if (!matchedObject.hasModifiedAircraftFamily && !matchedObject.hasModifiedAirlineIcaoDesignator) {
				lm = "Model matching with same family " + matchedObject.aircraftFamily + " and airline " + matchedObject.airlineIcao;
			} else if (!matchedObject.hasModifiedAirlineIcaoDesignator) {
				lm = "Model matching with same airline: " + matchedObject.aircraftIcao + "/" + matchedObject.airlineIcao;
			} else {
				var combinedType = inObject.combinedType;
				var family = inObject.aircraftFamily;
				var searchColor = "ffffff";
				var mscl = modelSet.findAircraftFamilyWithClosestColorLivery(family, searchColor);
				if (mscl && 0 > mscl.length) {
					lm = "Color livery model found for " + combinedType + ": " + mscl;
				} else {
					mscl = modelSet.findClosestCombinedTypeWithClosestColorLivery(combinedType, searchColor);
					if (!mscl || 0 === mscl.length) {
						// not found
						mscl = "PIPER CHEROKEE 180"; // some hardcoded value, change to your preferred default
						lm = "No color livery model found for " + combinedType + ". Using hardcoded: " + mscl;
					} else {
						lm = "Color livery model found for " + combinedType + ": " + mscl;
					}
				}
				outObject.modelString = mscl;
				outObject.modified = true; // tell we have changed the values
			}
		} else {
			// no airline, so most likely some small or military aircraft
			lm = "Model without airline";
		}

		outObject.logMessage = lm; // display log

	} catch (err) {
		return err.toString();
	}

	return outObject;
})

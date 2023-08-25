// SPDX-FileCopyrightText: Copyright (C) swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

// This is a demo file demonstrating some capabilities of the
// matching script engine
// This script runs after(!) the reverse lookup has been completed
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
		// EASYJET?
		if (inObject.airlineIcao === "EJU") {
			outObject.airlineIcao = "EZY";
			outObject.dbAirlineIcaoId = 2217;
			outObject.logMessage = "Changing EJU to EZY"; // display log
			outObject.modelString = ""; // invalidate any model
			outObject.dbModelId = -1; // invalidate
			outObject.modified = true;
			outObject.rerun = true; // treat the changed values as coming from network
		} else {
			outObject.logMessage = "No airline change, keeping " + inObject.airlineIcao;
		}
	} catch (err) {
		return err.toString();
	}

	return outObject;
})

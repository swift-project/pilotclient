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
(function() {
  // inObject
  // outObject
  // modelSet, not available in reverse lookup
  outObject.aircraftIcao = "C172";
  outObject.modified     = true; // tell we changed something
  outObject.rerun        = true; // treat the changed values as coming from network
  outObject.logMessage   = "Changed to C172"; // just logging
  outObject.modelString  = ""; // invalidate any model
  outObject.dbModelId    = -1; // invalidate
	
  return outObject;
  
  // string only return is possible
  // return "Changed ICAO to C172";
})

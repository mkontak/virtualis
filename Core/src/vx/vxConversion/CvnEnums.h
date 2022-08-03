// Copyright© 2011, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form, 
// in whole or in part, without the prior written permission of Viatronix Inc. 
// 
// Owner: Mark Kontak  mkontak@viatronix.net

#pragma once

namespace cvn
{

/**
 * Conversion Events
 */
typedef enum CvnEvents
{
  E_NONE = 0,       // No Events
  E_STARTED,        // Started
  E_COMPLETED,      // Completed
  E_UPDATED,        // Updated 
  E_FAILED,         // Failed
  E_ABORTED         // Aborted
} CvnEvents;  // CvnEvents

/**
 * Conversion Stages
 */
typedef enum CvnStages
{
  S_NONE = 0,     // No-Op
  S_RETRIEVAL,    // Retreival ( PACS )
  S_LOADING,      // Lodaing files (Files List)
  S_CREATION      // Actual conversion of the Volume
} CvnStages;  


/**
	*  v3D application type enum
	*  Including VC=Colon, CAR=Cardiac, XP=Explorer, CS=Calcium Scoring, VAS=Vascular, and XA=X-ray Angiograph
	*/
typedef enum ApplicationTypes
{ 
  VC,                 // Virtual Colonoscopy
  CAR,                // Cardiac
  CS,                 // Calcium Scoring
  XP,                 // Explorer (General)
  VAS,                // Vascular
	PTX,                // Pneumothorax
  XA                  // 
} ApplicationTypes;

};
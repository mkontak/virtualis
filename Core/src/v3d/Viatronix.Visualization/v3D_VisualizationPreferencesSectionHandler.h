// $Id: v3D_VisualizationPreferencesSectionHandler.h,v 1.4 2006/10/04 18:29:06 gdavidson Exp $
//
// Copyright © 2006, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form, 
// in whole or in part, without the prior written permission of Viatronix Inc. 
//
// Owner: Greg Davidson(mailto:gdavidson@viatronix.com)


// pragmas
#pragma once


// namespaces
OPEN_VISUALIZATION_NS


// class definition
public ref class VisualizationPreferencesSectionHandler : public System::Configuration::IConfigurationSectionHandler
{
// member functions
public:

  virtual System::Object ^ Create( System::Object ^ mpParent, System::Object ^ mpConfigContext, System::Xml::XmlNode ^ mpSection );

}; // VisualizationPreferencesSectionHandler


CLOSE_VISUALIZATION_NS


// $Log: v3D_VisualizationPreferencesSectionHandler.h,v $
// Revision 1.4  2006/10/04 18:29:06  gdavidson
// Ported to C++/CLI
//
// Revision 1.3  2006/09/20 15:05:26  frank
// added preference for displaying rendering manager statistics
//
// Revision 1.2  2006/08/03 14:21:18  geconomos
// class was totally foobar; working now
//
// $Header: /CVS/cvsRepository/V3D/src/Viatronix.Visualization/v3D_VisualizationPreferencesSectionHandler.h,v 1.4 2006/10/04 18:29:06 gdavidson Exp $
// $Id: v3D_VisualizationPreferencesSectionHandler.h,v 1.4 2006/10/04 18:29:06 gdavidson Exp $

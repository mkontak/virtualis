// $Id: MovieFormat.h,v 1.2 2007/03/12 19:43:46 jmeade Exp $
//
// Copyright© 2000, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form, 
// in whole or in part, without the prior written permission of Viatronix Inc. 
// 
// Owner: Jeff Meade  jmeade@viatronix.net

// pragmas
#pragma once

// namesapces
OPEN_MOVIES_NS

/// class declaration
class MovieFormat
{
// member functions
public:
  
  /// Default constructor
  MovieFormat();

  /// desturctor
  virtual ~MovieFormat();
  
  /// Output stream operator
  friend std::ostream & operator<<(std::ostream& os, const MovieFormat& fmt)
  { os << fmt.m_sFileTypeExt << " " << fmt.m_bIncludeMarkings << " " << fmt.m_bIncludeOverview; return os; }

  /// Input stream operator
  friend std::istream & operator>>(std::istream& is, MovieFormat& fmt)
  { is>>fmt.m_sFileTypeExt; is.ignore(1); is>>fmt.m_bIncludeMarkings; is.ignore(1); is>>fmt.m_bIncludeOverview; return is; }

// member variables
public:

/****  Persistent (over sessions) info ****/

  /// Identifies the type of the file (avi, wmv, mpg, etc.)
  std::string m_sFileTypeExt;
  
  /// Whether to include markings, such as measurements, annotations, etc. (mainly for endoscopic view quality)
  bool m_bIncludeMarkings;
  
  /// Whether to include overview image in movie
  bool m_bIncludeOverview;

/**** Non-persistent (session to session) info ****/
  
  /// Title of movie file
  std::string m_sFileTitle;

}; // class MovieFormat

CLOSE_MOVIES_NS

// $Log: MovieFormat.h,v $
// Revision 1.2  2007/03/12 19:43:46  jmeade
// code standards.
//
// Revision 1.1  2005/10/28 13:27:43  geconomos
// refactor and cleanup of movie related classes
//
// $Header: /CVS/cvsRepository/V3D/src/Viatronix.v3D.Viewer.Colon/MovieFormat.h,v 1.2 2007/03/12 19:43:46 jmeade Exp $
// $Id: MovieFormat.h,v 1.2 2007/03/12 19:43:46 jmeade Exp $

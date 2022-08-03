  // Copyright© 2011, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form, 
// in whole or in part, without the prior written permission of Viatronix Inc. 
// 
// File: Pdf.cpp
//
// Owner: Mark Kontak  mkontak@viatronix.net

#pragma once


#include "PdfGdiplusEngine.h"

#pragma warning(push)
#pragma warning(disable:4251)



namespace pdf
{

class PdfBuffer;

#ifdef UNICODE
#define DrawText DrawTextW
#else
#define DrawText DrawTextA
#endif

/**
 * PDF class  encapsulating the pdf library
 */
class Pdf
{
// enums
public:

  /**
   * An enum to specify text alignment.
   */
  typedef enum Alignment
  {
    ALIGN_LEFT                = 0,
    ALIGN_CENTER              = 1,
    ALIGN_RIGHT               = 2,
    ALIGN_JUSTIFIED           = 3,
    ALIGN_FORCE_JUSTIFIED     = 4,
    ALIGN_LAST_LINE_JUSTIFIED = 5
  } Alignment;


  /**
   * An enum to specify the supported origins.
   */
  typedef enum Origin
  {
    ORIGIN_BOTTOM_LEFT  = 0,
    ORIGIN_TOP_LEFT     = 1,
    ORIGIN_TOP_RIGHT    = 2,
    ORIGIN_BOTTOM_RIGHT = 3
  } Origin;


  /**
   * An enum to specify the supported measurement units.
   */
  typedef enum MeasurementUnits
  {
    UNITS_POINTS      = 0, // Pixels
    UNITS_MILLIMETERS = 1,
    UNITS_INCHES      = 2
  } MeasurementUnits;


  /**
   * An enum to specify PDF about info.
   */
  typedef enum InformationFields
  {
    INFO_VERSION  = 0,
    INFO_AUTHOR   = 1,
    INFO_TITLE    = 2,
    INFO_SUBJECT  = 3,
    INFO_KEYWORDS = 4,
    INFO_CREATOR  = 5,
    INFO_PRODUCER = 6
  } InformationFields;


  /**
   * An enum to specify path options.
   */
  typedef enum PathOptions
  {
    PATH_OUTLINE      = 0,
    PATH_FILL         = 1,
    PATH_OUTLINE_FILL = 2
  } PathOptions;


  /**
   * Options for underlining text.
   */
  typedef enum UnderlineOptions
  {
    UNDERLINE_NONE = 0,
    UNDERLINE_SINGLE = 1,
    UNDERLINE_DOUBLE = 2,
    UNDERLINE_STRIKEOUT = 3,
    UNDERLINE_OVER = 4
  } UnderlineOptions;

// construction
protected:

  /// constructor.
  Pdf();

public:

  /// destructor.
  virtual ~Pdf();

#pragma region create methods

  /// Creates the PDF
  static std::shared_ptr<Pdf> Create(std::shared_ptr<PdfBuffer> & pPdfBuffer );

  /// Creates the PDF
  static std::shared_ptr<Pdf> Create(const std::string & sFilename );

  /// Creates the PDF
  static std::shared_ptr<Pdf> Create();

#pragma endregion

#pragma region methods used with pacs

  /// Gets the pages as bitmaps
  std::vector<std::shared_ptr<Gdiplus::Bitmap>> GetPages();

  /// Converts the PDF to a PDF buffer
  std::shared_ptr<PdfBuffer> ToPdfBuffer();

#pragma endregion

  /// Gets the page count
  unsigned int PageCount();

  /// Adds the true type font specified. Returns the font id added.
  int AddTrueTypeFont(const std::string & sFontname, bool bEmbed = false);


#pragma region text methods

  /// Sets the font.
  void SelectFont(int iFontId);

  /// Sets the text size
  void SetTextSize(unsigned int iSize);

  /// Sets the text underline mode.
  void SetTextUnderline(UnderlineOptions eOptions);

  /// Sets the text alignment
  void SetTextAlignment(Alignment eAlign);

  /// Gets the text height
  double GetTextHeight();

/// Draws the specified text at the specified x,y location
#ifdef UNICODE
   void DrawTextW(double dX, double dY, const std::wstring & sText);
#else
   void DrawTextA(double dX, double dY, const std::string & sText);
#endif

  /// Draws wrapped text
  void DrawWrappedText(double dX, double dY, double dWidth, const std::string & sText );

  /// Returns the wrapped text height
  double GetWrappedTextHeight( double dWidth,  const std::string & sText );

#pragma endregion


#pragma region image methods

  /// Fits the image to the size
  void FitImage( double dLeft, double dTop, double dWidth, double dHeight, int iHorizontalAlignment = 0, int iVerticalAlignment = 0, int iRotate = 0);

  /// Adds the image from the file specified and returns the image id
  int AddImageFromFile(const std::string & sFilename, int iOptions = 0 );

  /// Gets the image width
  int ImageWidth();

  /// Gets the image height
  int ImageHeight();

#pragma endregion

  /// Creates a new page
  void NewPage();

  /// Sets the page size
  void SetPageSize(const std::string & sPageSize);

  /// Sets the line width
  void SetLineWidth(double dWidth );

  void DrawLine(double dStartX, double dStartY, double dEndX, double dEndY );

  /// Sets the fill color
  void SetFillColor(double dRed, double dGreen, double dBlue);

  void DrawBox(double dLeft, double dTop, double dWidth, double dHeight, int iOptions = 0);

  /// Selects the page
  void SelectPage( unsigned int iPage );

  /// Gets the currently selected page
  unsigned int SelectedPage();

  /// Start the path at the specified coordinates.
  void StartPath(double dStartX, double dStartY);

  /// Adds a line to the path at the specified coordinates.
  void AddLineToPath(double dEndX, double dEndY);

  /// Moves the path to the specified coordinates.
  void MovePath(double dNewX, double dNewY);

  /// Draws the path using the specified options.
  void DrawPath(PathOptions eOptions);

  /// Closes path
  void ClosePath();

  /// Save to file specified
  void Save(const std::string & sFilename);

  /// Adds image
  int AddImage(std::shared_ptr<Gdiplus::Bitmap> pBitmap);

  /// Sets information about this Pdf.
  void SetInformation(InformationFields eField, const std::string & sValue);

  /// Sets the origin of the document.
  void SetOrigin(Origin eOrigin);

  /// Sets the measurement units.
  void SetMeasurementUnits(MeasurementUnits eUnits);

private:

  int m_iInstanceId;

  static const wchar_t m_licenseKey[]; 

  DebenuPDFLibraryDLL0916 m_qp;


  PdfGdiplusEngine m_engine;


};  // class pdf


} // namespace pdf

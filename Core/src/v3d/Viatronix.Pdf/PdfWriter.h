// pragmas
#pragma once

// includes
#include "scoped_ptr.h"


// namespaces
using namespace System;
using namespace System::IO;
using namespace System::Drawing;
using namespace System::Collections::Generic;


OPEN_PDF_NS

#ifdef DrawText
#undef DrawText
#endif

/**
 * Wrapper to wrap the underlying Pdf object.
 */
public ref class PdfWriter
{
// enums
public:

  /**
   * The available alignments.
   */
  enum class Alignment
  {
    Left,
    Center,
    Right,
    Justified,
    ForceJustified,
    LastLineJustified
  }; // enum Alignment


  /**
   * The available info fields.
   */
  enum class InformationFields
  {
    Version,
    Author,
    Title,
    Subject,
    Keywords,
    Creator,
    Producer
  }; // enum InformationFields


    /**
   * An enum to specify the supported origins.
   */
  enum class Origin
  {
    BottomLeft,
    TopLeft,
    TopRight,
    BottomRight
  }; // enum Origin


  /**
   * An enum to specify the supported measurement units.
   */
  enum class MeasurementUnits
  {
    Points, // Pixels
    Millimeters,
    Inches
  }; // enum MesurementUnits


  /**
   * An enum to specify path options.
   */
  enum class PathOptions
  {
    Outline,     // Draw the outline only.
    Fill,        // Fill in the path with the selected color.
    Outline_Fill // Draw both the outline and fill it with the selected color.
  }; // enum PathOptions


  /**
   * Options for underlining text.
   */
  enum class UnderlineOptions
  {
    None,
    Single,
    Double,
    Strikeout,
    Over
  }; // enum UnderlineOptions

// construction
public:

  /// Constructor.
  PdfWriter();

// properties
public:

  /// Gets the number of pages in the report.
  property int PageCount
  {
    int get();
  } // PageCount


  /// Gets the currently selected page.
  property unsigned int SelectedPage
  {
    unsigned int get();
  } // SelectedPage

// methods
public:

  /// Adds an image to the library.
  int AddImage(Bitmap ^ mpBitmap);

  /// Adds an image from the filesystem to the document.
  int AddImageFromFile(String ^ mpFilename, int iOptions);

  /// Adds a line to the path from the current point to the new point specified.
  void AddLineToPath(double dEndX, double dEndY);

  /// Adds a true type font to the document.
  int AddTrueTypeFont(String ^ mpFontName, bool iEmbed);

  /// Closes the path.
  void ClosePath();

  /// Draws a box on the selected page with the specified coordinates.
  void DrawBox(double dStartX, double dStartY, double dWidth, double dHeight, int iOptions);

  /// Draws a line on the selected page with the specified coordinates.
  void DrawLine(double dStartX, double dStartY, double dEndX, double dEndY);

  /// Draws the path.
  void DrawPath(PathOptions eOptions);

  /// Draws text on the current page.
  void DrawText(double dX, double dY, String ^ mpText);

  /// Draws text wrapped to the specified width.
  void DrawWrappedText(double dX, double dY, double dWidth, String ^ mpText);

  /// Draws the image at the selected coordinates with the specified transform.
  void FitImage(double dLeft, double dTop, double dWidth, double dHeight, int iHorizontalAlignment, int iVerticalAlignment, int iRotate);

  /// Gets the selected image's width.
  int GetImageWidth();

  /// Gets the selected image's height.
  int GetImageHeight();

  /// Gets the text height.
  double GetTextHeight();

  /// Gets the wrapped text height.
  double GetWrappedTextHeight(double dWidth, String ^ mpText);

  /// Moves the path to the specified coordinates.
  void MovePath(double dNewX, double dNewY);

  /// Adds a new page to the document.
  void NewPage();
  
  /// Saves the document out to the specified file.
  void Save(String ^ mpFilename);

  /// Marks the provided font as active.
  void SelectFont(int iFontId);

  /// Selects the specified page.
  void SelectPage(unsigned int iPage);

  /// Sets the fill color.
  void SetFillColor(double dRed, double dGreen, double dBlue);

  /// Sets information about this pdf.
  void SetInformation(InformationFields eField, String ^ mpValue);

  /// Sets the line width.
  void SetLineWidth(double dWidth);

  /// Sets the measurement units.
  void SetMeasurementUnits(MeasurementUnits eUnits);

  /// Sets the origin.
  void SetOrigin(Origin eOrigin);

  /// Sets the document's page size.
  void SetPageSize(String ^ mpPageSize);

  /// Sets the text alignment from the enum.
  void SetTextAlignment(Alignment align);

  /// Sets the text size.
  void SetTextSize(unsigned int iSize);

  /// Sets the text underline mode.
  void SetTextUnderline(UnderlineOptions eOptions);

  /// Starts the path.
  void StartPath(double dStartX, double dStartY);

// fields
private:

  /// The object to wrap.
  scoped_ptr<std::shared_ptr<pdf::Pdf>> m_pdf;

}; // class PdfWriter

CLOSE_PDF_NS
// $Id: ReportTags.cs,v 1.13.2.3 2009/07/17 13:17:26 kchalupa Exp $ 
//
// Copyright © 2005, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form, 
// in whole or in part, without the prior written permission of Viatronix Inc. 
//
// Author: Romy Sreedharan ( mailto:romy@viatronix.com )

#region using

using System;
using System.Xml;
using Viatronix.v3D.Core;
using System.ComponentModel;
using System.Collections;
using System.Web.UI;
using System.Web.UI.WebControls;
using System.IO;
using System.Collections.Generic;
using Viatronix.Enterprise.Entities;

#endregion

namespace Viatronix.Console.Transfer
{
  
  #region interface

  /// <summary>
  /// Interface for v3D Report Tags
  /// </summary>
  interface ITag
  {
    /// <summary>
    /// Initializes based on series
    /// </summary>
    /// <param name="series">Series</param>
    /// <param name="data">string</param>
    void Initialize ( Series series, string data );

    /// <summary>
    /// Initializes based on series collection
    /// </summary>
    /// <param name="seriesCollection">List<Series></param>
    /// <param name="data">string</param>
    void Initialize ( List<Series> seriesCollection, string data );

    /// <summary>
    /// Renders the Html
    /// </summary>
    /// <param name="writer">HtmlTextWriter</param>
    void RenderHtml( HtmlTextWriter writer );

    /// <summary>
    /// Gets the TagType string
    /// </summary>
    /// <returns></returns>
    string GetTagType();

  }//interface ITag

  #endregion

  #region PatientInfoTag class

  /// <summary>
  /// Summary description for PatientInfoTag.
  /// </summary>
  public class PatientInfoTag : ITag
  {
    #region fields
    
    /// <summary>
    /// data string
    /// </summary>
    string m_data = string.Empty;

    /// <summary>
    /// TagName
    /// </summary>
    string m_tagName = string.Empty;

    /// <summary>
    /// Patient Object
    /// </summary>
    string m_studyUid = string.Empty;

    #endregion

    #region ITag methods

    /// <summary>
    /// Renders HTML based on the node data
    /// </summary>
    /// <param name="writer">HtmlTextWriter</param>
    public void RenderHtml( HtmlTextWriter htmlWriter )
    {
      XmlDocument doc = new XmlDocument();
      doc.LoadXml ( m_data );
      System.Xml.XmlNode node = doc.DocumentElement;

      string property = node.Attributes["property"].Value;
      PropertyFinder propertyFinder = new PropertyFinder();
      propertyFinder.CollectProperty("Viatronix.Enterprise.Entities.Patient, v3dent", property);
      string result = propertyFinder.GetPropertyValue( m_studyUid ).ToString();

      htmlWriter.WriteFullBeginTag( "div" );
      htmlWriter.Write( result );
      htmlWriter.WriteEndTag( "div" );
    }//RenderHtml( HtmlTextWriter htmlWriter )


    /// <summary>
    /// Initialize the series data and node data
    /// </summary>
    /// <param name="series">Series</param>
    /// <param name="data">string</param>
    public void Initialize ( Series series, string data )
    {
      m_data = data; 
      m_studyUid = series.StudyUid;
    }//Initialize ( Series series, string data )

    /// <summary>
    /// Initialize the series data and node data
    /// </summary>
    /// <param name="seriesCollection">List<Series></param>
    /// <param name="data">string</param>
    public void Initialize ( List<Series> seriesCollection, string data )
    {
      if ( seriesCollection.Count > 0 )
        Initialize( seriesCollection[0], data );
      else
        throw new NullReferenceException();
    }// void Initialize ( List<Series> seriesCollection, string data )

    
    /// <summary>
    /// Gets the TagType
    /// </summary>
    /// <returns>string</returns>
    public string GetTagType()
    {
      return m_tagName;
    }//GetTagType()


    #endregion

  }//PatientInfoTag


  #endregion

  #region TableTag

  /// <summary>
  /// TableTag class creates Tables based on the properties
  /// </summary>
  public class TableTag : ITag
  {
    #region fields
    /// <summary>
    /// description string
    /// </summary>
    protected string m_alt = "description";

    /// <summary>
    /// Width information
    /// </summary>
    protected string m_width = "100";

    /// <summary>
    /// Height Information
    /// </summary>
    protected string m_height = "100";

    /// <summary>
    /// Border Information
    /// </summary>
    protected string m_border = "1";

    /// <summary>
    /// Horizontal space
    /// </summary>
    protected string m_hspace = "1";

    /// <summary>
    /// Vertical space
    /// </summary>
    protected string m_vspace = "1";

    /// <summary>
    /// Number of columns
    /// </summary>
    protected int m_columns = 3;

    /// <summary>
    /// Dynsrc
    /// </summary>
    protected bool m_dynsrc = false;

    /// <summary>
    /// Series object
    /// </summary>
    protected Series m_series= null;

    /// <summary>
    /// The Tag Name
    /// </summary>
    protected string m_tagName = string.Empty;

    #endregion

    #region ITag methods
    
    /// <summary>
    /// Initialize the series object
    /// </summary>
    /// <param name="series">Series</param>
    /// <param name="data">string</param>
    public void Initialize ( Series series, string data  )
    {
      m_series = series;

      XmlDocument doc = new XmlDocument();
      doc.LoadXml ( data );
      System.Xml.XmlNode node = doc.DocumentElement;

      try
      {
        m_alt = node.Attributes["alt"].Value;
      }
      catch(Exception ex)
      {
        Viatronix.Logging.Log.Debug("Ignoring alt table property" + ex.Message, "TableTag", "Initialize");
      }
      try
      {
        m_width = node.Attributes["height"].Value;
      }
      catch(Exception ex)
      {
        Viatronix.Logging.Log.Debug("Ignoring height table property" + ex.Message, "TableTag", "Initialize");
      }

      try
      {
        m_height = node.Attributes["width"].Value;
      }
      catch (Exception ex)
      {
        Viatronix.Logging.Log.Debug("Ignoring width table property" + ex.Message, "TableTag", "Initialize");
      }
      try
      {
        m_border = node.Attributes["border"].Value;
      }
      catch(Exception ex)
      {
        Viatronix.Logging.Log.Debug("Ignoring border table property" + ex.Message, "TableTag", "Initialize");
      }
      try
      {
        m_vspace = node.Attributes["vspace"].Value;
      }
      catch(Exception ex)
      {
        Viatronix.Logging.Log.Debug("Ignoring vspace table property" + ex.Message, "TableTag", "Initialize");
      }
      try
      {
        m_hspace = node.Attributes["hspace"].Value;
      }
      catch(Exception ex)
      {
        Viatronix.Logging.Log.Debug("Ignoring hspace table property" + ex.Message, "TableTag", "Initialize");
      }

      try
      {
        int columns = System.Convert.ToInt32( node.Attributes["columns"].Value );
        if ( columns > 0 )
          m_columns = columns;
      }
      catch(Exception ex)
      {
        Viatronix.Logging.Log.Debug("Ignoring coulumns table property" + ex.Message, "TableTag", "Initialize");
      }
      try
      {
        m_dynsrc = node.Attributes["dynsrc"].Value == "true" ? true : false;
      }
      catch(Exception ex)
      {
        Viatronix.Logging.Log.Debug("Ignoring hspace table property" + ex.Message, "TableTag", "Initialize");
      }
      
    }//Initialize ( Series series, string data  )


    /// <summary>
    /// Override of RenderHtml
    /// </summary>
    /// <param name="htmlWriter">HtmlTextWriter</param>
    public virtual void RenderHtml( HtmlTextWriter htmlWriter )
    {
      throw new NotImplementedException();
    }//RenderHtml( HtmlTextWriter htmlWriter )

    /// <summary>
    /// Initialize the series data and node data
    /// </summary>
    /// <param name="seriesCollection">List<Series></param>
    /// <param name="data">string</param>
    public void Initialize ( List<Series> seriesCollection, string data )
    {
      throw new NotImplementedException();
    }// void Initialize ( List<Series> seriesCollection, string data )

    /// <summary>
    /// Gets the TagType
    /// </summary>
    /// <returns>string</returns>
    virtual public string GetTagType()
    {
      return m_tagName;
    }//GetTagType()
   
    #endregion

    #region methods
    /// <summary>
    /// Build the Table property
    /// </summary>
    /// <param name="htmlWriter"></param>
    protected void BuildTableProperties( HtmlTextWriter htmlWriter )
    {
      htmlWriter.WriteAttribute ( "height", m_height );
      htmlWriter.WriteAttribute ( "width",  m_width );
      htmlWriter.WriteAttribute ( "hspace", m_hspace );
      htmlWriter.WriteAttribute ( "vspace", m_vspace );
      htmlWriter.WriteAttribute ( "border", m_border );
      htmlWriter.WriteAttribute ( "style", "cursor:hand;" );
    }//BuildTableProperties( HtmlTextWriter htmlWriter )
    #endregion

  }//class TableTag

  #endregion

  #region SnpshotTableTag

  /// <summary>
  /// Fills a Table Tag with Snapshots
  /// </summary>
  public class SnpshotTableTag : TableTag
  {
    /// <summary>
    /// Constructor
    /// </summary>
    public SnpshotTableTag()
    {
      m_tagName = "Snapshots";
    }//Constructor

    /// <summary>
    /// Overrides the RenderHtml
    /// </summary>
    /// <param name="htmlWriter">HtmlTextWriter</param>
    public override void RenderHtml( HtmlTextWriter htmlWriter )
    {
      try
      {
        htmlWriter.WriteFullBeginTag ( "Table" );
        int columncount = 1;
        bool bSnapshotFound = false;

        foreach ( string file in m_series.Files )
        {
          if ( Path.GetFileName(file).ToLower().StartsWith("image") && Path.GetExtension(file).ToLower() == ".jpg" || Path.GetExtension(file).ToLower() == ".png")
          {
            string imageSource = System.IO.Path.Combine ( "Snapshots",  System.IO.Path.GetFileName( file ));

            if ( columncount == 1 || columncount > m_columns )
            {
              htmlWriter.WriteFullBeginTag ( "TR" );
              columncount = 1;
            }
 
            htmlWriter.WriteFullBeginTag ( "TD");
            htmlWriter.WriteBeginTag  ( "image");
            htmlWriter.WriteAttribute ( "src", imageSource );
            htmlWriter.WriteAttribute ( "onclick", "OpenDataFile(src)" );
            bSnapshotFound = true;

            BuildTableProperties( htmlWriter );

            htmlWriter.Write( HtmlTextWriter.TagRightChar );
            htmlWriter.WriteEndTag("image" );
            htmlWriter.WriteEndTag ( "TD");

            if ( columncount == m_columns )
              htmlWriter.WriteEndTag ( "TR");

            columncount++;
          }
        }
        if ( columncount <= m_columns && bSnapshotFound )
          htmlWriter.WriteEndTag ( "TR");

        htmlWriter.WriteEndTag( "Table" );
      }
      catch ( Exception ex )
      {
        Viatronix.Logging.Log.Error(ex.Message, "SnapshotTableTag", "RenderHtml");
        throw ex;
      }

    }//RenderHtml

    /// <summary>
    /// Gets the TagType
    /// </summary>
    /// <returns>string</returns>
    new public string GetTagType()
    {
      return m_tagName;
    }//GetTagType()


  }//SnpshotTableTag
  #endregion
 
  #region MovieTableTag

  /// <summary>
  /// MovieTable class fills the Tables with Movie Previews
  /// </summary>
  public class MovieTableTag : TableTag
  {
    /// <summary>
    /// Constructor
    /// </summary>
    public MovieTableTag()
    {
      m_tagName = "Movies";
    }//MovieTableTag()


    /// <summary>
    /// Writes the html summary for movies out to the writer.
    /// </summary>
    /// <param name="htmlWriter">Writes out the HTML in the correct formatting.</param>
    public override void RenderHtml(HtmlTextWriter htmlWriter)
    {
      try
      {
        int columncount = 1;
        bool bMovieFound = false;

        htmlWriter.WriteFullBeginTag("Table");

        foreach (string file in m_series.Files)
        {
          if (System.IO.Path.GetDirectoryName(file).ToLower().EndsWith("movies"))
          {
            string imageSource = System.IO.Path.Combine("Movies", System.IO.Path.GetFileName(file));

            string extension = System.IO.Path.GetExtension(file).ToLower();
            if (extension != ".png" && extension != ".jpg")
            {
              continue;
            }

            string movieFileName = string.Empty;

            ////////////////////////////////////////////////////////
            /////////// Examine Directory Image Resides in /////////
            ////////////////////////////////////////////////////////

            string pattern = System.IO.Path.GetFileName(file);
            pattern = pattern.Replace(extension, string.Empty) + ".*";
            string[] files = System.IO.Directory.GetFiles(System.IO.Path.Combine(m_series.Path, "Movies"), pattern);

            // Check the directory contents for a like-named file with another extension.
            foreach (string resultfile in files)
            {
              string imageFileName = System.IO.Path.GetFileName(file);
              string resultFileName = System.IO.Path.GetFileName(resultfile);

              //assume there is another file name with a different extension.
              if (imageFileName.ToLower() != resultFileName.ToLower())
              {
                movieFileName = System.IO.Path.Combine("Movies", resultFileName);
                break;
              }
            }

            if (movieFileName.Equals(string.Empty))
            {
              continue;
            }

            if (columncount == 1 || columncount > m_columns)
            {
              htmlWriter.WriteFullBeginTag("TR");
              columncount = 1;
            }

            htmlWriter.WriteFullBeginTag("TD");
            htmlWriter.WriteBeginTag("image");
            htmlWriter.WriteAttribute("src", imageSource);
            bMovieFound = true;

            movieFileName = movieFileName.Replace(@"\", @"\\");
            string openString = "OpenDataFile('" + movieFileName + "')";

            if (m_dynsrc)
            {
              htmlWriter.WriteAttribute("dynsrc", movieFileName);
            }

            htmlWriter.WriteAttribute("onclick", openString);

            BuildTableProperties(htmlWriter);

            htmlWriter.Write(HtmlTextWriter.TagRightChar);
            htmlWriter.WriteEndTag("image");
            htmlWriter.WriteEndTag("TD");

            if (columncount == m_columns)
            {
              htmlWriter.WriteEndTag("TR");
            }

            columncount++;
          }
        }

        if (columncount <= m_columns && bMovieFound)
          htmlWriter.WriteEndTag("TR");

        htmlWriter.WriteEndTag("Table");
      }
      catch (Exception ex)
      {
        Viatronix.Logging.Log.Error(ex.Message, "MovieTableTag", "Initialize");
        throw ex;
      }
    }// RenderHtml( HtmlTextWriter htmlWriter )


    /// <summary>
    /// Gets the TagType
    /// </summary>
    /// <returns>string</returns>
    new public string GetTagType()
    {
      return m_tagName;
    }//GetTagType()

  }//class MovieTableTag

  #endregion

  #region ReportLinkTag
  
  /// <summary>
  /// Report Link Tag class creates links for Reports
  /// </summary>
  public class ReportLinkTag : ITag
  {
    #region fields
    /// <summary>
    /// Series Object
    /// </summary>
    Series m_series= null;

    /// <summary>
    /// tag Name
    /// </summary>
    protected string m_tagName = string.Empty;

    #endregion

    #region ITag methods

    /// <summary>
    /// Constructor
    /// </summary>
    public ReportLinkTag()
    {
      m_tagName = "Reports";
    }//ReportLinkTag()

    /// <summary>
    /// Renders HTML 
    /// </summary>
    /// <param name="htmlWriter">HtmlTextWriter</param>
    public void RenderHtml( HtmlTextWriter htmlWriter )
    {
      try
      {
        //check to see whether the Report File exists or not.
        bool reportAvailable = false;

        foreach ( string file in m_series.Files )
        {
          if ( file.ToLower().EndsWith(".vrx" ) || file.ToLower().EndsWith("report.xml" ) )
          {
            reportAvailable = true;
            break;
          }
        }

        if ( reportAvailable )
        {
          HyperLink reportLink = new HyperLink();
 
          string file = System.IO.Path.Combine ( "Reports", "report.pdf" );
      
          reportLink.NavigateUrl = file ;
          reportLink.Text = "Click Here to View Session Report";

          htmlWriter.RenderBeginTag(HtmlTextWriterTag.Td);
          reportLink.RenderControl(htmlWriter);
          htmlWriter.RenderEndTag();
        }
      }
      catch ( Exception ex )
      {
        Viatronix.Logging.Log.Error(ex.Message, "ReportLinkTag", "RenderHtml");
        throw ex;
      }
    }//RenderHtml( HtmlTextWriter htmlWriter )

    /// <summary>
    /// Gets the TagType
    /// </summary>
    /// <returns>string</returns>
    public string GetTagType()
    {
      return m_tagName;
    }//GetTagType()



    /// <summary>
    /// Initializes the series object
    /// </summary>
    /// <param name="series">Series</param>
    /// <param name="data">string</param>
    public void Initialize ( Series series, string data )
    {
      m_series = series;
    }//Initialize ( Series series, string data )

    /// <summary>
    /// Initialize the series data and node data
    /// </summary>
    /// <param name="seriesCollection">List<Series></param>
    /// <param name="data">string</param>
    public void Initialize ( List<Series> seriesCollection, string data )
    {
      throw new NotImplementedException();
    }// void Initialize ( List<Series> seriesCollection, string data )

    #endregion
  }//ReportLinkTag

  #endregion

  #region ReportHeaderTag
  
  /// <summary>
  /// ReportHeaderTag creates links for Reports
  /// </summary>
  public class ReportHeaderTag : ITag
  {
    #region fields
    /// <summary>
    /// Series Object
    /// </summary>
    List<Series> m_seriesCollection = null;

    #endregion

    #region ITag methods

    /// <summary>
    /// Renders HTML 
    /// </summary>
    /// <param name="htmlWriter">HtmlTextWriter</param>
    public void RenderHtml( HtmlTextWriter htmlWriter )
    {
      try
      {
        htmlWriter.WriteFullBeginTag ( "Table" );

        System.Collections.Specialized.StringCollection reportPageLinks = new System.Collections.Specialized.StringCollection();

        foreach ( Series series in m_seriesCollection )
        {
          if ( !series.Type.Equals( "Session" ))
            continue;

          htmlWriter.WriteFullBeginTag ( "TR" );
 
          HyperLink reportLink = new HyperLink();
          string file = System.IO.Path.Combine( TransferUtilities.CreatesubFolderName( series ), "Report.html" );
 
          reportLink.NavigateUrl = file ;
          reportLink.Text = series.Description;

          htmlWriter.RenderBeginTag( HtmlTextWriterTag.Td );
          reportLink.RenderControl( htmlWriter );
          htmlWriter.RenderEndTag();

          htmlWriter.WriteEndTag ( "TR");
        }
      }
      catch ( Exception ex )
      {
        Viatronix.Logging.Log.Error(ex.Message, "ReportHeaderTag", "Initialize");
        throw ex;
      }
      finally
      {
        htmlWriter.WriteEndTag ( "Table" );
      }
    }//RenderHtml( HtmlTextWriter htmlWriter )


    /// <summary>
    /// Gets the TagType
    /// </summary>
    /// <returns>string</returns>
    public string GetTagType()
    {
      return string.Empty;
    }//GetTagType()


    /// <summary>
    /// Initializes the series object
    /// </summary>
    /// <param name="series">Series</param>
    /// <param name="data">string</param>
    public void Initialize ( Series series, string data )
    {
      m_seriesCollection = new List<Series>();
      m_seriesCollection.Add ( series );
    }//Initialize ( Series series, string data )

    /// <summary>
    /// Initialize the series data and node data
    /// </summary>
    /// <param name="seriesCollection">List<Series></param>
    /// <param name="data">string</param>
    public void Initialize ( List<Series> seriesCollection, string data )
    {
      m_seriesCollection = new List<Series>();
      m_seriesCollection.AddRange( seriesCollection );
    }// void Initialize ( List<Series> seriesCollection, string data )

    #endregion
  }//ReportHeaderTag

  #endregion

  #region TitleTag

  /// <summary>
  /// Summary description for TitleTag.
  /// </summary>
  public class TitleTag : ITag
  {
    #region fields
    
    /// <summary>
    /// Patient Object
    /// </summary>
    Study m_study= null;

    #endregion

    #region ITag methods

    /// <summary>
    /// Renders HTML based on the node data
    /// </summary>
    /// <param name="writer">HtmlTextWriter</param>
    public void RenderHtml( HtmlTextWriter htmlWriter )
    {
      htmlWriter.WriteFullBeginTag( "title" );
      htmlWriter.Write( m_study.Name );
      htmlWriter.WriteEndTag( "title" );
    }//RenderHtml( HtmlTextWriter htmlWriter )

    /// <summary>
    /// Gets the TagType
    /// </summary>
    /// <returns>string</returns>
    public string GetTagType()
    {
      return string.Empty;
    }//GetTagType()



    /// <summary>
    /// Initialize the series data and node data
    /// </summary>
    /// <param name="series">Series</param>
    /// <param name="data">string</param>
    public void Initialize ( Series series, string data )
    {
      m_study = Global.DataProvider.GetStudy( series);
    }//Initialize ( Series series, string data )

    /// <summary>
    /// Initialize the series data and node data
    /// </summary>
    /// <param name="seriesCollection">List<Series></param>
    /// <param name="data">string</param>
    public void Initialize ( List<Series> seriesCollection, string data )
    {
      if ( seriesCollection.Count > 0 )
        Initialize( seriesCollection[0], data );
      else
        throw new NullReferenceException();
    }// void Initialize ( List<Series> seriesCollection, string data )


    #endregion

  }//TitleTag

  #endregion
    
  #region PropertyFinder

  /// <summary>
  /// Finds the property from a Type
  /// </summary>
  public class PropertyFinder
  {
    /// <summary>
    /// Property collection
    /// </summary>
    private System.Collections.ArrayList m_properties = new ArrayList();

    /// <summary>
    /// Collects property from a type
    /// </summary>
    /// <param name="type">string</param>
    /// <param name="property">string</param>
    public void CollectProperty( string type, string property )
    {
      try
      {
        Type objectType = Type.GetType( type );
        m_properties.Clear();
      
        foreach( string propertyName in property.Split( '.' ))
        {
          PropertyDescriptor propertyDesc = TypeDescriptor.GetProperties( objectType )[ propertyName ];
          m_properties.Add ( propertyDesc );
          objectType = propertyDesc.PropertyType;
        }
      }
      catch( Exception ex )
      {
        Viatronix.Logging.Log.Error("unable to collect property" + property + ex.Message, "PropertyFinder", "CollectProperty");
      }
    }//CollectProperty( string type, string property )
 
    
    /// <summary>
    /// Gets the Property Value
    /// </summary>
    /// <param name="target">object</param>
    /// <returns>object</returns>
    public object GetPropertyValue( object target )
    {
      try
      {
        object result = target;
        foreach( PropertyDescriptor property in m_properties )
          result = property.GetValue( result );

        return result;
      }
      catch( Exception ex )
      {
        Viatronix.Logging.Log.Debug ( "unable to collect property value" + ex.Message, "PropertyFinder", "GetPropertyValue");
        return null;
      }
    }//GetPropertyValue( object target )


  }//PropertyFinder

  #endregion

}//Viatronix.Console.Transfer



// $Log: ReportTags.cs,v $
// Revision 1.13.2.3  2009/07/17 13:17:26  kchalupa
// Patient CD burning has problems with showing the movie in HTML session report.
//
// Revision 1.13.2.2  2008/09/16 17:12:51  kchalupa
// Issue
// 6150  No Movie Preview Console Crashes
// VCSessionViewer
// 6122 VC Sessions Not Showing Completed
// VCLauncher
// 6073 Each Movie (VC) Is Listed Twice On Patient CD
// VCSessionProvider
// 6153 Movies that have no preview do not show up in Patient CD HTML Report
// RemovableMediaForPatient
// ReportTags
//
// Revision 1.13.2.2  2008/09/11 12:00:23  kchalupa
// Issue #6153
// Needed to examine both the Movie folder directory contents and the series file list for movie files. 
//
// Revision 1.13.2.1  2007/07/02 17:41:43  romy
// added JPG type for movie fils this is for VC
//
// Revision 1.13  2007/03/02 21:28:18  romy
// Issue # 5446 Patient CD transfer, deselecting Movies or Reports leave dead links on the CD.
// Added a hook to match the tags related to the selected items.
//
// Revision 1.12  2007/02/28 19:58:16  romy
// added comments before the code review
//
// Revision 1.11  2006/04/24 16:43:08  romy
// added TitleTag  class
//
// Revision 1.10  2006/02/27 20:55:21  romy
// adjusted for VC studies
//
// Revision 1.9  2006/02/16 21:46:32  romy
// report header added
//
// Revision 1.8  2006/02/16 19:45:38  romy
// VC Transfer added
//
// Revision 1.7  2006/02/16 15:10:47  romy
// using ISessionProvider
//
// Revision 1.6  2006/02/06 19:06:31  romy
// fixed empty report tag issue.
//
// Revision 1.5  2006/01/25 21:29:25  romy
// fixed Report Name
//
// Revision 1.4  2006/01/25 20:46:44  romy
// check added for empty movies
//
// Revision 1.3  2006/01/20 08:43:31  romy
// minor fixes
//
// Revision 1.2  2006/01/17 21:16:14  romy
// nitial version
//
// $Header: /CVS/cvsRepository/V3D/src/Viatronix.Console/Transfer/ReportTags.cs,v 1.13.2.3 2009/07/17 13:17:26 kchalupa Exp $
// $Id: ReportTags.cs,v 1.13.2.3 2009/07/17 13:17:26 kchalupa Exp $


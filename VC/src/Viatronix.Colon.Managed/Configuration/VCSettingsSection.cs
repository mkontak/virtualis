// Copyright © 2011, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form, 
// in whole or in part, without the prior written permission of Viatronix Inc. 
//
// Author: Greg Davidson ( mailto:gdavidson@viatronix.com )


using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Configuration;
using System.Xml;

namespace Viatronix.Colon.Configuration
{
  public class VCSettingsSection : ConfigurationSection
  {

    #region fields

    private const string SECTION = "settings";

    private static VCSettingsSection m_section = null;

    private static string m_commonDataDir = string.Empty;

    #endregion

    #region properties


    public static string CommonApplicationData
    {
      get
      {
        if( string.IsNullOrWhiteSpace( m_commonDataDir ))
        {
          string programDataDir = System.Environment.GetFolderPath( Environment.SpecialFolder.CommonApplicationData );
          m_commonDataDir = System.IO.Path.Combine( programDataDir, @"viatronix\colon\" );

          // create directory if it does not exist
          System.IO.Directory.CreateDirectory( m_commonDataDir );
        }

        return m_commonDataDir;
      }

    }
    

    // Add declarations for child elements and attributes like this:
    [ConfigurationProperty( "ver", IsRequired = true )]
    public string Version
    {
      get
      {
        return (string) this["ver"];
      }
    } // Version


    [ConfigurationProperty( "log", IsRequired = true )]
    public ConfigElement<string> Log
    {
      get
      {
        return (ConfigElement<string>) this["log"];
      }
    } // Log


    [ConfigurationProperty( "language", IsRequired = true )]
    public ConfigElement<string> Language
    {
      get
      {
        return (ConfigElement<string>) this["language"];
      }
    } // Language


    [ConfigurationProperty( "processor", IsRequired = true )]
    public ProcessorElement Processor
    {
      get
      {
        return (ProcessorElement) this["processor"];
      }
    } // Processor


    [ConfigurationProperty( "viewer", IsRequired = true )]
    public ViewerElement Viewer
    {
      get
      {
        return (ViewerElement) this["viewer"];
      }
    } // Viewer

    #endregion

    #region methods

    /// <summary>
    /// Gets the configuration section using the default element name.
    /// </summary>
    public static VCSettingsSection GetSection()
    {
      return GetSection( SECTION );
    } // GetSection()


    /// <summary>
    /// Gets the configuration section using the specified element name.
    /// </summary>
    public static VCSettingsSection GetSection( string definedName )
    {
      if( m_section == null )
      {
        m_section = ConfigurationManager.GetSection( definedName ) as VCSettingsSection;
        if( m_section == null )
          throw new ConfigurationErrorsException( "The <" + definedName + "> section is not defined in your .config file!" );
      }

      return m_section;
    } // GetSection( definedName )


    /// <summary>
    /// Returns the raw xml
    /// </summary>
    /// <returns>xml</returns>
    public static string GetRawXml()
    {
      System.Configuration.Configuration config = ConfigurationManager.OpenExeConfiguration( ConfigurationUserLevel.None );
      ConfigurationSection section = config.GetSection( SECTION );

      string xml = section.SectionInformation.GetRawXml();

      return xml;
    } // GetXml()

    #endregion

  } // class VCSettingsSection

  #region class ProcessorElement

  public class ProcessorElement : ConfigurationElement
  {

    #region properties

    [ConfigurationProperty( "ver", IsRequired = true )]
    public string Version
    {
      get
      {
        return (string) this["ver"];
      }
    } // Version


    [ConfigurationProperty( "displayQuality2D", IsRequired = true )]
    public ConfigElement<int> DisplayQuality2D
    {
      get
      {
        return (ConfigElement<int>) this["displayQuality2D"];
      }
    } // DisplayQuality2D


    [ConfigurationProperty( "electronicCleanse", IsRequired = true )]
    public ConfigElement<int> ElectronicCleanse
    {
      get
      {
        return (ConfigElement<int>) this["electronicCleanse"];
      }
    } // ElectronicCleanse


    [ConfigurationProperty( "createCAD", IsRequired = true )]
    public ConfigElement<int> CreateCAD
    {
      get
      {
        return (ConfigElement<int>) this["createCAD"];
      }
    } // CreateCAD


    [ConfigurationProperty( "compressed3DOn", IsRequired = true )]
    public ConfigElement<int> Compressed3DOn
    {
      get
      {
        return (ConfigElement<int>) this["compressed3DOn"];
      }
    } // Compressed3DOn


    [ConfigurationProperty( "applyInterpolation", IsRequired = true )]
    public ConfigElement<int> ApplyInterpolation
    {
      get
      {
        return (ConfigElement<int>) this["applyInterpolation"];
      }
    } // ApplyInterpolation


    [ConfigurationProperty( "compressed3DSliceBound", IsRequired = true )]
    public ConfigElement<int> Compressed3DSliceBound
    {
      get
      {
        return (ConfigElement<int>) this["compressed3DSliceBound"];
      }
    } // Compressed3DSliceBound


    [ConfigurationProperty( "residuePercent", IsRequired = true )]
    public ConfigElement<double> ResiduePercent
    {
      get
      {
        return (ConfigElement<double>) this["residuePercent"];
      }
    } // ResiduePercent

    #endregion

  } // class ProcessorElement

  #endregion

  #region class ViewerElement

  public class ViewerElement : ConfigurationElement
  {

    #region properties

    [ConfigurationProperty( "ver", IsRequired = true )]
    public string Version
    {
      get
      {
        return (string) this["ver"];
      }
    } // Version


    [ConfigurationProperty( "auto2DFlight", IsRequired = true )]
    public ConfigElement<int> Auto2DFlight
    {
      get
      {
        return (ConfigElement<int>) this["auto2DFlight"];
      }
    } // Auto2DFlight


    [ConfigurationProperty( "cutRenderSize", IsRequired = true )]
    public ConfigElement<int> CutRenderSize
    {
      get
      {
        return (ConfigElement<int>) this["cutRenderSize"];
      }
    } // CutRenderSize


    [ConfigurationProperty( "minRenderSize", IsRequired = true )]
    public ConfigElement<int> MinRenderSize
    {
      get
      {
        return (ConfigElement<int>) this["minRenderSize"];
      }
    } // MinRenderSize


    [ConfigurationProperty( "maxRenderSize", IsRequired = true )]
    public ConfigElement<int> MaxRenderSize
    {
      get
      {
        return (ConfigElement<int>) this["maxRenderSize"];
      }
    } // MaxRenderSize


    [ConfigurationProperty( "minFrameRate", IsRequired = true )]
    public FloatElement MinFrameRate
    {
      get
      {
        return (FloatElement) this["minFrameRate"];
      }
    } // MinFrameRate


    [ConfigurationProperty( "maxFrameRate", IsRequired = true )]
    public FloatElement MaxFrameRate
    {
      get
      {
        return (FloatElement) this["maxFrameRate"];
      }
    } // MaxFrameRate


    [ConfigurationProperty( "drawDisclaimer", IsRequired = true )]
    public ConfigElement<int> DrawDisclaimer
    {
      get
      {
        return (ConfigElement<int>) this["drawDisclaimer"];
      }
    } // DrawDisclaimer


    [ConfigurationProperty( "fieldOfView3D", IsRequired = true )]
    public ConfigElement<int> FieldOfView3D
    {
      get
      {
        return (ConfigElement<int>) this["fieldOfView3D"];
      }
    } // FieldOfView3D


    [ConfigurationProperty( "windowWidthIncreaseRight", IsRequired = true )]
    public ConfigElement<int> WindowWidthIncreaseRight
    {
      get
      {
        return (ConfigElement<int>) this["windowWidthIncreaseRight"];
      }
    } // WindowWidthIncreaseRight


    [ConfigurationProperty( "movieImageSize", IsRequired = true )]
    public ConfigElement<int> MovieImageSize
    {
      get
      {
        return (ConfigElement<int>) this["movieImageSize"];
      }
    } // MovieImageSize


    [ConfigurationProperty( "movieFrameRate", IsRequired = true )]
    public ConfigElement<int> MovieFrameRate
    {
      get
      {
        return (ConfigElement<int>) this["movieFrameRate"];
      }
    } // MovieFrameRate


    [ConfigurationProperty( "movieRenderSize", IsRequired = true )]
    public ConfigElement<int> MovieRenderSize
    {
      get
      {
        return (ConfigElement<int>) this["movieRenderSize"];
      }
    } // MovieRenderSize


    [ConfigurationProperty( "maxRenderThreads", IsRequired = true )]
    public ConfigElement<int> MaxRenderThreads
    {
      get
      {
        return (ConfigElement<int>) this["maxRenderThreads"];
      }
    } // MaxRenderThreads


    [ConfigurationProperty( "synch3D2DFlight", IsRequired = true )]
    public ConfigElement<int> Synch3D2DFlight
    {
      get
      {
        return (ConfigElement<int>) this["synch3D2DFlight"];
      }
    } // Synch3D2DFlight


    [ConfigurationProperty( "allowDual3DFlight", IsRequired = true )]
    public ConfigElement<int> AllowDual3DFlight
    {
      get
      {
        return (ConfigElement<int>) this["allowDual3DFlight"];
      }
    } // AllowDual3DFlight


    [ConfigurationProperty( "defaultCenterlineDisplay", IsRequired = true )]
    public ConfigElement<int> DefaultCenterlineDisplay
    {
      get
      {
        return (ConfigElement<int>) this["defaultCenterlineDisplay"];
      }
    } // DefaultCenterlineDisplay


    [ConfigurationProperty( "defaultLinkedDatasets", IsRequired = true )]
    public ConfigElement<int> DefaultLinkedDatasets
    {
      get
      {
        return (ConfigElement<int>) this["defaultLinkedDatasets"];
      }
    } // DefaultLinkedDatasets


    [ConfigurationProperty( "defaultCadSliderValue", IsRequired = true )]
    public ConfigElement<int> DefaultCadSliderValue
    {
      get
      {
        return (ConfigElement<int>) this["defaultCadSliderValue"];
      }
    } // DefaultCadSliderValue


    [ConfigurationProperty( "alternateProjectionLayouts", IsRequired = true )]
    public ConfigElement<int> AlternateProjectionLayouts
    {
      get
      {
        return (ConfigElement<int>) this["alternateProjectionLayouts"];
      }
    } // AlternateProjectionLayouts


    [ConfigurationProperty( "cadVendors", IsRequired = true, IsDefaultCollection = true )]
    [ConfigurationCollection( typeof( ProcessorElement ), AddItemName = "cadVendor" )]
    public CadVendorElementCollection CadVendors
    {
      get
      {
        return (CadVendorElementCollection) this["cadVendors"];
      }
    } // CadVendors


    [ConfigurationProperty( "allowDataExport", IsRequired = true )]
    public ConfigElement<int> AllowDataExport
    {
      get
      {
        return (ConfigElement<int>) this["allowDataExport"];
      }
    } // AllowDataExport


    [ConfigurationProperty( "cadDisplayName", IsRequired = true )]
    public ConfigElement<string> CADDisplayName
    {
      get
      {
        return (ConfigElement<string>) this["cadDisplayName"];
      }
    } // CADDisplayName


    [ConfigurationProperty( "enableSRT", IsRequired = true )]
    public ConfigElement<int> EnableSRT
    {
      get
      {
        return (ConfigElement<int>) this["enableSRT"];
      }
    } // EnableSRT


    [ConfigurationProperty( "outputSROI", IsRequired = true )]
    public ConfigElement<int> OutputSROI
    {
      get
      {
        return (ConfigElement<int>) this["outputSROI"];
      }
    } // OutputSROI


    [ConfigurationProperty( "useULDVolume", IsRequired = true )]
    public ConfigElement<int> UseULDVolume
    {
      get
      {
        return (ConfigElement<int>) this["useULDVolume"];
      }
    } // UseULDVolume

    #endregion

  } // class ViewerElement

  #endregion

  #region class CadVendorElement

  public class CadVendorElement : ConfigurationElement
  {

    #region properties

    [ConfigurationProperty( "imagePath", IsRequired = true )]
    public string Version
    {
      get
      {
        return (string) this["imagePath"];
      }
    } // ImagePath


    [ConfigurationProperty( "name", IsRequired = true )]
    public string Name
    {
      get
      {
        return (string) this["name"];
      }
    } // Name


    [ConfigurationProperty( "website", IsRequired = false )]
    public string Website
    {
      get
      {
        return (string) this["website"];
      }
    } // Website

    #endregion

  } // class CadVendorElement

  #endregion

  #region class CadVendorElementCollection

  [ConfigurationCollection( typeof( CadVendorElement ), AddItemName = "cadVendor", CollectionType = ConfigurationElementCollectionType.BasicMap )]
  public class CadVendorElementCollection : ConfigurationElementCollection
  {

    #region properties

    public CadVendorElement this[int index]
    {
      get
      {
        return (CadVendorElement) base.BaseGet( index );
      }
    }


    public new CadVendorElement this[string name]
    {
      get
      {
        return (CadVendorElement) base.BaseGet( name );
      }
    }

    #endregion

    #region ConfigurationElementCollection Members

    protected override ConfigurationElement CreateNewElement()
    {
      return new CadVendorElement();
    }


    protected override object GetElementKey( ConfigurationElement element )
    {
      return ( element as CadVendorElement ).Name;
    }

    #endregion

  } // class TaskElementCollection

  #endregion

} // namespace Viatronix.Colon.Configuration

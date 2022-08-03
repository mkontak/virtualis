// $Id: EnvrionmentArgs.cs,v 1.1 2006/12/21 11:27:23 romy Exp $ 
//
// Copyright © 2005, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form, 
// in whole or in part, without the prior written permission of Viatronix Inc. 
// Author: Romy Sreedharan ( mailto:romy@viatronix.com )

using System;
using System.Collections.Generic;
using System.Text;

namespace Viatronix.v3D.Processor.Plugins.CAD.Test
{
  /// <summary>
  /// Class which represents the environment variables for the test
  /// </summary>
  class EnvrionmentArgs
  {
    #region fields

    /// <summary>
    /// Flag indicates whether to run all processing or not
    /// </summary>
    private bool m_runPreProcess = false;

    /// <summary>
    /// AppId string
    /// </summary>
    private string m_appId = string.Empty;

    /// <summary>
    /// Working Folder
    /// </summary>
    private string m_workingFolder = string.Empty;

    /// <summary>
    /// DICOM Folder
    /// </summary>
    private string m_dicomFolder = string.Empty;

    /// <summary>
    /// Volume Folder
    /// </summary>
    private string m_volumeFolder = string.Empty;

    /// <summary>
    /// Pre processed folder
    /// </summary>
    private string m_preProcessedFolder = string.Empty;

    /// <summary>
    /// cad folder
    /// </summary>
    private string m_cadFolder = string.Empty;

    /// <summary>
    /// Conversion APP
    /// </summary>
    private string m_conversionApp = string.Empty;

    /// <summary>
    /// Pipeline App
    /// </summary>
    private string m_pipelineApp = string.Empty;

    /// <summary>
    /// CAD Plugin app
    /// </summary>
    private string m_cadPlugin = string.Empty;

    #endregion

    #region property

    /// <summary>
    /// APP ID
    /// </summary>
    public string AppId
    {
      get { return m_appId; }
      set { m_appId = value; }
    }//AppId

    /// <summary>
    /// DicomFolder
    /// </summary>
    public string DicomFolder
    {
      get { return m_dicomFolder; }
      set { m_dicomFolder = value; }
    }//DicomFolder

    /// <summary>
    /// WorkingFolder
    /// </summary>
    public string WorkingFolder
    {
      get { return m_workingFolder; }
      set { m_workingFolder = value; }
    }//WorkingFolder

    /// <summary>
    /// VolumeFolder
    /// </summary>
    public string VolumeFolder
    {
      get { return m_volumeFolder; }
      set { m_volumeFolder = value; }
    }//VolumeFolder

    /// <summary>
    /// CadFolder
    /// </summary>
    public string CadFolder
    {
      get { return m_cadFolder; }
      set { m_cadFolder = value; }
    }//CadFolder

    /// <summary>
    /// PreProcessedFolder
    /// </summary>
    public string PreProcessedFolder
    {
      get { return m_preProcessedFolder; }
      set { m_preProcessedFolder = value; }
    }//PreProcessedFolder

    /// <summary>
    /// RunPreProcess Flag
    /// </summary>
    public bool RunPreProcess
    {
      get { return m_runPreProcess; }
      set { m_runPreProcess = value; }
    }//RunPreProcess

    /// <summary>
    /// ConversionApp
    /// </summary>
    public string ConversionApp
    {
      get { return m_conversionApp; }
      set
      {
        string processorFolder = Viatronix.v3D.Utilities.Registry.SystemSettings.ProcessorDirectory;
        m_conversionApp = System.IO.Path.Combine(processorFolder, value);
      }
    }//ConversionApp

    /// <summary>
    /// PipelineApp
    /// </summary>
    public string PipelineApp
    {
      get{ return m_pipelineApp; }
      set
      {
        string processorFolder = Viatronix.v3D.Utilities.Registry.SystemSettings.ProcessorDirectory;
        m_pipelineApp = System.IO.Path.Combine(processorFolder, value);
      }
    }//PipelineApp

    /// <summary>
    /// PipelineApp
    /// </summary>
    public string CADPlugin
    {
      get { return m_cadPlugin; }
      set
      {
        string processorFolder = Viatronix.v3D.Utilities.Registry.SystemSettings.ProcessorDirectory;
        m_cadPlugin = System.IO.Path.Combine(processorFolder, value);
      }
    }//PipelineApp

    #endregion
  }//EnvrionmentArgs
}//namespace Viatronix.v3D.Processor.Plugins.CAD.Test



#region
// $Log: EnvrionmentArgs.cs,v $
// Revision 1.1  2006/12/21 11:27:23  romy
// initial version
//
// $Header: /CVS/cvsRepository/V3D/src/Viatronix.v3D.Processor.Plugins.CAD.Test/EnvrionmentArgs.cs,v 1.1 2006/12/21 11:27:23 romy Exp $
// $Id: EnvrionmentArgs.cs,v 1.1 2006/12/21 11:27:23 romy Exp $
#endregion
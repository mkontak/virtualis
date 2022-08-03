using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

using GalaSoft.MvvmLight;

using Viatronix.Imapi;

namespace Viatronix.Utilities.Transfer.ViewModels
{
  /// <summary>
  /// The device view model.
  /// </summary>
  public class DeviceViewModel : ViewModelBase
  {

    #region fields

    /// <summary>
    /// The device id for the device.
    /// </summary>
    private string m_deviceId = string.Empty;

    /// <summary>
    /// The description of the device.
    /// </summary>
    private string m_description = string.Empty;

    #endregion

    #region construction

    /// <summary>
    /// Constructor.
    /// </summary>
    public DeviceViewModel()
    {
    } // DeviceViewModel()

    #endregion

    #region properties

    /// <summary>
    /// Gets or sets the device id.
    /// </summary>
    public string DeviceId
    {
      get { return m_deviceId; }
      private set
      {
        m_deviceId = value;
        RaisePropertyChanged("DeviceId");
      }
    } // DeviceId


    /// <summary>
    /// Gets or sets the description.
    /// </summary>
    public string Description
    {
      get { return m_description; }
      private set
      {
        m_description = value;
        RaisePropertyChanged("Description");
      }
    } // Description

    #endregion

    #region methods

    /// <summary>
    /// Performs the initialization of the device view model.
    /// </summary>
    /// <param name="deviceId">The device id.</param>
    public void Initialize(string deviceId)
    {
      DeviceId = deviceId;
    } // Initialize( deviceId )


    /// <summary>
    /// Refreshes the device description.
    /// </summary>
    public void Refresh()
    {
      DiscRecorder recorder = null;
      DiscFormatData discData = null;
      FileSystemImage fsImage = null;

      try
      {
        recorder = new DiscRecorder();
        recorder.InitializeDiscRecorder(m_deviceId);

        discData = new DiscFormatData();
        if (discData.IsCurrentMediaSupported(recorder))
        {
          discData.Recorder = recorder;

          fsImage = new FileSystemImage();
          fsImage.ChooseImageDefaults(recorder);

          Description = string.Format("{0}  {1}  {2}  {3}", recorder.ProductId.Trim(),
            EnumUtils.ConvertMediaPhysicalTypeToShortString(discData.CurrentPhysicalMediaType),
            (fsImage.FreeMediaBlocks * 2048) > 1000000000 ? Math.Round((double)(fsImage.FreeMediaBlocks * 2048) / 1000000000.0, 2) + " GB" : (int)(((double)(fsImage.FreeMediaBlocks * 2048) / 1000000.0) + 0.5) + " MB",
            discData.MediaPhysicallyBlank | discData.MediaHeuristicallyBlank ? "Media Blank" : "Media Not Empty");
        }
        else
        {
          Description = string.Format("{0}  No Media In Device", recorder.ProductId.Trim());
        }
      }
      finally
      {
        if (fsImage != null)
        {
          fsImage.Dispose();
          fsImage = null;
        }

        if (discData != null)
        {
          discData.Dispose();
          discData = null;
        }

        if (recorder != null)
        {
          recorder.Dispose();
          recorder = null;
        }
      }
    } // Refresh()

    #endregion

  } // class DeviceViewModel
} // namespace Viatronix.Utilities.Transfer.ViewModels

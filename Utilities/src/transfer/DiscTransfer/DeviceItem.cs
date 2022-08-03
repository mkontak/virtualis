using System;
using System.Linq;
using System.Text;
using System.Windows;
using System.Threading.Tasks;
using System.Collections.Generic;
using System.Runtime.InteropServices;

using GalaSoft.MvvmLight;

using Viatronix.Imapi;

namespace Viatronix.Utilities.Transfer.DiscTransfer
{
  /// <summary>
  /// A combo box item to show the device.
  /// </summary>
  public class DeviceItem
  {

    #region fields

    /// <summary>
    /// The device unique identifier.
    /// </summary>
    private string m_deviceId = string.Empty;

    #endregion

    #region construction

    /// <summary>
    /// Constructor.
    /// </summary>
    /// <param name="deviceId">The device identifier.</param>
    public DeviceItem(string deviceId)
    {
      m_deviceId = deviceId;
    } // DeviceItem( deviceId )

    #endregion

    #region properties

    /// <summary>
    /// Gets the device id.
    /// </summary>
    public string DeviceId
    {
      get { return m_deviceId; }
    } // DeviceId

    #endregion

    #region methods

    /// <summary>
    /// Returns a string representation of this object.
    /// </summary>
    /// <returns></returns>
    public override string ToString()
    {
      string insertedMedia = "No Media Inserted";
      string capacityDisplay = "";
      Int64 diskCapacity = 0;
      bool mediaBlank = false;

      if (m_deviceId != string.Empty)
      {
        DiscRecorder recorder = null;
        DiscFormatData discData = null;
        FileSystemImage fsImage = null;

        recorder = new DiscRecorder();
        recorder.InitializeDiscRecorder(DeviceId);

        discData = new DiscFormatData();
        if (discData.IsCurrentMediaSupported(recorder))
        {
          discData.Recorder = recorder;
          MediaTypes type = discData.CurrentPhysicalMediaType;

          fsImage = new FileSystemImage();
          fsImage.ChooseImageDefaultsForMediaType((MediaTypes)discData.CurrentPhysicalMediaType);

          insertedMedia = EnumUtils.ConvertMediaPhysicalTypeToShortString(discData.CurrentPhysicalMediaType);
          diskCapacity = (Int64)fsImage.FreeMediaBlocks * (Int64)2048;
          capacityDisplay = diskCapacity > 1000000000 ? Math.Round((double)diskCapacity / 1000000000.0, 2) + " GB" : (int)(((double)diskCapacity / 1000000.0) + 0.5) + " MB";
          mediaBlank = discData.MediaHeuristicallyBlank || (!discData.MediaHeuristicallyBlank && discData.MediaPhysicallyBlank);

          return string.Format("{0}  {1}  {2}  {3}", recorder.ProductId.Trim(), insertedMedia, capacityDisplay, mediaBlank ? "Media Blank" : "Media Not Empty");
        }
        else
        {
          return string.Format("{0}  No Media In Device", recorder.ProductId.Trim());
        }
      }

      return string.Empty;
    } // ToString()

    #endregion

  } // class DeviceItem
} // namespace Viatronix.Utilities.Transfer.DiscTransfer

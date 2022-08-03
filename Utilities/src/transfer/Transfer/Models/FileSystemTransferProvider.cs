using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace Viatronix.Utilities.Transfer.Models
{
  /// <summary>
  /// A model to transfer to a file system directory.
  /// </summary>
  public class FileSystemTransferProvider : TransferModel
  {

    #region fields

    #endregion

    #region construction

    /// <summary>
    /// Constructor.
    /// </summary>
    public FileSystemTransferProvider()
    {
    } // FileSystemTransferProvider()

    #endregion

    #region methods

    /// <summary>
    /// Transfers the data to the specified directory.
    /// </summary>
    /// <param name="studies">The studies to transfer.</param>
    /// <param name="args">Arguments to take into account.</param>
    public override void Transfer(ViewModels.StudyViewModel[] studies, object args)
    {
      throw new NotImplementedException();
    } // Transfer( studies, args )

    #endregion
    
  } // class FileSystemTransferProvider
} // namespace Viatronix.Utilities.Transfer.Models

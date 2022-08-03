using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

namespace Viatronix.Dicom.MVVM.Model
{
  public interface IDataService
  {
    void GetElements(string file, Action<ElementCollection, Exception> callback);

    void GetGroups(Action<IEnumerable<Group>, Exception> callback);
  }
}

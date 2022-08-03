using System;
using System.Collections.Generic;
using System.Text;
using Viatronix.Enterprise.Entities;
using Viatronix.v3D.Core;

namespace Viatronix.Console
{
  public class ViewerLauncherComparer : IComparer<IViewerLauncher>
  {
    #region IComparer<IViewerLauncher> members

    /// <summary>
    /// Looks for sessions within the data sets and compares the DateRecieved and if they are equal the 
    /// Description.
    /// </summary>
    /// <param name="obj1">The first object to compare.</param>
    /// <param name="obj2">The second object to compare.</param>
    /// <returns>
    /// 0 if both launchers don't have sessions within their datasets, or if the descriptions are equal.
    /// +1 if a session was found in the second object, but not in the first or under normal comparison conditions.
    /// -1 if a session was found in the first object, but not in the second or under normal comparison conditions.
    /// </returns>
    int IComparer<IViewerLauncher>.Compare(IViewerLauncher obj1, IViewerLauncher obj2)
    {
      Series session1 = null;
      Series session2 = null;

      foreach(List<Series> seriesCollection in obj1.Datasets)
      {
        Series temp = null;

        if (seriesCollection.TryFind("Session", ref temp))
        { 
          session1 = temp;
          break;
        } // if(temp != null)

      } // foreach(List<Series> seriesCollection in obj1.Datasets)

      foreach (List<Series> seriesCollection in obj2.Datasets)
      {
        Series temp = null;
          
        if ( seriesCollection.TryFind("Session", ref temp) )
        {
          session2 = temp;
          break;
        } // if(temp != null)

      } // foreach(List<Series> seriesCollection in obj1.Datasets)

      if(session1 == null && session2 == null)
      {
        return 0;
      } // if(session1 == null && session2 == null)
      else if(session1 != null && session2 == null)
      {
        return -1;
      } // else if(session1 != null && session2 == null)
      else if(session1 == null && session2 != null)
      {
        return 1;
      } // else if(session1 == null && session2 != null)
      else
      {
        if(session1.DateReceived == session2.DateReceived)
        {
          return session1.Description.CompareTo(session2.Description);
        } // if(session1.DateReceived == session2.DateReceived)
        else
        {
          return (session1.DateReceived == session2.DateReceived ? 0 : (session1.DateReceived < session2.DateReceived ? -1 : 1));
        }
      } // else

    } // IComparer<IViewerLauncher>.Compare(IViewerLauncher obj1, IViewerLauncher obj2)

    #endregion
  } // ViewerLauncherComparer
}

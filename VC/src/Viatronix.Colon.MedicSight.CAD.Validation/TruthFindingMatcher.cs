using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

namespace Viatronix.Colon.Medicsight.CAD.Validation
{
  /// <summary>
  /// Finds the matching points from the truth points to the cad finding points.
  /// </summary>
  public static class TruthFindingMatcher
  {

    #region methods

    /// <summary>
    /// Finds the best matches between the two lists.
    /// 
    /// Point 1 searches the list finding the point with the minimum euclidean distance to himself and those two points are added to the result.  The two are removed from the list.
    /// Point 2 searches the remaining list and so on.
    /// </summary>
    /// <param name="truthPoints">The list of points occupied in the truth file.</param>
    /// <param name="cadFindings">The list of points in the CAD Findings Xml file.</param>
    /// <returns>A list of married points that are closest.</returns>
    public static List<KeyValuePair<Point3Df, Point3Df>> FindMatches(List<Point3Df> truthPoints, List<Point3Df> cadFindings)
    {
      List<KeyValuePair<Point3Df, Point3Df>> results = new List<KeyValuePair<Point3Df, Point3Df>>();
      List<Point3Df> availablePoints = new List<Point3Df>();

      // Copy the list.
      foreach(Point3Df point in cadFindings)
      {
        availablePoints.Add(point);
      }

      for (int i = 0; i < truthPoints.Count; ++i)
      {
        int position = -1;
        double minDistance = double.MaxValue;

        for (int j = 0; j < availablePoints.Count; ++j)
        {
          double distance = truthPoints[i].GetEuclideanDistance(availablePoints[j]);

          if (distance < minDistance)
          {
            position = j;
            minDistance = distance;
          }
        }

        results.Add(new KeyValuePair<Point3Df, Point3Df>(truthPoints[i], availablePoints[position]));
        availablePoints.RemoveAt(position);
      }

      return results;
    } // FindMatches( truthPoints, cadFindings )

    #endregion

  } // class TruthFindingMatcher
} // namespace Viatronix.Colon.Medicsight.CAD.Validation

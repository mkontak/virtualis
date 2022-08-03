

using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

namespace Viatronix
{

  public class EventArgs<T> : EventArgs
  {

    #region fields

    /// <summary>
    /// arguements
    /// </summary>
    private T m_args = default(T);

    #endregion

    #region construction

    /// <summary>
    /// default constructor
    /// </summary>
    public EventArgs()
    {
    }


    /// <summary>
    /// Creates a new instance of EventArgs
    /// </summary>
    /// <param name="args">arguments</param>
    public EventArgs(T args)
    {
      m_args = args;
    } // EventArgs( args )

    #endregion


    #region properties

    /// <summary>
    /// Gets or sets the args
    /// </summary>
    public T Args
    {
      get { return m_args; }
      set { m_args = value; }
    } // Args

    #endregion

  } // class EventArgs<T>



} // namespace Viatronix

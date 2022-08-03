using System;
using System.Collections;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.ComponentModel;
using System.ServiceModel;
using System.Configuration;


namespace Viatronix.Enterprise.Servers
{

  /// <summary>
  /// Server interface for enterprise
  /// </summary>
  [ServiceContract(Namespace = "http://Viatronix.Enterprise.Servers")]
  interface IServer
  {

    /// <summary>
    /// Aborted event
    /// </summary>
    event EventHandler Aborted;

    /// <summary>
    /// Started event
    /// </summary>
    event EventHandler Started;

    /// <summary>
    /// Failed event
    /// </summary>
    event EventHandler<EventArgs<Exception>> Failed;

    /// <summary>
    /// Name of the server
    /// </summary>
    string Name { get; }

    /// <summary>
    /// Starts the server asynchronously
    /// </summary>
    void Start(Dictionary<string,string> parameters);

    /// <summary>
    /// Waits for the asynchronous 
    /// </summary>
    /// <param name="timeOut"></param>
    void Wait(int timeOut);

    /// <summary>
    /// Runs the server synchronously
    /// </summary>
    void RunSynchronously();

    /// <summary>
    /// Aborts the server
    /// </summary>
    void Abort(Dictionary<string, string> parameters);

    /// <summary>
    /// Aborts the server
    /// </summary>
    void Abort();

    /// <summary>
    /// Generates XML from the server
    /// </summary>
    /// <returns></returns>
    string ToXml();


 
  }

}

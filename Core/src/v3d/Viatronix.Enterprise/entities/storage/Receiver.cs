using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Xml;
using System.IO;

namespace Viatronix.Enterprise.Entities
{
  /// <summary>
  /// Class corresponds  to the Receiver table in the database
  /// </summary>
  public class Receiver : Entity
  {

    #region class Attributes

    /// <summary>
    /// Attributes
    /// </summary>
    public static class Attributes
    {

      public const string AETitle = "aetitle";
      public const string Server = "server";
      public const string Association = "association";
      public const string Date = "date";
      public const string Count = "cnt";
      public const string Uid = "uid";
      public const string Name = "name";
      public const string Orientation = "ort";
    } // class Attributes

    #endregion

    #region fields

     /// <summary>
    /// Patient name
    /// </summary>
    private string m_name = string.Empty;

    /// <summary>
    /// Association id
    /// </summary>
    private string m_association = string.Empty;

    /// <summary>
    /// Server name
    /// </summary>
    private string m_server = string.Empty;

    /// <summary>
    /// Series uid
    /// </summary>
    private string m_uid = string.Empty;

    /// <summary>
    /// AETitle
    /// </summary>
    private string m_aetitle = string.Empty;

    /// <summary>
    /// Image count
    /// </summary>
    private UInt32 m_count = 0;

    /// <summary>
    /// Last received
    /// </summary>
    private DateTimeOffset? m_date = null;

    /// <summary>
    /// Series orientation
    /// </summary>
    private string m_orientation = string.Empty;

    #endregion


    #region properties
    /// <summary>
    /// Gets the association id
    /// </summary>
    public string Association
    { 
      get { return m_association; }
      private set { SetProperty<string>("Association", value, ref m_association);  }
    }

    /// <summary>
    /// Gets/Sets the server name
    /// </summary>
    public string Server
    { 
      get { return m_server; }
      set { SetProperty<string>("Server", value, ref m_server);  }
    }

    /// <summary>
    /// Gets/Sets the AETitle
    /// </summary>
    public string AETitle
    {
      get { return m_aetitle;  }
      set { SetProperty<string>("AETitle", value, ref m_aetitle );  }
    }

    /// <summary>
    /// Gets the image count
    /// </summary>
    public UInt32 Count
    { 
      get { return m_count;  }
      private set { SetProperty<UInt32>("Count", value, ref m_count);  }
    }

    /// <summary>
    /// Gets the series uid
    /// </summary>
    public string Uid
    { 
      get { return m_uid; }
      set { SetProperty<string>("Uid", value, ref m_uid); }
    }

    /// <summary>
    /// Gets the patient name
    /// </summary>
    public string Name
    { 
      get { return m_name;  }
      set { SetProperty<string>("PatientName", value, ref m_name);  }
    }

    /// <summary>
    /// Gets the orientation 
    /// </summary>
    public string Orientation
    { 
      get { return m_orientation; }
      set { SetProperty<string>("Orientation", value, ref m_orientation); }
    }

    /// <summary>
    /// Gets the last image receive date
    /// </summary>
    public DateTimeOffset Date
    {
      get { return m_date.Value; }
      set { SetProperty<DateTimeOffset?>("Date", value, ref m_date ); }
    }


    #endregion


    #region methods

 
    /// <summary>
    /// Creates the received from the xml string
    /// </summary>
    /// <param name="xml"></param>
    /// <returns></returns>
    public static Receiver Create(string xml)
    {
      return Create(xml.ToXmlNode());

    } // Create(string xml)

    /// <summary>
    /// Creates the Receiver object from the xml node
    /// </summary>
    /// <param name="node"></param>
    /// <returns>Receiver</returns>
    public static Receiver Create(XmlNode node)
    {

      Receiver receiver = new Receiver();

      if (receiver == null)
        throw new OutOfMemoryException("Failed to allocate receiver");

      receiver.Load(node);


      return receiver;
    } // Load(XmlNode node)



    #endregion

    #region IEntity

    /// <summary>
    /// Creates the Receiver object from the xml node
    /// </summary>
    /// <param name="node"></param>
    /// <returns>Receiver</returns>
    public override void Load(XmlNode node)
    {

      base.Load(node);

      m_date = DateTimeOffset.Parse(node.Attributes[Attributes.Date].Value);
      m_aetitle = node.Attributes[Attributes.AETitle].Value;
      m_association = node.Attributes[Attributes.Association].Value;
      m_count = UInt32.Parse(node.Attributes[Attributes.Count].Value);
      m_name = node.Attributes[Attributes.Name].Value;
      m_server = node.Attributes[Attributes.Server].Value;
      m_uid = node.Attributes[Attributes.Uid].Value;
      m_orientation = node.Attributes[Attributes.Orientation].Value;

      

    } // Load(XmlNode node)


    /// <summary>
    /// Creates the User object
    /// </summary>
    /// <param name="node"></param>
    /// <returns></returns>
    public override object Clone()
    {

      Receiver rec = new Receiver();
      CopyTo(rec);

       // Initialize
      rec.m_date = m_date;
      rec.m_aetitle = m_aetitle;
      rec.Association = m_association;
      rec.m_count = m_count;
      rec.m_name = m_name;
      rec.m_server = m_server;
      rec.m_uid = m_uid;


      return rec;

 
    } // Clone()


    /// <summary>
    /// Creates an xml representation of a User object.
    /// </summary>
    /// <returns>Xml string representing the User object.</returns>
    public override string ToXml()
    {

      StringBuilder builder = new StringBuilder();
      using (XmlWriter writer = XmlWriter.Create(builder, new XmlWriterSettings() { OmitXmlDeclaration = true, Indent = false }))
      {


        writer.WriteStartElement(this.Tag);
        {



          writer.WriteAttributeString(Attributes.Uid, m_uid);
          writer.WriteAttributeString(Attributes.Server, m_server);

          if ( m_date.HasValue )
            writer.WriteAttributeString(Attributes.Date, m_date.Value.ToString("O"));

          writer.WriteAttributeString(Attributes.Association, m_association);
          writer.WriteAttributeString(Attributes.Name, m_name);
          writer.WriteAttributeString(Attributes.AETitle, m_aetitle);
          writer.WriteAttributeString(Attributes.Orientation, m_orientation);
          writer.WriteAttributeString(Attributes.Count, m_count.ToString());

          WriteAttributeStrings(writer);
          WriteElements(writer);
        }

        writer.WriteEndElement();
       }

      return builder.ToString();

    } // ToXml()

    /// <summary>
    /// Checks for value equality of task objects
    /// </summary>
    /// <param name="obj">A job to compare with this instance or a null reference</param>
    /// <returns>true if they are equal values; otherwise false</returns>
    public override bool Equals(IEntity obj)
    {
      Receiver other = obj as Receiver;

      return other != null &&
             base.Equals(other) &&
              string.Equals(m_aetitle, other.m_aetitle) &&
                string.Equals(m_association, other.m_association) &&
                  string.Equals(m_server, other.m_server);

    } // Equals( obj )


    #endregion

  } // class Receiver

}

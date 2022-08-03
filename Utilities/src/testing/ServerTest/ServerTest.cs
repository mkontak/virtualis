using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Linq;
using System.Text;
using System.Xml;
using System.Windows.Forms;
using Viatronix.Dicom;

namespace Viatronix.Utilities.Dicom
{
  public partial class ServerTestForm : Form
  {
    /// <summary>
    /// Update status delegate 
    /// </summary>
    /// <param name="status"></param>
    private delegate void UpdateStatusDelegate(string [] items);

    private delegate void EnableFormDelegate(bool enable);

    private XmlNodeList m_serverList;

    private bool m_closing = false;

    private List<Viatronix.Dicom.Server> m_servers = new List<Viatronix.Dicom.Server>();

    /// <summary>
    /// Update status delegate
    /// </summary>
    private UpdateStatusDelegate m_updateStatus = null;

    /// <summary>
    /// Update status delegate
    /// </summary>
    private EnableFormDelegate m_enableForm = null;

     public ServerTestForm()
    {
      m_updateStatus = new UpdateStatusDelegate(this.UpdateStatus);
      m_enableForm = new EnableFormDelegate(this.EnableForm);

      int count = System.Configuration.ConfigurationManager.AppSettings.Count;

 
      InitializeComponent();

      EnableForm(true);

      m_serverList = (XmlNodeList)System.Configuration.ConfigurationManager.GetSection("servers");

      foreach (XmlNode node in m_serverList)
      {


        Viatronix.Dicom.Server server = Viatronix.Dicom.Server.Create(node);

        server.Started += new Viatronix.Dicom.ServerEventHandler(Server_Started);
        server.Stopped += new Viatronix.Dicom.ServerEventHandler(Server_Stopped);
        server.Completed += new Viatronix.Dicom.ServerEventHandler(Server_Completed);
        server.Completed += new Viatronix.Dicom.ServerEventHandler(Server_Failed);
        server.AssociationStarted += new Viatronix.Dicom.ServerEventHandler(OnAssociationStarted);
        server.AssociationCompleted += new Viatronix.Dicom.ServerEventHandler(OnAssociationCompleted);
        server.ImageReceived += new Viatronix.Dicom.ServerEventHandler(OnImageReceived);
        server.AssociationFailed += new Viatronix.Dicom.ServerEventHandler(OnAssociationFailed);

        m_servers.Add(server);

        string[] items = new string[8];
        items[0] = server.Id.ToString();
        items[1] = "Server";
        items[2] = "0";
        items[3] = string.Empty;
        items[4] = server.AETitle;
        items[5] = string.Empty;
        items[6] = server.Port.ToString();
        items[7] = string.Empty;

        server.Start();

        UpdateStatus(items);


      }

 
    }

    private void OnStartClick(object sender, EventArgs e)
    {
      m_listView.Items.Clear();

     EnableForm(false);

        foreach (  Viatronix.Dicom.Server server in m_servers )
        {
          server.Start();
        }



   }


    /// <summary>
    /// Updates the status box
    /// </summary>
    /// <param name="status"></param>
    private void UpdateStatus(string [] items)
    {

      if (m_closing) 
        return;
      
      //=================================================================================================
      // Only update if we are not closing otherwise we error because the resource has been disposed
      // and the process thread has not terminated yet.
      //================================================================================================

        if (this.InvokeRequired)
        {
          this.Invoke(m_updateStatus, new object[] { items });
        }
        else
        {

          ListViewItem listViewItem = null;
          foreach ( ListViewItem item in m_listView.Items )
          {
            if ( item.SubItems[0].Text == items[0] )
            {

              listViewItem = item;
              break;
            }

          }

          if (listViewItem == null )
          {

            listViewItem = new ListViewItem(items);

            if (items[3] == "Started")
            {
              listViewItem.ForeColor = Color.DarkCyan;
            }
            else if (items[3] == "Completed")
            {
              listViewItem.ForeColor = Color.DarkGoldenrod;
            }
            else if (items[3] == "Failed")
            {

              listViewItem.ForeColor = Color.DarkRed;
            }
            else if (items[3] == "Stopped")
            {
              listViewItem.ForeColor = Color.DarkViolet;
            }
            else if ( items[3] == "Image Received" )
            {
              listViewItem.ForeColor = Color.ForestGreen;

            }
            else
            {

              listViewItem.ForeColor = Color.Black;
            }

            m_listView.Items.Add(listViewItem);
          }
          else
          {
            if (items[3] == "Started")
            {
              listViewItem.ForeColor = Color.DarkCyan;
            }
            else if ( items[3] == "Completed" )
            {
              listViewItem.ForeColor = Color.DarkGoldenrod;
            }
            else if ( items[3] == "Failed" )
            {

              listViewItem.ForeColor = Color.DarkRed;
            }
            else if ( items[3] == "Stopped" )
            {
              listViewItem.ForeColor = Color.DarkViolet;
            }
            else if (items[3] == "Image Received")
            {
              listViewItem.ForeColor = Color.ForestGreen;

            }
            else
            {

              listViewItem.ForeColor = Color.Black;
            }

            for (int i = 0; i < 8; listViewItem.SubItems[i].Text = items[i], ++i) ;
          
          }

        }

    } // UpdateStatus(string status)

    /// <summary>
    /// Enable the form
    /// </summary>
    /// <param name="status"></param>
    private void EnableForm(bool enable)
    {

      if (m_closing)
        return;

      //=================================================================================================
      // Only update if we are not closing otherwise we error because the resource has been disposed
      // and the process thread has not terminated yet.
      //================================================================================================

      if (this.InvokeRequired)
      {
        this.Invoke(m_enableForm, new object[] { enable });
      }
      else
      {
        m_menuStrip.Items[0].Enabled = enable;
        m_menuStrip.Items[1].Enabled = !enable;
      }
    } // UpdateStatus(string status)

    void Server_Started(object sender, ServerEventArgs args)
    {
      string [] items = new string [8];
      items[0] = args.Source.ToString();
      items[1] = "Server";
      items[2] = "";
      items[3] = "Started";
      items[4] = args.CalledAETitle;
      items[5] = string.Empty;
      items[6] = args.Port.ToString();
      items[7] = args.Message;

     UpdateStatus( items );
    }

    void Server_Stopped(object sender, ServerEventArgs args)
    {

      string[] items = new string[8];
      items[0] = args.Source.ToString();
      items[1] = "Server";
      items[2] = "";
      items[3] = "Stopped";
      items[4] = args.CalledAETitle;
      items[5] = string.Empty;
      items[6] = args.Port.ToString();
      items[7] = args.Message;

      UpdateStatus(items);
      EnableForm(true);

 
    }

    void Server_Completed(object sender, ServerEventArgs args)
    {
      string[] items = new string[8];
      items[0] = args.Source.ToString();
      items[1] = "Server";
      items[2] = "";
      items[3] = "Completed";
      items[4] = args.CalledAETitle;
      items[5] = string.Empty;
      items[6] = args.Port.ToString();
      items[7] = args.Message;

      UpdateStatus(items);
      EnableForm(true);
    }

    void Server_Failed(object sender, ServerEventArgs args)
    {
      string[] items = new string[8];
      items[0] = args.Source.ToString();
      items[1] = "Server";
      items[2] = "";
      items[3] = "Failed";
      items[4] = args.CalledAETitle;
      items[5] = string.Empty;
      items[6] = args.Port.ToString();
      items[7] = args.Message;

      UpdateStatus(items);
      EnableForm(true);
    }

    void OnAssociationStarted(object sender, ServerEventArgs args)
    {
      string[] items = new string[8];
      items[0] = args.Source.ToString();
      items[1] = "Receiver";
      items[2] = args.Completed.ToString();
      items[3] = "Started";
      items[4] = args.CalledAETitle;
      items[5] = args.CallingAETitle;
      items[6] = string.Empty;
      items[7] = args.Message;

      UpdateStatus(items);
    }

    void OnAssociationFailed(object sender, ServerEventArgs args)
    {
      string[] items = new string[8];
      items[0] = args.Source.ToString();
      items[1] = "Receiver";
      items[2] = args.Completed.ToString();
      items[3] = "Failed";
      items[4] = args.CalledAETitle;
      items[5] = args.CallingAETitle;
      items[6] = string.Empty;
      items[7] = args.Message;

      UpdateStatus(items);
    }

    void OnAssociationCompleted(object sender, ServerEventArgs args)
    {
      string[] items = new string[8];
      items[0] = args.Source.ToString();
      items[1] = "Receiver";
      items[2] = args.Completed.ToString();
      items[3] = "Completed";
      items[4] = args.CalledAETitle;
      items[5] = args.CallingAETitle;
      items[6] = string.Empty;
      items[7] = args.Message;

      UpdateStatus(items);
    }

    void OnImageReceived(object sender, ServerEventArgs args)
    {
      string[] items = new string[8];
      items[0] = args.Source.ToString();
      items[1] = "Receiver";
      items[2] = args.Completed.ToString();
      items[3] = "Image Received";
      items[4] = args.CalledAETitle;
      items[5] = args.CallingAETitle;
      items[6] = string.Empty;
      items[7] = args.Message;

      UpdateStatus(items);

      XmlDocument doc = new XmlDocument();
      doc.LoadXml(args.Header);
      

      XmlTextWriter writer = new XmlTextWriter("lastDicomHeader.xml", Encoding.ASCII);
      writer.Formatting = Formatting.Indented;

      doc.Save(writer);

      writer.Close();

      //using ( System.Data.SqlClient.SqlConnection connection = new System.Data.SqlClient.SqlConnection("Data Source=APOLLO\\V3DENT;Initial Catalog=Archive; User Id=v3duser; Password=qwe321$") )
      //{
        
      //  connection.Open();

      //  System.Data.SqlClient.SqlCommand command = new System.Data.SqlClient.SqlCommand("sp_receiveDicomImage",connection);

      //  command.CommandType = CommandType.StoredProcedure;
      //  command.Parameters.Add(new System.Data.SqlClient.SqlParameter("SOURCE","source"));
      //  command.Parameters.Add(new System.Data.SqlClient.SqlParameter("RECEIVER","receiver"));
      //  command.Parameters.Add(new System.Data.SqlClient.SqlParameter("XML","<dicomHeader>" + args.DicomHeader + "</dicomHeader>"));
      //  command.Parameters.Add(new System.Data.SqlClient.SqlParameter("FILEPATH", SqlDbType.NVarChar, 512));

      //  command.Parameters[3].Direction = ParameterDirection.Output;

      //  command.ExecuteNonQuery();

      //  args.FilePath = command.Parameters[3].Value.ToString();
      //}

    }

    private void OnAbortClick(object sender, EventArgs e)
    {
      foreach (Viatronix.Dicom.Server server in m_servers)
      {
        server.Stop();
      }

    }

    private void ServerTestForm_FormClosed(object sender, FormClosedEventArgs e)
    {
      m_closing = true;

      // Abort
      OnAbortClick(sender, e);

      System.Threading.Thread.Sleep(1000);
    }

 
  }
}

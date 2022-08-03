using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Net;
using System.Net.Mail;

namespace Viatronix
{
  /// <summary>
  /// 
  /// </summary>
  class Email
  {

    /// <summary>
    /// Main entry point to console application used to send email
    /// </summary>
    /// <param name="args">Arguement list</param>
    static int Main(string[] args)
    {

      try
      {
        
        // Set up default values based on the parameters in the Application Setting section of the confi file
        string to = System.Configuration.ConfigurationManager.AppSettings["to"];
        string server = System.Configuration.ConfigurationManager.AppSettings["server"];
        string portValue = System.Configuration.ConfigurationManager.AppSettings["port"];
        string from = System.Configuration.ConfigurationManager.AppSettings["from"];
        string user = System.Configuration.ConfigurationManager.AppSettings["user"];
        string password = System.Configuration.ConfigurationManager.AppSettings["password"];
        string ssl = System.Configuration.ConfigurationManager.AppSettings["ssl"];
        string subject = System.Configuration.ConfigurationManager.AppSettings["subject"];
        bool enableSSL = (ssl.Length > 0 ? Boolean.Parse(ssl) : false);
        int port = (portValue.Length == 0 ? 0 : int.Parse(portValue));

        string body = string.Empty;
        string attachments = string.Empty;

        //======================================================
        // Parse the arguement list to override any settings
        //=====================================================
        for ( int i = 0; i < args.Length; ++i )
        {

          //===========================
          // SERVER setting
          //============================
          if (args[i] == "-server")
          {
            if (++i == args.Length)
            {
              Console.WriteLine("No server specified");
              return 1;
            }
            server = args[i];
          }
          //============================
          // PORT setting
          //===========================
          else if (args[i] == "-port")
          {
            if (++i == args.Length)
            {
              Console.WriteLine("No port specified");
              return 1;
            }
            port = int.Parse(args[i]);

          }
          //============================
          // USER setting
          //===========================
          else if (args[i] == "-user")
          {
            if (++i == args.Length)
            {
              Console.WriteLine("No user specified");
              return 1;
            }
            user = args[i];

          }
          //============================
          // PASSWORD setting
          //===========================
          else if (args[i] == "-password")
          {
            if (++i == args.Length)
            {
              Console.WriteLine("No password specified");
              return 1;
            }
            password = args[i];

          }
          //============================
          // SSL setting
          //===========================
          else if (args[i] == "-ssl")
          {
            enableSSL = true;
          }
          //============================
          // FROM setting
          //===========================
          else if (args[i] == "-from")
          {
            if (++i == args.Length)
            {
              Console.WriteLine("No from specified");
              return 1;
            }
            from = args[i];

          }
          //============================
          // TO setting
          //===========================
          else if (args[i] == "-to")
          {
            if (++i == args.Length)
            {
              Console.WriteLine("No to specified");
              return 1;
            }

            to = args[i];

          }
          //============================
          // BODY setting
          //===========================
          else if (args[i] == "-body")
          {
            if (++i == args.Length)
            {
              Console.WriteLine("No body specified");
              return 1;
            }

            body += args[i].Replace("<NL>","\n");

          }
          //============================
          // BODY setting
          //===========================
          else if (args[i] == "-bodyFile")
          {
            if (++i == args.Length)
            {
              Console.WriteLine("No body file specified");
              return 1;
            }

            System.IO.StreamReader reader = new System.IO.StreamReader(args[i]);

            body += reader.ReadToEnd();

            reader.Close();
 
          }
          //============================
          // SUBJECT setting
          //===========================
          else if (args[i] == "-subject")
          {
            if (++i == args.Length)
            {
              Console.WriteLine("No subject specified");
              return 1;
            }

            subject = args[i];

          }
          //============================
          // ATTACHMENTS setting
          //===========================
          else if (args[i] == "-a")
          {
            if (++i == args.Length)
            {
              Console.WriteLine("No attachments specified");
              return 1;
            }

            attachments = args[i];
          }
          //============================
          // Invalid setting
          //===========================
          else
          {
            Console.WriteLine("Invalid parameter specified");
            return 1;
          } // END ... If any of the settings match


        } // END ... For each arguement


        //======================================
        // Make sure we have people to send to
        //=======================================
        if (to.Length == 0)
        {
          Console.WriteLine("No to specified");
          return 1;
        } // END ... If there are no people to send to

        // Seperate list
        Char[] sep = { ',', ';',  '|' };

        // Split out the to field for multiple to
        string[] toList = to.Split(sep);

        // Mail message
        System.Net.Mail.MailMessage message = new System.Net.Mail.MailMessage();

        // Add all the To people to the list
        for ( int i = 0; i < toList.Length; ++i )
          message.To.Add(toList[i]);

        // Set the subect
        message.Subject = subject;

        // Set the from 
        message.From = new System.Net.Mail.MailAddress(from);

        // Set the boidy
        message.Body = body;

        //======================================
        // Check if any attachments are needed
        //======================================
        if (attachments.Length > 0)
        {
          // Split out hte attachments since ther can be more then one
          string[] attachmentList = attachments.Split(sep);

          // Add all attachmenst to the mail message
          for (int i = 0; i < attachmentList.Length; ++i)
            message.Attachments.Add(new Attachment(attachmentList[i]));

        } // END ... If attachments are present


        // Create SMTP client using the server setting
        System.Net.Mail.SmtpClient smtp = new System.Net.Mail.SmtpClient(server);

        // Set Enable SSL setting
        smtp.EnableSsl = enableSSL;

        // Set the port
        smtp.Port = port;

        //====================================================================
        // Only set the credentials if a user name and password is specified.
        //====================================================================
        if (user.Length > 0 && password.Length > 0)
        {
          smtp.Credentials = new System.Net.NetworkCredential(user, password);
        } // END ... If the user and password are specified

        // Set the delivery method to network
        smtp.DeliveryMethod = System.Net.Mail.SmtpDeliveryMethod.Network;

        // Send the message
        smtp.Send(message);

        return 0;
      }
      catch (Exception ex)
      {
        // Dump error message
        Console.WriteLine("Sending to message failed : " + ex.Message);
        return 1;

      } //END ... Catch

    } // main()

  } // 
}

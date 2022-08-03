using System;
using System.IO;
using System.Xml;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Linq;
using System.Text;
using System.Windows.Forms;

using Viatronix.Utilities.GenerateManifest.Base;

namespace Viatronix.Utilities.SelectiveManifestGenerator
{
  /// <summary>
  /// The main form for the application.
  /// </summary>
  public partial class MainForm : Form
  {

    #region fields

    /// <summary>
    /// The root node.
    /// </summary>
    private TreeNode m_rootNode = null;

    #endregion

    #region construction

    /// <summary>
    /// Constructor.
    /// </summary>
    public MainForm()
    {
      InitializeComponent();
    } // MainForm()

    #endregion

    #region methods

    /// <summary>
    /// Performs the directory load and initializes the root node.
    /// </summary>
    /// <param name="directory">The root directory to load.</param>
    private void PerformDirectoryLoad(string directory)
    {
      try
      {
        if (directory.EndsWith("\\"))
        {
          // Trim the seperators on the end to get the correct file name.
          directory = directory.TrimEnd('\\');
        }

        m_directoriesTreeView.Nodes.Clear();

        m_rootNode = new TreeNode();
        PopulateDirectories(directory, m_rootNode);

        m_rootNode.ExpandAll();
        m_directoriesTreeView.Nodes.Add(m_rootNode);
      }
      catch (Exception)
      {
        // If the load failed, reinitialize key pieces to ensure proper handling of the failed load.
        m_rootNode = null;
        m_directoriesTreeView.Nodes.Clear();

        throw;
      }
    } // PerformDirectoryLoad( directory )


    /// <summary>
    /// Populates the directories tree.
    /// </summary>
    /// <param name="directory">The directory to show in the tree view.</param>
    /// <param name="node">The node to correspond to the provided directory.</param>
    private void PopulateDirectories(string directory, TreeNode node)
    {
      node.Text = Path.GetFileName(directory);
      node.Checked = true;
      node.Tag = directory;
      node.ToolTipText = string.Format("{0}: {1}", directory, node.Checked ? "Selected for inclusion in output results." : "Excluded from inclusion in output results.");

      foreach (string childDirectory in Directory.GetDirectories(directory))
      {
        TreeNode child = new TreeNode();
        node.Nodes.Add(child);

        PopulateDirectories(childDirectory, child);
      }
    } // PopulateDirectories( directory )


    /// <summary>
    /// Updates the check state of the specified node to that of affected node.  Function is called recursively to update the descendents.
    /// </summary>
    /// <param name="node">The current node we're pointing to.</param>
    /// <param name="checkChangedNode">The node that the user changed state on.</param>
    private void UpdateCheckStates(TreeNode node, bool rootCheckState)
    {
      node.Checked = rootCheckState;
      node.ToolTipText = string.Format("{0}: {1}", node.Tag, node.Checked ? "Selected for inclusion in output results." : "Excluded from inclusion in output results.");

      foreach (TreeNode child in node.Nodes)
      {
        UpdateCheckStates(child, rootCheckState);
      }
    } // UpdateCheckStates( node )


    /// <summary>
    /// Retrieves the list of directories that we are going to output.
    /// </summary>
    /// <param name="node">The node we are examining.</param>
    /// <param name="output">The list of output directories.</param>
    private void GetDirectoriesForOutput(TreeNode node, List<string> output)
    {
      // If unchecked, all children are unchecked.
      if (node.Checked)
      {
        output.Add(node.Tag.ToString());

        foreach (TreeNode child in node.Nodes)
        {
          GetDirectoriesForOutput(child, output);
        }
      }
    } // GetDirectoriesForOutput( node, output )


    /// <summary>
    /// Writes out the manifest to the desired file.
    /// </summary>
    /// <param name="outputFile"></param>
    /// <param name="node"></param>
    private void WriteManifest(string outputFile, TreeNode node)
    {
      XmlDocument doc = new XmlDocument();
      XmlNode docElement = doc.CreateElement(node.Text);

      WriteVersionInformation(doc, node, docElement);

      doc.AppendChild(docElement);
      doc.Save(outputFile);
    } // WriteManifest( node )


    /// <summary>
    /// Writes the version information into the xml node.
    /// </summary>
    /// <param name="doc">The xml document.</param>
    /// <param name="currentTreeNode">The current tree node we're at.</param>
    /// <param name="currentXmlNode">The xml node corresponding to the tree.</param>
    private void WriteVersionInformation(XmlDocument doc, TreeNode currentTreeNode, XmlNode currentXmlNode)
    {
      string directory = currentTreeNode.Tag.ToString();
      if (Directory.Exists(directory))
      {
        foreach (TreeNode node in currentTreeNode.Nodes)
        {
          if (node.Checked)
          {
            try
            {
              XmlNode childNode = doc.CreateElement(node.Text);

              WriteVersionInformation(doc, node, childNode);

              currentXmlNode.AppendChild(childNode);
            }
            catch (Exception e)
            {
              MiniLogger.Error(string.Format("Failed to add child node to the xml document.  Directory name: {0}  [ERROR=\"{1}\"]", node.Text, e.Message), "MainForm", "WriteVersionInformation");
              throw;
            }
          }
        }

        string[] files = Directory.GetFiles(directory);
        if (files.Length > 0)
        {
          XmlNode filesNode = doc.CreateElement("files");

          for (int i = 0; i < files.Length; ++i)
          {
            Tuple<string, string> version = VersionInfo.RetrieveVersionInformation(files[i]);

            XmlNode fileNode = doc.CreateElement("file");

            XmlAttribute name = doc.CreateAttribute("name");
            name.Value = Path.GetFileName(files[i]);
            fileNode.Attributes.Append(name);

            XmlAttribute fileVersion = doc.CreateAttribute("fileversion");
            fileVersion.Value = version.Item1;
            fileNode.Attributes.Append(fileVersion);

            XmlAttribute productVersion = doc.CreateAttribute("productversion");
            productVersion.Value = version.Item2;
            fileNode.Attributes.Append(productVersion);

            XmlAttribute dateModified = doc.CreateAttribute("datemodified");
            dateModified.Value = File.GetLastWriteTime(files[i]).ToString("o");
            fileNode.Attributes.Append(dateModified);

            filesNode.AppendChild(fileNode);
          }

          currentXmlNode.AppendChild(filesNode);
        }
      }
    } // WriteVersionInformation( doc, currentTreeNode, currentXmlNode )

    #endregion

    #region event handlers

    /// <summary>
    /// Handles the directory navigator's click event.
    /// </summary>
    /// <param name="sender">The origin of the event.</param>
    /// <param name="args">Arguments to take into account.</param>
    private void OnDirectoryNavigatorClickHandler(object sender, EventArgs args)
    {
      FolderBrowserDialog dialog = new FolderBrowserDialog();
      if (dialog.ShowDialog(this) == DialogResult.OK)
      {
        m_directoryTextBox.Text = dialog.SelectedPath;

        try
        {
          m_directoriesTreeView.BeforeCheck -= new TreeViewCancelEventHandler(OnDirectoriesTreeViewBeforeCheckHandler);
          m_directoriesTreeView.AfterCheck -= new TreeViewEventHandler(OnDirectoriesTreeViewAfterCheckHandler);

          PerformDirectoryLoad(m_directoryTextBox.Text);
        }
        catch (Exception e)
        {
          MiniLogger.Error(string.Format("An error occured during the load operation.  [ERROR={0}]", e.Message), "MainForm", "OnDirectoryNavigatorClickHandler");
          MessageBox.Show(Messages.GetMessage("VX011"), Messages.GetMessage("VX012"), MessageBoxButtons.OK, MessageBoxIcon.Error);
        }
        finally
        {
          m_directoriesTreeView.BeforeCheck += new TreeViewCancelEventHandler(OnDirectoriesTreeViewBeforeCheckHandler);
          m_directoriesTreeView.AfterCheck += new TreeViewEventHandler(OnDirectoriesTreeViewAfterCheckHandler);
        }
      }
    } // OnDirectoryNavigatorClickHandler( sender, args )


    /// <summary>
    /// Handles the load button's click event.
    /// </summary>
    /// <param name="sender">The origin of the event.</param>
    /// <param name="args">Arguments to take into account.</param>
    private void OnLoadButtonClickHandler(object sender, EventArgs args)
    {
      if (m_directoryTextBox.Text != string.Empty && Directory.Exists(m_directoryTextBox.Text))
      {
        try
        {
          m_directoriesTreeView.BeforeCheck -= new TreeViewCancelEventHandler(OnDirectoriesTreeViewBeforeCheckHandler);
          m_directoriesTreeView.AfterCheck -= new TreeViewEventHandler(OnDirectoriesTreeViewAfterCheckHandler);

          PerformDirectoryLoad(m_directoryTextBox.Text);
        }
        catch (Exception e)
        {
          MiniLogger.Error(string.Format("An error occured during the load operation.  [ERROR={0}]", e.Message), "MainForm", "OnLoadButtonClickHandler");
          MessageBox.Show(Messages.GetMessage("VX011"), Messages.GetMessage("VX012"), MessageBoxButtons.OK, MessageBoxIcon.Error);
        }
        finally
        {
          m_directoriesTreeView.BeforeCheck += new TreeViewCancelEventHandler(OnDirectoriesTreeViewBeforeCheckHandler);
          m_directoriesTreeView.AfterCheck += new TreeViewEventHandler(OnDirectoriesTreeViewAfterCheckHandler);
        }
      }
      else
      {
        m_rootNode = null;
        m_directoriesTreeView.Nodes.Clear();

        MessageBox.Show(this, Messages.GetMessage("VX005"), Messages.GetMessage("VX006"), MessageBoxButtons.OK, MessageBoxIcon.Warning);
      }
    } // OnLoadButtonClickHandler( sender, args )


    /// <summary>
    /// Handles the directories treeview's BeforeCheck event.
    /// </summary>
    /// <param name="sender">The origin of the event.</param>
    /// <param name="args">Arguments to take into account.</param>
    private void OnDirectoriesTreeViewBeforeCheckHandler(object sender, TreeViewCancelEventArgs args)
    {
      if (args.Action != TreeViewAction.Unknown)
      {
        // Go up the tree to find if any parent was unchecked.
        TreeNode parent = args.Node.Parent;
        while (parent != null)
        {
          if (!parent.Checked)
          {
            args.Cancel = true;
            return;
          }

          parent = parent.Parent;
        }
      }
    } // OnDirectoriesTreeViewBeforeCheckHandler( sender, args )


    /// <summary>
    /// Handles the directories treeview's AfterCheck event.
    /// </summary>
    /// <param name="sender">The origin of the event.</param>
    /// <param name="args">Arguments to take into account.</param>
    private void OnDirectoriesTreeViewAfterCheckHandler(object sender, TreeViewEventArgs args)
    {
      try
      {
        m_directoriesTreeView.BeforeCheck -= new TreeViewCancelEventHandler(OnDirectoriesTreeViewBeforeCheckHandler);
        m_directoriesTreeView.AfterCheck -= new TreeViewEventHandler(OnDirectoriesTreeViewAfterCheckHandler);

        if (args.Action != TreeViewAction.Unknown)
        {
          UpdateCheckStates(args.Node, args.Node.Checked);
        }
      }
      finally
      {
        Invalidate();

        m_directoriesTreeView.BeforeCheck += new TreeViewCancelEventHandler(OnDirectoriesTreeViewBeforeCheckHandler);
        m_directoriesTreeView.AfterCheck += new TreeViewEventHandler(OnDirectoriesTreeViewAfterCheckHandler);
      }
    } // OnDirectoriesTreeViewAfterCheckHandler( sender, args )


    /// <summary>
    /// Handles the generate button's click event.
    /// </summary>
    /// <param name="sender">The origin of the event.</param>
    /// <param name="args">Arguments to take into account.</param>
    private void OnGenerateClickHandler(object sender, EventArgs args)
    {
      if (m_rootNode != null)
      {
        List<string> output = new List<string>();
        GetDirectoriesForOutput(m_rootNode, output);

        if (m_rootNode.Checked && output.Count > 0)
        {
          DirectoriesConfirmationDialog dialog = new DirectoriesConfirmationDialog(output.ToArray());
          if (dialog.ShowDialog() == DialogResult.OK)
          {
            SaveFileDialog saveDialog = new SaveFileDialog();
            saveDialog.Filter = "Xml Files (*.xml)|*.xml";

            if (saveDialog.ShowDialog() == DialogResult.OK)
            {
              try
              {
                WriteManifest(saveDialog.FileName, m_rootNode);

                MiniLogger.Information("Successfully saved manifest information for selected directories.", "MainForm", "OnGenerateClickHandler");
                MessageBox.Show(this, Messages.GetMessage("VX013"), Messages.GetMessage("VX014"), MessageBoxButtons.OK, MessageBoxIcon.Information);
              }
              catch (Exception e)
              {
                MiniLogger.Error(string.Format("Failed to save manifest information for selected directories.  [ERROR=\"{0}\"]", e.Message), "MainForm", "OnGenerateClickHandler");
                MessageBox.Show(this, Messages.GetMessage("VX015"), Messages.GetMessage("VX016"), MessageBoxButtons.OK, MessageBoxIcon.Error);
              }
            }
          }
        }
        else
        {
          MessageBox.Show(this, Messages.GetMessage("VX009"), Messages.GetMessage("VX010"), MessageBoxButtons.OK, MessageBoxIcon.Warning);
        }
      }
      else
      {
        MessageBox.Show(this, Messages.GetMessage("VX007"), Messages.GetMessage("VX008"), MessageBoxButtons.OK, MessageBoxIcon.Warning);
      }
    } // OnGenerateClickHandler( sender, args )


    /// <summary>
    /// Handles the select all button's click event.
    /// </summary>
    /// <param name="sender">The origin of the event.</param>
    /// <param name="args">Arguments to take into account.</param>
    private void OnSelectAllClickHandler(object sender, EventArgs args)
    {
      try
      {
        m_directoriesTreeView.BeforeCheck -= new TreeViewCancelEventHandler(OnDirectoriesTreeViewBeforeCheckHandler);
        m_directoriesTreeView.AfterCheck -= new TreeViewEventHandler(OnDirectoriesTreeViewAfterCheckHandler);

        if (m_rootNode != null)
        {
          UpdateCheckStates(m_rootNode, true);
        }
        else
        {
          MessageBox.Show(this, Messages.GetMessage("VX007"), Messages.GetMessage("VX008"), MessageBoxButtons.OK, MessageBoxIcon.Warning);
        }
      }
      finally
      {
        m_directoriesTreeView.BeforeCheck += new TreeViewCancelEventHandler(OnDirectoriesTreeViewBeforeCheckHandler);
        m_directoriesTreeView.AfterCheck += new TreeViewEventHandler(OnDirectoriesTreeViewAfterCheckHandler);
      }
    } // OnSelectAllClickHandler( sender, args )


    /// <summary>
    /// Handles the select none button's click event.
    /// </summary>
    /// <param name="sender">The origin of the event.</param>
    /// <param name="args">Arguments to take into account.</param>
    private void OnSelectNoneClickHandler(object sender, EventArgs args)
    {
      try
      {
        m_directoriesTreeView.BeforeCheck -= new TreeViewCancelEventHandler(OnDirectoriesTreeViewBeforeCheckHandler);
        m_directoriesTreeView.AfterCheck -= new TreeViewEventHandler(OnDirectoriesTreeViewAfterCheckHandler);

        if (m_rootNode != null)
        {
          UpdateCheckStates(m_rootNode, false);
        }
        else
        {
          MessageBox.Show(this, Messages.GetMessage("VX007"), Messages.GetMessage("VX008"), MessageBoxButtons.OK, MessageBoxIcon.Warning);
        }
      }
      finally
      {
        m_directoriesTreeView.BeforeCheck += new TreeViewCancelEventHandler(OnDirectoriesTreeViewBeforeCheckHandler);
        m_directoriesTreeView.AfterCheck += new TreeViewEventHandler(OnDirectoriesTreeViewAfterCheckHandler);
      }
    } // OnSelectNoneClickHandler( sender, args )

    #endregion

  } // class MainForm
} // namespace Viatronix.Utilities.SelectiveManifestGenerator

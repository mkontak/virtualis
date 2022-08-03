using System;
using System.Collections;
using System.Management;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Text;
using System.Windows.Forms;

// Source code designed by Alireza shirazi
// www.ShiraziOnline.net
// year 2007
// This source code is absolutely FREE ! Do whatever you want to do with it ;)



namespace GetHardwareInfo
{
    public partial class GetHardwareInfo : Form
    {
        public GetHardwareInfo()
        {
            InitializeComponent();
            cmbxOption.SelectedItem = "Win32_Processor";

        }

        private void InsertInfo(string Key, ref ListView m_listView, bool DontInsertNull)
        {
            m_listView.Items.Clear();

            ManagementObjectSearcher searcher = new ManagementObjectSearcher("select * from " + Key);

            try
            {
                foreach (ManagementObject share in searcher.Get())
                {

                    ListViewGroup group;
                    try
                    {
                        group = m_listView.Groups.Add(share["Name"].ToString(), share["Name"].ToString());
                    }
                    catch
                    {
                        group = m_listView.Groups.Add(share.ToString(), share.ToString());
                    }

             
                    if (share.Properties.Count <= 0)
                    {
                        MessageBox.Show("No Information Available", "No Info", MessageBoxButtons.OK, MessageBoxIcon.Information);
                        return;
                    }

                    foreach (PropertyData property in share.Properties)
                    {

                        ListViewItem item = new ListViewItem(group);
                        if (m_listView.Items.Count % 2 != 0)
                        {
                          item.BackColor = Color.White;
                          //item.ForeColor = Color.Black;
                        }
                        else
                        {
                          item.BackColor = Color.LightGray;
                          //item.ForeColor = Color.White;
                        }

                        item.Text = property.Name;

                        if (property.Value != null && property.Value.ToString() != "")
                        {
                            switch (property.Value.GetType().ToString())
                            {
                                case "System.String[]":
                                    string[] str = (string[])property.Value;

                                    string str2 = "";
                                    foreach (string st in str)
                                        str2 += st + " ";

                                    item.SubItems.Add(str2);

                                    break;
                                case "System.UInt16[]":
                                    ushort[] shortData = (ushort[])property.Value;


                                    string tstr2 = "";
                                    foreach (ushort st in shortData)
                                        tstr2 += st.ToString() + " ";

                                    item.SubItems.Add(tstr2);

                                    break;

                                default:
                                    item.SubItems.Add(property.Value.ToString());
                                    break;
                            }
                        }
                        else
                        {
                            if (!DontInsertNull)
                                item.SubItems.Add("No Information available");
                            else
                                continue;
                        }

                        m_listView.Items.Add(item);
                    }
                }
            }


            catch (Exception exp)
            {
                MessageBox.Show("can't get data because of the followeing error \n" + exp.Message, "Error", MessageBoxButtons.OK, MessageBoxIcon.Information);
            }


        }

        private void RemoveNullValue(ref ListView m_listView)
        {
            foreach (ListViewItem item in m_listView.Items)
                if (item.SubItems[1].Text == "No Information available")
                    item.Remove();
        }


        #region Control events ...

        private void cmbxNetwork_SelectedIndexChanged(object sender, EventArgs e)
        {
            InsertInfo(cmbxNetwork.SelectedItem.ToString(), ref m_listViewNetwork, chkNetwork.Checked);
        }

        private void cmbxSystemInfo_SelectedIndexChanged(object sender, EventArgs e)
        {
            InsertInfo(cmbxSystemInfo.SelectedItem.ToString(), ref m_listViewSystemInfo, chkSystemInfo.Checked);
        }

        private void cmbxUtility_SelectedIndexChanged(object sender, EventArgs e)
        {
            InsertInfo(cmbxUtility.SelectedItem.ToString(), ref m_listViewUtility, chkUtility.Checked);
        }

        private void cmbxUserAccount_SelectedIndexChanged(object sender, EventArgs e)
        {
            InsertInfo(cmbxUserAccount.SelectedItem.ToString(), ref m_listViewUserAccount, chkUserAccount.Checked);
        }

        private void cmbxStorage_SelectedIndexChanged(object sender, EventArgs e)
        {
            InsertInfo(cmbxStorage.SelectedItem.ToString(), ref m_listViewStorage, chkDataStorage.Checked);
        }

        private void cmbxDeveloper_SelectedIndexChanged(object sender, EventArgs e)
        {
            InsertInfo(cmbxDeveloper.SelectedItem.ToString(), ref m_listViewDeveloper, chkDeveloper.Checked);
        }

        private void cmbxMemory_SelectedIndexChanged(object sender, EventArgs e)
        {
            InsertInfo(cmbxMemory.SelectedItem.ToString(), ref m_listViewMemory, chkMemory.Checked);
        }

        private void chkHardware_CheckedChanged(object sender, EventArgs e)
        {
            if (chkHardware.Checked)
                RemoveNullValue(ref m_listViewDisplayHardware);
            else
                InsertInfo(cmbxOption.SelectedItem.ToString(), ref m_listViewDisplayHardware, chkHardware.Checked);
        }

        private void cmbxOption_SelectedIndexChanged(object sender, EventArgs e)
        {
            InsertInfo(cmbxOption.SelectedItem.ToString(), ref m_listViewDisplayHardware, chkHardware.Checked);
        }

        private void chkDataStorage_CheckedChanged(object sender, EventArgs e)
        {
            if (chkDataStorage.Checked)
                RemoveNullValue(ref m_listViewStorage);
            else
                InsertInfo(cmbxStorage.SelectedItem.ToString(), ref m_listViewStorage, chkDataStorage.Checked);
        }

        private void chkMemory_CheckedChanged(object sender, EventArgs e)
        {
            if (chkMemory.Checked)
                RemoveNullValue(ref m_listViewMemory);
            else
                InsertInfo(cmbxMemory.SelectedItem.ToString(), ref m_listViewStorage, false);
        }

        private void chkSystemInfo_CheckedChanged(object sender, EventArgs e)
        {
            if (chkSystemInfo.Checked)
                RemoveNullValue(ref m_listViewSystemInfo);
            else
                InsertInfo(cmbxSystemInfo.SelectedItem.ToString(), ref m_listViewSystemInfo, false);
        }

        private void chkNetwork_CheckedChanged(object sender, EventArgs e)
        {
            if (chkNetwork.Checked)
                RemoveNullValue(ref m_listViewNetwork);
            else
                InsertInfo(cmbxNetwork.SelectedItem.ToString(), ref m_listViewNetwork, false);
        }

        private void chkUserAccount_CheckedChanged(object sender, EventArgs e)
        {
            if (chkUserAccount.Checked)
                RemoveNullValue(ref m_listViewUserAccount);
            else
                InsertInfo(cmbxUserAccount.SelectedItem.ToString(), ref m_listViewUserAccount, false);
        }

        private void chkDeveloper_CheckedChanged(object sender, EventArgs e)
        {
            if (chkDeveloper.Checked)
                RemoveNullValue(ref m_listViewDeveloper);
            else
                InsertInfo(cmbxDeveloper.SelectedItem.ToString(), ref m_listViewDeveloper, false);
        }

        private void chkUtility_CheckedChanged(object sender, EventArgs e)
        {
            if (chkUtility.Checked)
                RemoveNullValue(ref m_listViewUtility);
            else
                InsertInfo(cmbxUtility.SelectedItem.ToString(), ref m_listViewUtility, false);
        }

        #endregion

       
    }
}
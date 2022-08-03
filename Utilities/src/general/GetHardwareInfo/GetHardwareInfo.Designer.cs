namespace GetHardwareInfo
{
    partial class GetHardwareInfo
    {
        /// <summary>
        /// Required designer variable.
        /// </summary>
        private System.ComponentModel.IContainer components = null;

        /// <summary>
        /// Clean up any resources being used.
        /// </summary>
        /// <param name="disposing">true if managed resources should be disposed; otherwise, false.</param>
        protected override void Dispose(bool disposing)
        {
            if (disposing && (components != null))
            {
                components.Dispose();
            }
            base.Dispose(disposing);
        }

        #region Windows Form Designer generated code

        /// <summary>
        /// Required method for Designer support - do not modify
        /// the contents of this method with the code editor.
        /// </summary>
        private void InitializeComponent()
        {
          this.columnHeader5 = new System.Windows.Forms.ColumnHeader();
          this.columnHeader6 = new System.Windows.Forms.ColumnHeader();
          this.tabUtility = new System.Windows.Forms.TabPage();
          this.chkUtility = new System.Windows.Forms.CheckBox();
          this.cmbxUtility = new System.Windows.Forms.ComboBox();
          this.m_listViewUtility = new System.Windows.Forms.ListView();
          this.columnHeader9 = new System.Windows.Forms.ColumnHeader();
          this.columnHeader10 = new System.Windows.Forms.ColumnHeader();
          this.tabDeveloper = new System.Windows.Forms.TabPage();
          this.chkDeveloper = new System.Windows.Forms.CheckBox();
          this.cmbxDeveloper = new System.Windows.Forms.ComboBox();
          this.m_listViewDeveloper = new System.Windows.Forms.ListView();
          this.columnHeader15 = new System.Windows.Forms.ColumnHeader();
          this.columnHeader16 = new System.Windows.Forms.ColumnHeader();
          this.tabUserAccount = new System.Windows.Forms.TabPage();
          this.chkUserAccount = new System.Windows.Forms.CheckBox();
          this.cmbxUserAccount = new System.Windows.Forms.ComboBox();
          this.m_listViewUserAccount = new System.Windows.Forms.ListView();
          this.columnHeader13 = new System.Windows.Forms.ColumnHeader();
          this.columnHeader14 = new System.Windows.Forms.ColumnHeader();
          this.tabNetwork = new System.Windows.Forms.TabPage();
          this.chkNetwork = new System.Windows.Forms.CheckBox();
          this.cmbxNetwork = new System.Windows.Forms.ComboBox();
          this.m_listViewNetwork = new System.Windows.Forms.ListView();
          this.columnHeader3 = new System.Windows.Forms.ColumnHeader();
          this.columnHeader4 = new System.Windows.Forms.ColumnHeader();
          this.tabSystemInfo = new System.Windows.Forms.TabPage();
          this.chkSystemInfo = new System.Windows.Forms.CheckBox();
          this.cmbxSystemInfo = new System.Windows.Forms.ComboBox();
          this.m_listViewSystemInfo = new System.Windows.Forms.ListView();
          this.columnHeader7 = new System.Windows.Forms.ColumnHeader();
          this.columnHeader8 = new System.Windows.Forms.ColumnHeader();
          this.tabMemory = new System.Windows.Forms.TabPage();
          this.chkMemory = new System.Windows.Forms.CheckBox();
          this.cmbxMemory = new System.Windows.Forms.ComboBox();
          this.m_listViewMemory = new System.Windows.Forms.ListView();
          this.columnHeader17 = new System.Windows.Forms.ColumnHeader();
          this.columnHeader18 = new System.Windows.Forms.ColumnHeader();
          this.tabDataStorage = new System.Windows.Forms.TabPage();
          this.chkDataStorage = new System.Windows.Forms.CheckBox();
          this.cmbxStorage = new System.Windows.Forms.ComboBox();
          this.m_listViewStorage = new System.Windows.Forms.ListView();
          this.columnHeader11 = new System.Windows.Forms.ColumnHeader();
          this.columnHeader12 = new System.Windows.Forms.ColumnHeader();
          this.tabHardware = new System.Windows.Forms.TabPage();
          this.chkHardware = new System.Windows.Forms.CheckBox();
          this.cmbxOption = new System.Windows.Forms.ComboBox();
          this.m_listViewDisplayHardware = new System.Windows.Forms.ListView();
          this.columnHeader1 = new System.Windows.Forms.ColumnHeader();
          this.columnHeader2 = new System.Windows.Forms.ColumnHeader();
          this.MainTabControl = new System.Windows.Forms.TabControl();
          this.tabUtility.SuspendLayout();
          this.tabDeveloper.SuspendLayout();
          this.tabUserAccount.SuspendLayout();
          this.tabNetwork.SuspendLayout();
          this.tabSystemInfo.SuspendLayout();
          this.tabMemory.SuspendLayout();
          this.tabDataStorage.SuspendLayout();
          this.tabHardware.SuspendLayout();
          this.MainTabControl.SuspendLayout();
          this.SuspendLayout();
          // 
          // columnHeader5
          // 
          this.columnHeader5.Text = "Name";
          this.columnHeader5.Width = 72;
          // 
          // columnHeader6
          // 
          this.columnHeader6.Text = "Value";
          this.columnHeader6.Width = 503;
          // 
          // tabUtility
          // 
          this.tabUtility.Controls.Add(this.chkUtility);
          this.tabUtility.Controls.Add(this.cmbxUtility);
          this.tabUtility.Controls.Add(this.m_listViewUtility);
          this.tabUtility.Location = new System.Drawing.Point(4, 22);
          this.tabUtility.Name = "tabUtility";
          this.tabUtility.Padding = new System.Windows.Forms.Padding(3);
          this.tabUtility.Size = new System.Drawing.Size(640, 431);
          this.tabUtility.TabIndex = 3;
          this.tabUtility.Text = "Utility";
          this.tabUtility.UseVisualStyleBackColor = true;
          // 
          // chkUtility
          // 
          this.chkUtility.AutoSize = true;
          this.chkUtility.Checked = true;
          this.chkUtility.CheckState = System.Windows.Forms.CheckState.Checked;
          this.chkUtility.Location = new System.Drawing.Point(16, 18);
          this.chkUtility.Name = "chkUtility";
          this.chkUtility.Size = new System.Drawing.Size(162, 17);
          this.chkUtility.TabIndex = 5;
          this.chkUtility.Text = "Dont Display null Value items";
          this.chkUtility.UseVisualStyleBackColor = true;
          this.chkUtility.CheckedChanged += new System.EventHandler(this.chkUtility_CheckedChanged);
          // 
          // cmbxUtility
          // 
          this.cmbxUtility.DropDownStyle = System.Windows.Forms.ComboBoxStyle.DropDownList;
          this.cmbxUtility.FormattingEnabled = true;
          this.cmbxUtility.Items.AddRange(new object[] {
            "Win32_1394Controller",
            "Win32_1394ControllerDevice",
            "Win32_Account",
            "Win32_AccountSID",
            "Win32_ACE",
            "Win32_ActionCheck",
            "Win32_AllocatedResource",
            "Win32_ApplicationCommandLine",
            "Win32_ApplicationService",
            "Win32_AssociatedBattery",
            "Win32_AssociatedProcessorMemory",
            "Win32_BaseBoard",
            "Win32_BaseService",
            "Win32_Battery",
            "Win32_Binary",
            "Win32_BindImageAction",
            "Win32_BIOS",
            "Win32_BootConfiguration",
            "Win32_Bus",
            "Win32_CacheMemory",
            "Win32_CDROMDrive",
            "Win32_CheckCheck",
            "Win32_CIMLogicalDeviceCIMDataFile",
            "Win32_ClassicCOMApplicationClasses",
            "Win32_ClassicCOMClass",
            "Win32_ClassicCOMClassSetting",
            "Win32_ClassicCOMClassSettings",
            "Win32_ClassInfoAction",
            "Win32_ClientApplicationSetting",
            "Win32_CodecFile",
            "Win32_COMApplication",
            "Win32_COMApplicationClasses",
            "Win32_COMApplicationSettings",
            "Win32_COMClass",
            "Win32_ComClassAutoEmulator",
            "Win32_ComClassEmulator",
            "Win32_CommandLineAccess",
            "Win32_ComponentCategory",
            "Win32_ComputerSystem",
            "Win32_ComputerSystemProcessor",
            "Win32_ComputerSystemProduct",
            "Win32_COMSetting",
            "Win32_Condition",
            "Win32_CreateFolderAction",
            "Win32_CurrentProbe",
            "Win32_DCOMApplication",
            "Win32_DCOMApplicationAccessAllowedSetting",
            "Win32_DCOMApplicationLaunchAllowedSetting",
            "Win32_DCOMApplicationSetting",
            "Win32_DependentService",
            "Win32_Desktop",
            "Win32_DesktopMonitor",
            "Win32_DeviceBus",
            "Win32_DeviceMemoryAddress",
            "Win32_DeviceSettings",
            "Win32_Directory",
            "Win32_DirectorySpecification",
            "Win32_DiskDrive",
            "Win32_DiskDriveToDiskPartition",
            "Win32_DiskPartition",
            "Win32_DisplayConfiguration",
            "Win32_DisplayControllerConfiguration",
            "Win32_DMAChannel",
            "Win32_DriverVXD",
            "Win32_DuplicateFileAction",
            "Win32_Environment",
            "Win32_EnvironmentSpecification",
            "Win32_ExtensionInfoAction",
            "Win32_Fan",
            "Win32_FileSpecification",
            "Win32_FloppyController",
            "Win32_FloppyDrive",
            "Win32_FontInfoAction",
            "Win32_Group",
            "Win32_GroupUser",
            "Win32_HeatPipe",
            "Win32_IDEController",
            "Win32_IDEControllerDevice",
            "Win32_ImplementedCategory",
            "Win32_InfraredDevice",
            "Win32_IniFileSpecification",
            "Win32_InstalledSoftwareElement",
            "Win32_IRQResource",
            "Win32_Keyboard",
            "Win32_LaunchCondition",
            "Win32_LoadOrderGroup",
            "Win32_LoadOrderGroupServiceDependencies",
            "Win32_LoadOrderGroupServiceMembers",
            "Win32_LogicalDisk",
            "Win32_LogicalDiskRootDirectory",
            "Win32_LogicalDiskToPartition",
            "Win32_LogicalFileAccess",
            "Win32_LogicalFileAuditing",
            "Win32_LogicalFileGroup",
            "Win32_LogicalFileOwner",
            "Win32_LogicalFileSecuritySetting",
            "Win32_LogicalMemoryConfiguration",
            "Win32_LogicalProgramGroup",
            "Win32_LogicalProgramGroupDirectory",
            "Win32_LogicalProgramGroupItem",
            "Win32_LogicalProgramGroupItemDataFile",
            "Win32_LogicalShareAccess",
            "Win32_LogicalShareAuditing",
            "Win32_LogicalShareSecuritySetting",
            "Win32_ManagedSystemElementResource",
            "Win32_MemoryArray",
            "Win32_MemoryArrayLocation",
            "Win32_MemoryDevice",
            "Win32_MemoryDeviceArray",
            "Win32_MemoryDeviceLocation",
            "Win32_MethodParameterClass",
            "Win32_MIMEInfoAction",
            "Win32_MotherboardDevice",
            "Win32_MoveFileAction",
            "Win32_MSIResource",
            "Win32_NetworkAdapter",
            "Win32_NetworkAdapterConfiguration",
            "Win32_NetworkAdapterSetting",
            "Win32_NetworkClient",
            "Win32_NetworkConnection",
            "Win32_NetworkLoginProfile",
            "Win32_NetworkProtocol",
            "Win32_NTEventlogFile",
            "Win32_NTLogEvent",
            "Win32_NTLogEventComputer",
            "Win32_NTLogEventLog",
            "Win32_NTLogEventUser",
            "Win32_ODBCAttribute",
            "Win32_ODBCDataSourceAttribute",
            "Win32_ODBCDataSourceSpecification",
            "Win32_ODBCDriverAttribute",
            "Win32_ODBCDriverSoftwareElement",
            "Win32_ODBCDriverSpecification",
            "Win32_ODBCSourceAttribute",
            "Win32_ODBCTranslatorSpecification",
            "Win32_OnBoardDevice",
            "Win32_OperatingSystem",
            "Win32_OperatingSystemQFE",
            "Win32_OSRecoveryConfiguration",
            "Win32_PageFile",
            "Win32_PageFileElementSetting",
            "Win32_PageFileSetting",
            "Win32_PageFileUsage",
            "Win32_ParallelPort",
            "Win32_Patch",
            "Win32_PatchFile",
            "Win32_PatchPackage",
            "Win32_PCMCIAController",
            "Win32_Perf",
            "Win32_PerfRawData",
            "Win32_PerfRawData_ASP_ActiveServerPages",
            "Win32_PerfRawData_ASPNET_114322_ASPNETAppsv114322",
            "Win32_PerfRawData_ASPNET_114322_ASPNETv114322",
            "Win32_PerfRawData_ASPNET_ASPNET",
            "Win32_PerfRawData_ASPNET_ASPNETApplications",
            "Win32_PerfRawData_IAS_IASAccountingClients",
            "Win32_PerfRawData_IAS_IASAccountingServer",
            "Win32_PerfRawData_IAS_IASAuthenticationClients",
            "Win32_PerfRawData_IAS_IASAuthenticationServer",
            "Win32_PerfRawData_InetInfo_InternetInformationServicesGlobal",
            "Win32_PerfRawData_MSDTC_DistributedTransactionCoordinator",
            "Win32_PerfRawData_MSFTPSVC_FTPService",
            "Win32_PerfRawData_MSSQLSERVER_SQLServerAccessMethods",
            "Win32_PerfRawData_MSSQLSERVER_SQLServerBackupDevice",
            "Win32_PerfRawData_MSSQLSERVER_SQLServerBufferManager",
            "Win32_PerfRawData_MSSQLSERVER_SQLServerBufferPartition",
            "Win32_PerfRawData_MSSQLSERVER_SQLServerCacheManager",
            "Win32_PerfRawData_MSSQLSERVER_SQLServerDatabases",
            "Win32_PerfRawData_MSSQLSERVER_SQLServerGeneralStatistics",
            "Win32_PerfRawData_MSSQLSERVER_SQLServerLatches",
            "Win32_PerfRawData_MSSQLSERVER_SQLServerLocks",
            "Win32_PerfRawData_MSSQLSERVER_SQLServerMemoryManager",
            "Win32_PerfRawData_MSSQLSERVER_SQLServerReplicationAgents",
            "Win32_PerfRawData_MSSQLSERVER_SQLServerReplicationDist",
            "Win32_PerfRawData_MSSQLSERVER_SQLServerReplicationLogreader",
            "Win32_PerfRawData_MSSQLSERVER_SQLServerReplicationMerge",
            "Win32_PerfRawData_MSSQLSERVER_SQLServerReplicationSnapshot",
            "Win32_PerfRawData_MSSQLSERVER_SQLServerSQLStatistics",
            "Win32_PerfRawData_MSSQLSERVER_SQLServerUserSettable",
            "Win32_PerfRawData_NETFramework_NETCLRExceptions",
            "Win32_PerfRawData_NETFramework_NETCLRInterop",
            "Win32_PerfRawData_NETFramework_NETCLRJit",
            "Win32_PerfRawData_NETFramework_NETCLRLoading",
            "Win32_PerfRawData_NETFramework_NETCLRLocksAndThreads",
            "Win32_PerfRawData_NETFramework_NETCLRMemory",
            "Win32_PerfRawData_NETFramework_NETCLRRemoting",
            "Win32_PerfRawData_NETFramework_NETCLRSecurity",
            "Win32_PerfRawData_Outlook_Outlook",
            "Win32_PerfRawData_PerfDisk_PhysicalDisk",
            "Win32_PerfRawData_PerfNet_Browser",
            "Win32_PerfRawData_PerfNet_Redirector",
            "Win32_PerfRawData_PerfNet_Server",
            "Win32_PerfRawData_PerfNet_ServerWorkQueues",
            "Win32_PerfRawData_PerfOS_Cache",
            "Win32_PerfRawData_PerfOS_Memory",
            "Win32_PerfRawData_PerfOS_Objects",
            "Win32_PerfRawData_PerfOS_PagingFile",
            "Win32_PerfRawData_PerfOS_Processor",
            "Win32_PerfRawData_PerfOS_System",
            "Win32_PerfRawData_PerfProc_FullImage_Costly",
            "Win32_PerfRawData_PerfProc_Image_Costly",
            "Win32_PerfRawData_PerfProc_JobObject",
            "Win32_PerfRawData_PerfProc_JobObjectDetails",
            "Win32_PerfRawData_PerfProc_Process",
            "Win32_PerfRawData_PerfProc_ProcessAddressSpace_Costly",
            "Win32_PerfRawData_PerfProc_Thread",
            "Win32_PerfRawData_PerfProc_ThreadDetails_Costly",
            "Win32_PerfRawData_RemoteAccess_RASPort",
            "Win32_PerfRawData_RemoteAccess_RASTotal",
            "Win32_PerfRawData_RSVP_ACSPerRSVPService",
            "Win32_PerfRawData_Spooler_PrintQueue",
            "Win32_PerfRawData_TapiSrv_Telephony",
            "Win32_PerfRawData_Tcpip_ICMP",
            "Win32_PerfRawData_Tcpip_IP",
            "Win32_PerfRawData_Tcpip_NBTConnection",
            "Win32_PerfRawData_Tcpip_NetworkInterface",
            "Win32_PerfRawData_Tcpip_TCP",
            "Win32_PerfRawData_Tcpip_UDP",
            "Win32_PerfRawData_W3SVC_WebService",
            "Win32_PhysicalMemory",
            "Win32_PhysicalMemoryArray",
            "Win32_PhysicalMemoryLocation",
            "Win32_PNPAllocatedResource",
            "Win32_PnPDevice",
            "Win32_PnPEntity",
            "Win32_PointingDevice",
            "Win32_PortableBattery",
            "Win32_PortConnector",
            "Win32_PortResource",
            "Win32_POTSModem",
            "Win32_POTSModemToSerialPort",
            "Win32_PowerManagementEvent",
            "Win32_Printer",
            "Win32_PrinterConfiguration",
            "Win32_PrinterController",
            "Win32_PrinterDriverDll",
            "Win32_PrinterSetting",
            "Win32_PrinterShare",
            "Win32_PrintJob",
            "Win32_PrivilegesStatus",
            "Win32_Process",
            "Win32_Processor",
            "Win32_ProcessStartup",
            "Win32_Product",
            "Win32_ProductCheck",
            "Win32_ProductResource",
            "Win32_ProductSoftwareFeatures",
            "Win32_ProgIDSpecification",
            "Win32_ProgramGroup",
            "Win32_ProgramGroupContents",
            "Win32_ProgramGroupOrItem",
            "Win32_Property",
            "Win32_ProtocolBinding",
            "Win32_PublishComponentAction",
            "Win32_QuickFixEngineering",
            "Win32_Refrigeration",
            "Win32_Registry",
            "Win32_RegistryAction",
            "Win32_RemoveFileAction",
            "Win32_RemoveIniAction",
            "Win32_ReserveCost",
            "Win32_ScheduledJob",
            "Win32_SCSIController",
            "Win32_SCSIControllerDevice",
            "Win32_SecurityDescriptor",
            "Win32_SecuritySetting",
            "Win32_SecuritySettingAccess",
            "Win32_SecuritySettingAuditing",
            "Win32_SecuritySettingGroup",
            "Win32_SecuritySettingOfLogicalFile",
            "Win32_SecuritySettingOfLogicalShare",
            "Win32_SecuritySettingOfObject",
            "Win32_SecuritySettingOwner",
            "Win32_SelfRegModuleAction",
            "Win32_SerialPort",
            "Win32_SerialPortConfiguration",
            "Win32_SerialPortSetting",
            "Win32_Service",
            "Win32_ServiceControl",
            "Win32_ServiceSpecification",
            "Win32_ServiceSpecificationService",
            "Win32_SettingCheck",
            "Win32_Share",
            "Win32_ShareToDirectory",
            "Win32_ShortcutAction",
            "Win32_ShortcutFile",
            "Win32_ShortcutSAP",
            "Win32_SID",
            "Win32_SMBIOSMemory",
            "Win32_SoftwareElement",
            "Win32_SoftwareElementAction",
            "Win32_SoftwareElementCheck",
            "Win32_SoftwareElementCondition",
            "Win32_SoftwareElementResource",
            "Win32_SoftwareFeature",
            "Win32_SoftwareFeatureAction",
            "Win32_SoftwareFeatureCheck",
            "Win32_SoftwareFeatureParent",
            "Win32_SoftwareFeatureSoftwareElements",
            "Win32_SoundDevice",
            "Win32_StartupCommand",
            "Win32_SubDirectory",
            "Win32_SystemAccount",
            "Win32_SystemBIOS",
            "Win32_SystemBootConfiguration",
            "Win32_SystemDesktop",
            "Win32_SystemDevices",
            "Win32_SystemDriver",
            "Win32_SystemDriverPNPEntity",
            "Win32_SystemEnclosure",
            "Win32_SystemLoadOrderGroups",
            "Win32_SystemLogicalMemoryConfiguration",
            "Win32_SystemMemoryResource",
            "Win32_SystemNetworkConnections",
            "Win32_SystemOperatingSystem",
            "Win32_SystemPartitions",
            "Win32_SystemProcesses",
            "Win32_SystemProgramGroups",
            "Win32_SystemResources",
            "Win32_SystemServices",
            "Win32_SystemSetting",
            "Win32_SystemSlot",
            "Win32_SystemSystemDriver",
            "Win32_SystemTimeZone",
            "Win32_SystemUsers",
            "Win32_TapeDrive",
            "Win32_TemperatureProbe",
            "Win32_Thread",
            "Win32_TimeZone",
            "Win32_Trustee",
            "Win32_TypeLibraryAction",
            "Win32_UninterruptiblePowerSupply",
            "Win32_USBController",
            "Win32_USBControllerDevice",
            "Win32_UserAccount",
            "Win32_UserDesktop",
            "Win32_VideoConfiguration",
            "Win32_VideoController",
            "Win32_VideoSettings",
            "Win32_VoltageProbe",
            "Win32_WMIElementSetting",
            "Win32_WMISetting"});
          this.cmbxUtility.Location = new System.Drawing.Point(434, 16);
          this.cmbxUtility.Name = "cmbxUtility";
          this.cmbxUtility.Size = new System.Drawing.Size(194, 21);
          this.cmbxUtility.TabIndex = 4;
          this.cmbxUtility.SelectedIndexChanged += new System.EventHandler(this.cmbxUtility_SelectedIndexChanged);
          // 
          // m_listViewUtility
          // 
          this.m_listViewUtility.Columns.AddRange(new System.Windows.Forms.ColumnHeader[] {
            this.columnHeader9,
            this.columnHeader10});
          this.m_listViewUtility.Location = new System.Drawing.Point(16, 41);
          this.m_listViewUtility.Name = "m_listViewUtility";
          this.m_listViewUtility.Size = new System.Drawing.Size(612, 380);
          this.m_listViewUtility.TabIndex = 3;
          this.m_listViewUtility.UseCompatibleStateImageBehavior = false;
          this.m_listViewUtility.View = System.Windows.Forms.View.Details;
          // 
          // columnHeader9
          // 
          this.columnHeader9.Text = "Name";
          this.columnHeader9.Width = 72;
          // 
          // columnHeader10
          // 
          this.columnHeader10.Text = "Value";
          this.columnHeader10.Width = 503;
          // 
          // tabDeveloper
          // 
          this.tabDeveloper.Controls.Add(this.chkDeveloper);
          this.tabDeveloper.Controls.Add(this.cmbxDeveloper);
          this.tabDeveloper.Controls.Add(this.m_listViewDeveloper);
          this.tabDeveloper.Location = new System.Drawing.Point(4, 22);
          this.tabDeveloper.Name = "tabDeveloper";
          this.tabDeveloper.Padding = new System.Windows.Forms.Padding(3);
          this.tabDeveloper.Size = new System.Drawing.Size(640, 431);
          this.tabDeveloper.TabIndex = 6;
          this.tabDeveloper.Text = "Developer";
          this.tabDeveloper.UseVisualStyleBackColor = true;
          // 
          // chkDeveloper
          // 
          this.chkDeveloper.AutoSize = true;
          this.chkDeveloper.Checked = true;
          this.chkDeveloper.CheckState = System.Windows.Forms.CheckState.Checked;
          this.chkDeveloper.Location = new System.Drawing.Point(16, 18);
          this.chkDeveloper.Name = "chkDeveloper";
          this.chkDeveloper.Size = new System.Drawing.Size(162, 17);
          this.chkDeveloper.TabIndex = 7;
          this.chkDeveloper.Text = "Dont Display null Value items";
          this.chkDeveloper.UseVisualStyleBackColor = true;
          this.chkDeveloper.CheckedChanged += new System.EventHandler(this.chkDeveloper_CheckedChanged);
          // 
          // cmbxDeveloper
          // 
          this.cmbxDeveloper.DropDownStyle = System.Windows.Forms.ComboBoxStyle.DropDownList;
          this.cmbxDeveloper.FormattingEnabled = true;
          this.cmbxDeveloper.Items.AddRange(new object[] {
            "Win32_COMApplication",
            "Win32_COMApplicationClasses",
            "Win32_COMApplicationSettings",
            "Win32_COMClass",
            "Win32_ComClassAutoEmulator",
            "Win32_ComClassEmulator",
            "Win32_COMSetting",
            "Win32_ODBCAttribute",
            "Win32_ODBCDataSourceAttribute",
            "Win32_ODBCDataSourceSpecification",
            "Win32_ODBCDriverAttribute",
            "Win32_ODBCDriverSoftwareElement",
            "Win32_ODBCDriverSpecification",
            "Win32_ODBCSourceAttribute",
            "Win32_ODBCTranslatorSpecification",
            "Win32_Perf",
            "Win32_PerfRawData",
            "Win32_PerfRawData_ASP_ActiveServerPages",
            "Win32_PerfRawData_ASPNET_114322_ASPNETAppsv114322",
            "Win32_PerfRawData_ASPNET_114322_ASPNETv114322",
            "Win32_PerfRawData_ASPNET_ASPNET",
            "Win32_PerfRawData_ASPNET_ASPNETApplications",
            "Win32_PerfRawData_IAS_IASAccountingClients",
            "Win32_PerfRawData_IAS_IASAccountingServer",
            "Win32_PerfRawData_IAS_IASAuthenticationClients",
            "Win32_PerfRawData_IAS_IASAuthenticationServer",
            "Win32_PerfRawData_InetInfo_InternetInformationServicesGlobal",
            "Win32_PerfRawData_MSDTC_DistributedTransactionCoordinator",
            "Win32_PerfRawData_MSFTPSVC_FTPService",
            "Win32_PerfRawData_MSSQLSERVER_SQLServerAccessMethods",
            "Win32_PerfRawData_MSSQLSERVER_SQLServerBackupDevice",
            "Win32_PerfRawData_MSSQLSERVER_SQLServerBufferManager",
            "Win32_PerfRawData_MSSQLSERVER_SQLServerBufferPartition",
            "Win32_PerfRawData_MSSQLSERVER_SQLServerCacheManager",
            "Win32_PerfRawData_MSSQLSERVER_SQLServerDatabases",
            "Win32_PerfRawData_MSSQLSERVER_SQLServerGeneralStatistics",
            "Win32_PerfRawData_MSSQLSERVER_SQLServerLatches",
            "Win32_PerfRawData_MSSQLSERVER_SQLServerLocks",
            "Win32_PerfRawData_MSSQLSERVER_SQLServerMemoryManager",
            "Win32_PerfRawData_MSSQLSERVER_SQLServerReplicationAgents",
            "Win32_PerfRawData_MSSQLSERVER_SQLServerReplicationDist",
            "Win32_PerfRawData_MSSQLSERVER_SQLServerReplicationLogreader",
            "Win32_PerfRawData_MSSQLSERVER_SQLServerReplicationMerge",
            "Win32_PerfRawData_MSSQLSERVER_SQLServerReplicationSnapshot",
            "Win32_PerfRawData_MSSQLSERVER_SQLServerSQLStatistics",
            "Win32_PerfRawData_MSSQLSERVER_SQLServerUserSettable",
            "Win32_PerfRawData_NETFramework_NETCLRExceptions",
            "Win32_PerfRawData_NETFramework_NETCLRInterop",
            "Win32_PerfRawData_NETFramework_NETCLRJit",
            "Win32_PerfRawData_NETFramework_NETCLRLoading",
            "Win32_PerfRawData_NETFramework_NETCLRLocksAndThreads",
            "Win32_PerfRawData_NETFramework_NETCLRMemory",
            "Win32_PerfRawData_NETFramework_NETCLRRemoting",
            "Win32_PerfRawData_NETFramework_NETCLRSecurity",
            "Win32_PerfRawData_Outlook_Outlook",
            "Win32_PerfRawData_PerfDisk_PhysicalDisk",
            "Win32_PerfRawData_PerfNet_Browser",
            "Win32_PerfRawData_PerfNet_Redirector",
            "Win32_PerfRawData_PerfNet_Server",
            "Win32_PerfRawData_PerfNet_ServerWorkQueues",
            "Win32_PerfRawData_PerfOS_Cache",
            "Win32_PerfRawData_PerfOS_Memory",
            "Win32_PerfRawData_PerfOS_Objects",
            "Win32_PerfRawData_PerfOS_PagingFile",
            "Win32_PerfRawData_PerfOS_Processor",
            "Win32_PerfRawData_PerfOS_System",
            "Win32_PerfRawData_PerfProc_FullImage_Costly",
            "Win32_PerfRawData_PerfProc_Image_Costly",
            "Win32_PerfRawData_PerfProc_JobObject",
            "Win32_PerfRawData_PerfProc_JobObjectDetails",
            "Win32_PerfRawData_PerfProc_Process",
            "Win32_PerfRawData_PerfProc_ProcessAddressSpace_Costly",
            "Win32_PerfRawData_PerfProc_Thread",
            "Win32_PerfRawData_PerfProc_ThreadDetails_Costly",
            "Win32_PerfRawData_RemoteAccess_RASPort",
            "Win32_PerfRawData_RemoteAccess_RASTotal",
            "Win32_PerfRawData_RSVP_ACSPerRSVPService",
            "Win32_PerfRawData_Spooler_PrintQueue",
            "Win32_PerfRawData_TapiSrv_Telephony",
            "Win32_SoftwareElement",
            "Win32_SoftwareElementAction",
            "Win32_SoftwareElementCheck",
            "Win32_SoftwareElementCondition",
            "Win32_SoftwareElementResource",
            "Win32_SoftwareFeature",
            "Win32_SoftwareFeatureAction",
            "Win32_SoftwareFeatureCheck",
            "Win32_SoftwareFeatureParent",
            "Win32_SoftwareFeatureSoftwareElements",
            "Win32_ClassicCOMApplicationClasses",
            "Win32_ClassicCOMClass",
            "Win32_ClassicCOMClassSetting",
            "Win32_ClassicCOMClassSettings",
            "Win32_ClassInfoAction",
            "Win32_ClientApplicationSetting",
            "Win32_CodecFile",
            "Win32_DCOMApplication",
            "Win32_DCOMApplicationAccessAllowedSetting",
            "Win32_DCOMApplicationLaunchAllowedSetting",
            "Win32_DCOMApplicationSetting"});
          this.cmbxDeveloper.Location = new System.Drawing.Point(434, 16);
          this.cmbxDeveloper.Name = "cmbxDeveloper";
          this.cmbxDeveloper.Size = new System.Drawing.Size(194, 21);
          this.cmbxDeveloper.TabIndex = 6;
          this.cmbxDeveloper.SelectedIndexChanged += new System.EventHandler(this.cmbxDeveloper_SelectedIndexChanged);
          // 
          // m_listViewDeveloper
          // 
          this.m_listViewDeveloper.Columns.AddRange(new System.Windows.Forms.ColumnHeader[] {
            this.columnHeader15,
            this.columnHeader16});
          this.m_listViewDeveloper.Location = new System.Drawing.Point(16, 41);
          this.m_listViewDeveloper.Name = "m_listViewDeveloper";
          this.m_listViewDeveloper.Size = new System.Drawing.Size(612, 380);
          this.m_listViewDeveloper.TabIndex = 5;
          this.m_listViewDeveloper.UseCompatibleStateImageBehavior = false;
          this.m_listViewDeveloper.View = System.Windows.Forms.View.Details;
          // 
          // columnHeader15
          // 
          this.columnHeader15.Text = "Name";
          this.columnHeader15.Width = 193;
          // 
          // columnHeader16
          // 
          this.columnHeader16.Text = "Value";
          this.columnHeader16.Width = 337;
          // 
          // tabUserAccount
          // 
          this.tabUserAccount.Controls.Add(this.chkUserAccount);
          this.tabUserAccount.Controls.Add(this.cmbxUserAccount);
          this.tabUserAccount.Controls.Add(this.m_listViewUserAccount);
          this.tabUserAccount.Location = new System.Drawing.Point(4, 22);
          this.tabUserAccount.Name = "tabUserAccount";
          this.tabUserAccount.Padding = new System.Windows.Forms.Padding(3);
          this.tabUserAccount.Size = new System.Drawing.Size(640, 431);
          this.tabUserAccount.TabIndex = 5;
          this.tabUserAccount.Text = "User Account & Security";
          this.tabUserAccount.UseVisualStyleBackColor = true;
          // 
          // chkUserAccount
          // 
          this.chkUserAccount.AutoSize = true;
          this.chkUserAccount.Checked = true;
          this.chkUserAccount.CheckState = System.Windows.Forms.CheckState.Checked;
          this.chkUserAccount.Location = new System.Drawing.Point(16, 18);
          this.chkUserAccount.Name = "chkUserAccount";
          this.chkUserAccount.Size = new System.Drawing.Size(162, 17);
          this.chkUserAccount.TabIndex = 5;
          this.chkUserAccount.Text = "Dont Display null Value items";
          this.chkUserAccount.UseVisualStyleBackColor = true;
          this.chkUserAccount.CheckedChanged += new System.EventHandler(this.chkUserAccount_CheckedChanged);
          // 
          // cmbxUserAccount
          // 
          this.cmbxUserAccount.DropDownStyle = System.Windows.Forms.ComboBoxStyle.DropDownList;
          this.cmbxUserAccount.FormattingEnabled = true;
          this.cmbxUserAccount.Items.AddRange(new object[] {
            "Win32_SystemUsers",
            "Win32_Account",
            "Win32_AccountSID",
            "Win32_SecurityDescriptor",
            "Win32_SecuritySetting",
            "Win32_SecuritySettingAccess",
            "Win32_SecuritySettingAuditing",
            "Win32_SecuritySettingGroup",
            "Win32_SecuritySettingOfLogicalFile",
            "Win32_SecuritySettingOfLogicalShare",
            "Win32_SecuritySettingOfObject",
            "Win32_SecuritySettingOwner",
            "Win32_NTEventlogFile",
            "Win32_NTLogEvent",
            "Win32_NTLogEventComputer",
            "Win32_NTLogEventLog",
            "Win32_NTLogEventUser"});
          this.cmbxUserAccount.Location = new System.Drawing.Point(434, 16);
          this.cmbxUserAccount.Name = "cmbxUserAccount";
          this.cmbxUserAccount.Size = new System.Drawing.Size(194, 21);
          this.cmbxUserAccount.TabIndex = 4;
          this.cmbxUserAccount.SelectedIndexChanged += new System.EventHandler(this.cmbxUserAccount_SelectedIndexChanged);
          // 
          // m_listViewUserAccount
          // 
          this.m_listViewUserAccount.Columns.AddRange(new System.Windows.Forms.ColumnHeader[] {
            this.columnHeader13,
            this.columnHeader14});
          this.m_listViewUserAccount.Location = new System.Drawing.Point(16, 41);
          this.m_listViewUserAccount.Name = "m_listViewUserAccount";
          this.m_listViewUserAccount.Size = new System.Drawing.Size(612, 380);
          this.m_listViewUserAccount.TabIndex = 3;
          this.m_listViewUserAccount.UseCompatibleStateImageBehavior = false;
          this.m_listViewUserAccount.View = System.Windows.Forms.View.Details;
          // 
          // columnHeader13
          // 
          this.columnHeader13.Text = "Name";
          this.columnHeader13.Width = 72;
          // 
          // columnHeader14
          // 
          this.columnHeader14.Text = "Value";
          this.columnHeader14.Width = 503;
          // 
          // tabNetwork
          // 
          this.tabNetwork.Controls.Add(this.chkNetwork);
          this.tabNetwork.Controls.Add(this.cmbxNetwork);
          this.tabNetwork.Controls.Add(this.m_listViewNetwork);
          this.tabNetwork.Location = new System.Drawing.Point(4, 22);
          this.tabNetwork.Name = "tabNetwork";
          this.tabNetwork.Padding = new System.Windows.Forms.Padding(3);
          this.tabNetwork.Size = new System.Drawing.Size(640, 431);
          this.tabNetwork.TabIndex = 1;
          this.tabNetwork.Text = "Network";
          this.tabNetwork.UseVisualStyleBackColor = true;
          // 
          // chkNetwork
          // 
          this.chkNetwork.AutoSize = true;
          this.chkNetwork.Checked = true;
          this.chkNetwork.CheckState = System.Windows.Forms.CheckState.Checked;
          this.chkNetwork.Location = new System.Drawing.Point(16, 18);
          this.chkNetwork.Name = "chkNetwork";
          this.chkNetwork.Size = new System.Drawing.Size(162, 17);
          this.chkNetwork.TabIndex = 3;
          this.chkNetwork.Text = "Dont Display null Value items";
          this.chkNetwork.UseVisualStyleBackColor = true;
          this.chkNetwork.CheckedChanged += new System.EventHandler(this.chkNetwork_CheckedChanged);
          // 
          // cmbxNetwork
          // 
          this.cmbxNetwork.DropDownStyle = System.Windows.Forms.ComboBoxStyle.DropDownList;
          this.cmbxNetwork.FormattingEnabled = true;
          this.cmbxNetwork.Items.AddRange(new object[] {
            "Win32_NetworkAdapter",
            "Win32_NetworkAdapterConfiguration",
            "Win32_NetworkAdapterSetting",
            "Win32_NetworkClient",
            "Win32_NetworkConnection",
            "Win32_NetworkLoginProfile",
            "Win32_NetworkProtocol",
            "Win32_PerfRawData_Tcpip_ICMP",
            "Win32_PerfRawData_Tcpip_IP",
            "Win32_PerfRawData_Tcpip_NBTConnection",
            "Win32_PerfRawData_Tcpip_NetworkInterface",
            "Win32_PerfRawData_Tcpip_TCP",
            "Win32_PerfRawData_Tcpip_UDP",
            "Win32_PerfRawData_W3SVC_WebService",
            "Win32_SystemNetworkConnections"});
          this.cmbxNetwork.Location = new System.Drawing.Point(434, 16);
          this.cmbxNetwork.Name = "cmbxNetwork";
          this.cmbxNetwork.Size = new System.Drawing.Size(194, 21);
          this.cmbxNetwork.TabIndex = 2;
          this.cmbxNetwork.SelectedIndexChanged += new System.EventHandler(this.cmbxNetwork_SelectedIndexChanged);
          // 
          // m_listViewNetwork
          // 
          this.m_listViewNetwork.Columns.AddRange(new System.Windows.Forms.ColumnHeader[] {
            this.columnHeader3,
            this.columnHeader4});
          this.m_listViewNetwork.Location = new System.Drawing.Point(16, 41);
          this.m_listViewNetwork.Name = "m_listViewNetwork";
          this.m_listViewNetwork.Size = new System.Drawing.Size(612, 380);
          this.m_listViewNetwork.TabIndex = 1;
          this.m_listViewNetwork.UseCompatibleStateImageBehavior = false;
          this.m_listViewNetwork.View = System.Windows.Forms.View.Details;
          // 
          // columnHeader3
          // 
          this.columnHeader3.Text = "Name";
          this.columnHeader3.Width = 72;
          // 
          // columnHeader4
          // 
          this.columnHeader4.Text = "Value";
          this.columnHeader4.Width = 503;
          // 
          // tabSystemInfo
          // 
          this.tabSystemInfo.Controls.Add(this.chkSystemInfo);
          this.tabSystemInfo.Controls.Add(this.cmbxSystemInfo);
          this.tabSystemInfo.Controls.Add(this.m_listViewSystemInfo);
          this.tabSystemInfo.Location = new System.Drawing.Point(4, 22);
          this.tabSystemInfo.Name = "tabSystemInfo";
          this.tabSystemInfo.Padding = new System.Windows.Forms.Padding(3);
          this.tabSystemInfo.Size = new System.Drawing.Size(640, 431);
          this.tabSystemInfo.TabIndex = 2;
          this.tabSystemInfo.Text = "System Info";
          this.tabSystemInfo.UseVisualStyleBackColor = true;
          // 
          // chkSystemInfo
          // 
          this.chkSystemInfo.AutoSize = true;
          this.chkSystemInfo.Checked = true;
          this.chkSystemInfo.CheckState = System.Windows.Forms.CheckState.Checked;
          this.chkSystemInfo.Location = new System.Drawing.Point(16, 18);
          this.chkSystemInfo.Name = "chkSystemInfo";
          this.chkSystemInfo.Size = new System.Drawing.Size(162, 17);
          this.chkSystemInfo.TabIndex = 5;
          this.chkSystemInfo.Text = "Dont Display null Value items";
          this.chkSystemInfo.UseVisualStyleBackColor = true;
          this.chkSystemInfo.CheckedChanged += new System.EventHandler(this.chkSystemInfo_CheckedChanged);
          // 
          // cmbxSystemInfo
          // 
          this.cmbxSystemInfo.DropDownStyle = System.Windows.Forms.ComboBoxStyle.DropDownList;
          this.cmbxSystemInfo.FormattingEnabled = true;
          this.cmbxSystemInfo.Items.AddRange(new object[] {
            "Win32_ACE",
            "Win32_ActionCheck",
            "Win32_AllocatedResource",
            "Win32_ApplicationCommandLine",
            "Win32_ApplicationService",
            "Win32_ApplicationCommandLine",
            "Win32_ApplicationService",
            "Win32_Account",
            "Win32_AccountSID",
            "Win32_ACE",
            "Win32_ActionCheck",
            "Win32_AllocatedResource",
            "Win32_AssociatedBattery",
            "Win32_AssociatedProcessorMemory",
            "Win32_Process",
            "Win32_ProcessStartup",
            "Win32_Product",
            "Win32_ProductCheck",
            "Win32_ProductResource",
            "Win32_ProductSoftwareFeatures",
            "Win32_ProgIDSpecification",
            "Win32_ProgramGroup",
            "Win32_ProgramGroupContents",
            "Win32_ProgramGroupOrItem",
            "Win32_Property",
            "Win32_ProtocolBinding",
            "Win32_PublishComponentAction",
            "Win32_QuickFixEngineering",
            "Win32_Refrigeration",
            "Win32_Registry",
            "Win32_RegistryAction",
            "Win32_SystemAccount",
            "Win32_SystemBIOS",
            "Win32_SystemBootConfiguration",
            "Win32_SystemDesktop",
            "Win32_SystemDevices",
            "Win32_SystemDriver",
            "Win32_SystemDriverPNPEntity",
            "Win32_SystemEnclosure",
            "Win32_SystemLoadOrderGroups",
            "Win32_SystemLogicalMemoryConfiguration",
            "Win32_SystemMemoryResource",
            "Win32_SystemOperatingSystem",
            "Win32_SystemPartitions",
            "Win32_SystemProcesses",
            "Win32_SystemProgramGroups",
            "Win32_SystemResources",
            "Win32_SystemServices",
            "Win32_SystemSetting",
            "Win32_SystemSlot",
            "Win32_SystemSystemDriver",
            "Win32_SystemTimeZone",
            "Win32_ComputerSystem",
            "Win32_ComputerSystemProcessor",
            "Win32_ComputerSystemProduct",
            "Win32_Service",
            "Win32_ServiceControl",
            "Win32_ServiceSpecification",
            "Win32_ServiceSpecificationService"});
          this.cmbxSystemInfo.Location = new System.Drawing.Point(434, 16);
          this.cmbxSystemInfo.Name = "cmbxSystemInfo";
          this.cmbxSystemInfo.Size = new System.Drawing.Size(194, 21);
          this.cmbxSystemInfo.TabIndex = 4;
          this.cmbxSystemInfo.SelectedIndexChanged += new System.EventHandler(this.cmbxSystemInfo_SelectedIndexChanged);
          // 
          // m_listViewSystemInfo
          // 
          this.m_listViewSystemInfo.Columns.AddRange(new System.Windows.Forms.ColumnHeader[] {
            this.columnHeader7,
            this.columnHeader8});
          this.m_listViewSystemInfo.Location = new System.Drawing.Point(16, 41);
          this.m_listViewSystemInfo.Name = "m_listViewSystemInfo";
          this.m_listViewSystemInfo.Size = new System.Drawing.Size(612, 380);
          this.m_listViewSystemInfo.TabIndex = 3;
          this.m_listViewSystemInfo.UseCompatibleStateImageBehavior = false;
          this.m_listViewSystemInfo.View = System.Windows.Forms.View.Details;
          // 
          // columnHeader7
          // 
          this.columnHeader7.Text = "Name";
          this.columnHeader7.Width = 72;
          // 
          // columnHeader8
          // 
          this.columnHeader8.Text = "Value";
          this.columnHeader8.Width = 503;
          // 
          // tabMemory
          // 
          this.tabMemory.Controls.Add(this.chkMemory);
          this.tabMemory.Controls.Add(this.cmbxMemory);
          this.tabMemory.Controls.Add(this.m_listViewMemory);
          this.tabMemory.Location = new System.Drawing.Point(4, 22);
          this.tabMemory.Name = "tabMemory";
          this.tabMemory.Padding = new System.Windows.Forms.Padding(3);
          this.tabMemory.Size = new System.Drawing.Size(640, 431);
          this.tabMemory.TabIndex = 7;
          this.tabMemory.Text = "Memory";
          this.tabMemory.UseVisualStyleBackColor = true;
          // 
          // chkMemory
          // 
          this.chkMemory.AutoSize = true;
          this.chkMemory.Checked = true;
          this.chkMemory.CheckState = System.Windows.Forms.CheckState.Checked;
          this.chkMemory.Location = new System.Drawing.Point(16, 18);
          this.chkMemory.Name = "chkMemory";
          this.chkMemory.Size = new System.Drawing.Size(162, 17);
          this.chkMemory.TabIndex = 9;
          this.chkMemory.Text = "Dont Display null Value items";
          this.chkMemory.UseVisualStyleBackColor = true;
          this.chkMemory.CheckedChanged += new System.EventHandler(this.chkMemory_CheckedChanged);
          // 
          // cmbxMemory
          // 
          this.cmbxMemory.DropDownStyle = System.Windows.Forms.ComboBoxStyle.DropDownList;
          this.cmbxMemory.FormattingEnabled = true;
          this.cmbxMemory.Items.AddRange(new object[] {
            "Win32_CacheMemory",
            "Win32_MemoryArray",
            "Win32_MemoryArrayLocation",
            "Win32_MemoryDevice",
            "Win32_MemoryDeviceArray",
            "Win32_MemoryDeviceLocation",
            "Win32_AssociatedProcessorMemory",
            "Win32_DeviceMemoryAddress",
            "Win32_LogicalMemoryConfiguration",
            "Win32_PerfRawData_PerfOS_Memory",
            "Win32_PhysicalMemory",
            "Win32_PhysicalMemoryArray",
            "Win32_PhysicalMemoryLocation",
            "Win32_SMBIOSMemory",
            "Win32_SystemLogicalMemoryConfiguration",
            "Win32_SystemMemoryResource"});
          this.cmbxMemory.Location = new System.Drawing.Point(434, 16);
          this.cmbxMemory.Name = "cmbxMemory";
          this.cmbxMemory.Size = new System.Drawing.Size(194, 21);
          this.cmbxMemory.TabIndex = 8;
          this.cmbxMemory.SelectedIndexChanged += new System.EventHandler(this.cmbxMemory_SelectedIndexChanged);
          // 
          // m_listViewMemory
          // 
          this.m_listViewMemory.Columns.AddRange(new System.Windows.Forms.ColumnHeader[] {
            this.columnHeader17,
            this.columnHeader18});
          this.m_listViewMemory.Location = new System.Drawing.Point(16, 41);
          this.m_listViewMemory.Name = "m_listViewMemory";
          this.m_listViewMemory.Size = new System.Drawing.Size(612, 380);
          this.m_listViewMemory.TabIndex = 7;
          this.m_listViewMemory.UseCompatibleStateImageBehavior = false;
          this.m_listViewMemory.View = System.Windows.Forms.View.Details;
          // 
          // columnHeader17
          // 
          this.columnHeader17.Text = "Name";
          this.columnHeader17.Width = 72;
          // 
          // columnHeader18
          // 
          this.columnHeader18.Text = "Value";
          this.columnHeader18.Width = 503;
          // 
          // tabDataStorage
          // 
          this.tabDataStorage.Controls.Add(this.chkDataStorage);
          this.tabDataStorage.Controls.Add(this.cmbxStorage);
          this.tabDataStorage.Controls.Add(this.m_listViewStorage);
          this.tabDataStorage.Location = new System.Drawing.Point(4, 22);
          this.tabDataStorage.Name = "tabDataStorage";
          this.tabDataStorage.Padding = new System.Windows.Forms.Padding(3);
          this.tabDataStorage.Size = new System.Drawing.Size(640, 431);
          this.tabDataStorage.TabIndex = 4;
          this.tabDataStorage.Text = "Data Storage";
          this.tabDataStorage.UseVisualStyleBackColor = true;
          // 
          // chkDataStorage
          // 
          this.chkDataStorage.AutoSize = true;
          this.chkDataStorage.Checked = true;
          this.chkDataStorage.CheckState = System.Windows.Forms.CheckState.Checked;
          this.chkDataStorage.Location = new System.Drawing.Point(16, 18);
          this.chkDataStorage.Name = "chkDataStorage";
          this.chkDataStorage.Size = new System.Drawing.Size(162, 17);
          this.chkDataStorage.TabIndex = 5;
          this.chkDataStorage.Text = "Dont Display null Value items";
          this.chkDataStorage.UseVisualStyleBackColor = true;
          this.chkDataStorage.CheckedChanged += new System.EventHandler(this.chkDataStorage_CheckedChanged);
          // 
          // cmbxStorage
          // 
          this.cmbxStorage.DropDownStyle = System.Windows.Forms.ComboBoxStyle.DropDownList;
          this.cmbxStorage.FormattingEnabled = true;
          this.cmbxStorage.Items.AddRange(new object[] {
            "Win32_DiskDrive",
            "Win32_DiskDriveToDiskPartition",
            "Win32_DiskPartition",
            "Win32_LogicalDisk",
            "Win32_LogicalDiskRootDirectory",
            "Win32_LogicalDiskToPartition",
            "Win32_LogicalFileAccess",
            "Win32_LogicalFileAuditing",
            "Win32_LogicalFileGroup",
            "Win32_LogicalFileOwner",
            "Win32_LogicalFileSecuritySetting",
            "Win32_TapeDrive"});
          this.cmbxStorage.Location = new System.Drawing.Point(434, 16);
          this.cmbxStorage.Name = "cmbxStorage";
          this.cmbxStorage.Size = new System.Drawing.Size(194, 21);
          this.cmbxStorage.TabIndex = 4;
          this.cmbxStorage.SelectedIndexChanged += new System.EventHandler(this.cmbxStorage_SelectedIndexChanged);
          // 
          // m_listViewStorage
          // 
          this.m_listViewStorage.Columns.AddRange(new System.Windows.Forms.ColumnHeader[] {
            this.columnHeader11,
            this.columnHeader12});
          this.m_listViewStorage.Location = new System.Drawing.Point(16, 41);
          this.m_listViewStorage.Name = "m_listViewStorage";
          this.m_listViewStorage.Size = new System.Drawing.Size(612, 380);
          this.m_listViewStorage.TabIndex = 3;
          this.m_listViewStorage.UseCompatibleStateImageBehavior = false;
          this.m_listViewStorage.View = System.Windows.Forms.View.Details;
          // 
          // columnHeader11
          // 
          this.columnHeader11.Text = "Name";
          this.columnHeader11.Width = 72;
          // 
          // columnHeader12
          // 
          this.columnHeader12.Text = "Value";
          this.columnHeader12.Width = 503;
          // 
          // tabHardware
          // 
          this.tabHardware.Controls.Add(this.chkHardware);
          this.tabHardware.Controls.Add(this.cmbxOption);
          this.tabHardware.Controls.Add(this.m_listViewDisplayHardware);
          this.tabHardware.Location = new System.Drawing.Point(4, 22);
          this.tabHardware.Name = "tabHardware";
          this.tabHardware.Padding = new System.Windows.Forms.Padding(3);
          this.tabHardware.Size = new System.Drawing.Size(640, 431);
          this.tabHardware.TabIndex = 0;
          this.tabHardware.Text = "Hardware Info";
          this.tabHardware.UseVisualStyleBackColor = true;
          // 
          // chkHardware
          // 
          this.chkHardware.AutoSize = true;
          this.chkHardware.Checked = true;
          this.chkHardware.CheckState = System.Windows.Forms.CheckState.Checked;
          this.chkHardware.Location = new System.Drawing.Point(16, 18);
          this.chkHardware.Name = "chkHardware";
          this.chkHardware.Size = new System.Drawing.Size(162, 17);
          this.chkHardware.TabIndex = 2;
          this.chkHardware.Text = "Dont Display null Value items";
          this.chkHardware.UseVisualStyleBackColor = true;
          this.chkHardware.CheckedChanged += new System.EventHandler(this.chkHardware_CheckedChanged);
          // 
          // cmbxOption
          // 
          this.cmbxOption.DropDownStyle = System.Windows.Forms.ComboBoxStyle.DropDownList;
          this.cmbxOption.FlatStyle = System.Windows.Forms.FlatStyle.System;
          this.cmbxOption.FormattingEnabled = true;
          this.cmbxOption.Items.AddRange(new object[] {
            "Win32_1394Controller",
            "Win32_1394ControllerDevice",
            "Win32_BaseBoard",
            "Win32_Battery",
            "Win32_BIOS",
            "Win32_Bus",
            "Win32_CDROMDrive",
            "Win32_CIMLogicalDeviceCIMDataFile",
            "Win32_DeviceBus",
            "Win32_DeviceMemoryAddress",
            "Win32_DeviceSettings",
            "Win32_DisplayConfiguration",
            "Win32_DisplayControllerConfiguration",
            "Win32_DMAChannel",
            "Win32_DriverVXD",
            "Win32_FloppyController",
            "Win32_FloppyDrive",
            "Win32_HeatPipe",
            "Win32_IDEController",
            "Win32_IDEControllerDevice",
            "Win32_InfraredDevice",
            "Win32_IRQResource",
            "Win32_Keyboard",
            "Win32_MotherboardDevice",
            "Win32_OnBoardDevice",
            "Win32_PCMCIAController",
            "Win32_PNPAllocatedResource",
            "Win32_PnPDevice",
            "Win32_PnPEntity",
            "Win32_PointingDevice",
            "Win32_PortableBattery",
            "Win32_PortConnector",
            "Win32_PortResource",
            "Win32_POTSModem",
            "Win32_POTSModemToSerialPort",
            "Win32_PowerManagementEvent",
            "Win32_Printer",
            "Win32_PrinterConfiguration",
            "Win32_PrinterController",
            "Win32_PrinterDriverDll",
            "Win32_PrinterSetting",
            "Win32_PrinterShare",
            "Win32_PrintJob",
            "Win32_Processor",
            "Win32_SCSIController",
            "Win32_SCSIControllerDevice",
            "Win32_SerialPort",
            "Win32_SerialPortConfiguration",
            "Win32_SerialPortSetting",
            "Win32_SMBIOSMemory",
            "Win32_SoundDevice",
            "Win32_TemperatureProbe",
            "Win32_USBController",
            "Win32_USBControllerDevice",
            "Win32_VideoConfiguration",
            "Win32_VideoController",
            "Win32_VideoSettings",
            "Win32_VoltageProbe"});
          this.cmbxOption.Location = new System.Drawing.Point(434, 16);
          this.cmbxOption.Name = "cmbxOption";
          this.cmbxOption.Size = new System.Drawing.Size(194, 21);
          this.cmbxOption.Sorted = true;
          this.cmbxOption.TabIndex = 1;
          this.cmbxOption.SelectedIndexChanged += new System.EventHandler(this.cmbxOption_SelectedIndexChanged);
          // 
          // m_listViewDisplayHardware
          // 
          this.m_listViewDisplayHardware.Columns.AddRange(new System.Windows.Forms.ColumnHeader[] {
            this.columnHeader1,
            this.columnHeader2});
          this.m_listViewDisplayHardware.Location = new System.Drawing.Point(16, 41);
          this.m_listViewDisplayHardware.Name = "m_listViewDisplayHardware";
          this.m_listViewDisplayHardware.Size = new System.Drawing.Size(612, 380);
          this.m_listViewDisplayHardware.TabIndex = 0;
          this.m_listViewDisplayHardware.UseCompatibleStateImageBehavior = false;
          this.m_listViewDisplayHardware.View = System.Windows.Forms.View.Details;
          // 
          // columnHeader1
          // 
          this.columnHeader1.Text = "Name";
          this.columnHeader1.Width = 72;
          // 
          // columnHeader2
          // 
          this.columnHeader2.Text = "Value";
          this.columnHeader2.Width = 503;
          // 
          // MainTabControl
          // 
          this.MainTabControl.Controls.Add(this.tabHardware);
          this.MainTabControl.Controls.Add(this.tabDataStorage);
          this.MainTabControl.Controls.Add(this.tabMemory);
          this.MainTabControl.Controls.Add(this.tabSystemInfo);
          this.MainTabControl.Controls.Add(this.tabNetwork);
          this.MainTabControl.Controls.Add(this.tabUserAccount);
          this.MainTabControl.Controls.Add(this.tabDeveloper);
          this.MainTabControl.Controls.Add(this.tabUtility);
          this.MainTabControl.Dock = System.Windows.Forms.DockStyle.Fill;
          this.MainTabControl.Location = new System.Drawing.Point(0, 0);
          this.MainTabControl.Multiline = true;
          this.MainTabControl.Name = "MainTabControl";
          this.MainTabControl.SelectedIndex = 0;
          this.MainTabControl.Size = new System.Drawing.Size(648, 457);
          this.MainTabControl.TabIndex = 0;
          // 
          // GetHardwareInfo
          // 
          this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 13F);
          this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
          this.ClientSize = new System.Drawing.Size(648, 457);
          this.Controls.Add(this.MainTabControl);
          this.FormBorderStyle = System.Windows.Forms.FormBorderStyle.FixedSingle;
          this.MaximizeBox = false;
          this.Name = "GetHardwareInfo";
          this.Text = "Get Hardware Information";
          this.tabUtility.ResumeLayout(false);
          this.tabUtility.PerformLayout();
          this.tabDeveloper.ResumeLayout(false);
          this.tabDeveloper.PerformLayout();
          this.tabUserAccount.ResumeLayout(false);
          this.tabUserAccount.PerformLayout();
          this.tabNetwork.ResumeLayout(false);
          this.tabNetwork.PerformLayout();
          this.tabSystemInfo.ResumeLayout(false);
          this.tabSystemInfo.PerformLayout();
          this.tabMemory.ResumeLayout(false);
          this.tabMemory.PerformLayout();
          this.tabDataStorage.ResumeLayout(false);
          this.tabDataStorage.PerformLayout();
          this.tabHardware.ResumeLayout(false);
          this.tabHardware.PerformLayout();
          this.MainTabControl.ResumeLayout(false);
          this.ResumeLayout(false);

        }

        #endregion

        private System.Windows.Forms.ColumnHeader columnHeader5;
        private System.Windows.Forms.ColumnHeader columnHeader6;
        private System.Windows.Forms.TabPage tabUtility;
        private System.Windows.Forms.CheckBox chkUtility;
        private System.Windows.Forms.ComboBox cmbxUtility;
        private System.Windows.Forms.ListView m_listViewUtility;
        private System.Windows.Forms.ColumnHeader columnHeader9;
        private System.Windows.Forms.ColumnHeader columnHeader10;
        private System.Windows.Forms.TabPage tabDeveloper;
        private System.Windows.Forms.CheckBox chkDeveloper;
        private System.Windows.Forms.ComboBox cmbxDeveloper;
        private System.Windows.Forms.ListView m_listViewDeveloper;
        private System.Windows.Forms.ColumnHeader columnHeader15;
        private System.Windows.Forms.ColumnHeader columnHeader16;
        private System.Windows.Forms.TabPage tabUserAccount;
        private System.Windows.Forms.CheckBox chkUserAccount;
        private System.Windows.Forms.ComboBox cmbxUserAccount;
        private System.Windows.Forms.ListView m_listViewUserAccount;
        private System.Windows.Forms.ColumnHeader columnHeader13;
        private System.Windows.Forms.ColumnHeader columnHeader14;
        private System.Windows.Forms.TabPage tabNetwork;
        private System.Windows.Forms.CheckBox chkNetwork;
        private System.Windows.Forms.ComboBox cmbxNetwork;
        private System.Windows.Forms.ListView m_listViewNetwork;
        private System.Windows.Forms.ColumnHeader columnHeader3;
        private System.Windows.Forms.ColumnHeader columnHeader4;
        private System.Windows.Forms.TabPage tabSystemInfo;
        private System.Windows.Forms.CheckBox chkSystemInfo;
        private System.Windows.Forms.ComboBox cmbxSystemInfo;
        private System.Windows.Forms.ListView m_listViewSystemInfo;
        private System.Windows.Forms.ColumnHeader columnHeader7;
        private System.Windows.Forms.ColumnHeader columnHeader8;
        private System.Windows.Forms.TabPage tabMemory;
        private System.Windows.Forms.CheckBox chkMemory;
        private System.Windows.Forms.ComboBox cmbxMemory;
        private System.Windows.Forms.ListView m_listViewMemory;
        private System.Windows.Forms.ColumnHeader columnHeader17;
        private System.Windows.Forms.ColumnHeader columnHeader18;
        private System.Windows.Forms.TabPage tabDataStorage;
        private System.Windows.Forms.CheckBox chkDataStorage;
        private System.Windows.Forms.ComboBox cmbxStorage;
        private System.Windows.Forms.ListView m_listViewStorage;
        private System.Windows.Forms.ColumnHeader columnHeader11;
        private System.Windows.Forms.ColumnHeader columnHeader12;
        private System.Windows.Forms.TabPage tabHardware;
        private System.Windows.Forms.CheckBox chkHardware;
        private System.Windows.Forms.ComboBox cmbxOption;
        private System.Windows.Forms.ListView m_listViewDisplayHardware;
        private System.Windows.Forms.ColumnHeader columnHeader1;
        private System.Windows.Forms.ColumnHeader columnHeader2;
        private System.Windows.Forms.TabControl MainTabControl;
    }
}
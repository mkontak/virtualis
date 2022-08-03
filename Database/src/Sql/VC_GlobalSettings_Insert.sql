USE [System]

INSERT INTO [v3d].[Configs] ( [name], [desc], [set] ) VALUES ( 'globalSettings', 'Site level settings for the VC Viewer.', '<globalSettings ver="1.0">
    <key name="IdleInterval" value="1800" />
    <key name="TopMargin" value="1.0" />
    <key name="BottomMargin" value="1.0" />
    <key name="HeaderSelection" value="0" />
    <key name="HeaderText" value="" />
    <key name="Numbering" value="-1" />
    <key name="AddCRADStoReport" value="true" />
    <key name="ReportDisclaimer" value="" />
    <key name="PacsSupportsEncapsulatedPdf" value="<SupportsEncapsulatedPDF>"/>
    <key name="PacsAeTitleLocal" value="v3D" />
    <key name="PacsAeTitleServer" value="<AETitle>" />
    <key name="PacsHostname" value="<Server>" />
    <key name="PacsPort" value="<Port>" />
    <key name="PacsSingleSeries" value="true" />
    <key name="SendToPacs" value="<SendToPacs>" />
  </globalSettings>' )
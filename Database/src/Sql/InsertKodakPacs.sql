USE [System]
GO


INSERT [v3d].[DicomHosts] ( [name], [server], [aetitle], [port], [desc], [services], [attributes] ) VALUES
( 'Kodak', 'kodak-pacs', 'kodak-pacsFIR', 2104, '', 8, '<supports_get_command>false</supports_get_command>
    <supports_encapsulated_pdf>false</supports_encapsulated_pdf>
    <supports_series_level_query>true</supports_series_level_query>' )

GO
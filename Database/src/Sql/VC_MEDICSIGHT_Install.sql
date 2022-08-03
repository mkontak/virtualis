DECLARE @APP AS NVARCHAR(128)
DECLARE @CAD AS NVARCHAR(128)

SET @APP = 'VC'
SET @CAD = 'MedicSight'



IF NOT EXISTS ( SELECT * FROM [System].[v3d].[Applications] WHERE [id] = @APP AND [inst] = 1)
BEGIN

  DECLARE @MESSAGE AS NVARCHAR(256)

  SET @MESSAGE = 'Cannot install ' + @CAD + ' CAD for ' + @APP + ' ,  ' + @APP + ' is not installed'

  RAISERROR(@MESSAGE, 16, 1 ) WITH SETERROR

END
ELSE
BEGIN

  DECLARE @INSTALLED AS INT

  SET @INSTALLED = 0

 IF NOT EXISTS ( SELECT * FROM [System].[v3d].Templates WHERE [id] = 'vc-ec-medicsight' ) 
  INSERT INTO [System].[v3d].Templates ( [id], [app], [mod], [desc],  [cad], [default], [tasks] ) 
         VALUES ( 'vc-ec-medicsight', 'VC', '', @APP + ' with ' + @CAD + ' CAD and Electronic Cleansing',  @CAD, 0, 
                  '<task name="pipeline" exe="$(ProcessorDirectory)VC\vcPipeline.exe" log="3" type="Preprocessed" idx="2" max="600" >
                      <params>
                       <param name="ElectronicCleansing" value="Yes" cmd="-stage all" />
                      </params>
                   </task>
                   <task name="cad" exe="$(ProcessorDirectory)CAD\v3DCad.exe" log="3" type="CAD" idx="3" max="600" >
                     <params>
                       <param name="ApplicationId" value="MedicSight" cmd="-appid Medicsight.ColonCADPlugin.1" />
                       <param name="DicomOnly" value="No" cmd="-dicomonly 0" />
                     </params>
                   </task>' );

  SET @INSTALLED = @INSTALLED + @@ROWCOUNT

  IF NOT EXISTS ( SELECT * FROM [System].[v3d].Templates WHERE [id] = 'vc-noec-medicsight' ) 
   INSERT INTO [System].[v3d].Templates ( [id], [app], [mod], [desc], [cad], [default], [tasks] ) 
          VALUES (  'vc-noec-medicsight', 'VC', '', @APP + ' with ' + @CAD + ' CAD and  no Electronic Cleansing',  @CAD, 0, 
                    '<task name="pipeline" exe="$(ProcessorDirectory)VC\vcPipeline.exe" log="3" type="Preprocessed" idx="2" max="600" >
                        <params>
                         <param name="ElectronicCleansing" value="No" cmd="-stage allnoec" />
                        </params>
                     </task>
                     <task name="cad" exe="$(ProcessorDirectory)CAD\Viatronix.v3D.Processor.Plugins.CAD.exe" log="3" type="CAD" idx="3" max="600" >
                       <params>
                         <param name="ApplicationId" value="MedicSight" cmd="-appid Medicsight.ColonCADPlugin.1" />
                         <param name="DicomOnly" value="No" cmd="-dicomonly 0" />
                       </params>
                     </task>' );

  SET @INSTALLED = @INSTALLED + @@ROWCOUNT


  IF @INSTALLED > 0
    UPDATE [System].[v3d].[Applications]
            SET [cad] = [cad] + 1
          WHERE [id] = 'VC' 


END
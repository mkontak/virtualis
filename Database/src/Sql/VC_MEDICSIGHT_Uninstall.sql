
--- Delete all VC-iCad Templates
DELETE FROM [System].[v3d].[Templates] WHERE [app] = 'VC' AND [cad] = 'MedicSight'


IF @@ROWCOUNT > 0
  UPDATE [v3d].[Applications]
     SET [cad] = [cad] - 1
   WHERE [id] = 'VC'
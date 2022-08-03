
Changed the config file to contain encrypted database information

    Old:

   <database server="(local)\v3d" ver="1.0" >
     <system value="user id=v3duser;password=qwe321$" />
     <storage value="user id=v3duser;password=qwe321$" />
     <security value="user id=v3dsa;password=jkl789@" />
    </database>


    New:

    <database server="(local)\v3d" ver="1.0" >
      <system value="Ov9XHJP0HmspvoW2QotJBDY5xKwdE9FyorgCy84ck7hkXBibC5zYCsxh6r7mMUN4"/>
      <storage	value="Ov9XHJP0HmspvoW2QotJBDY5xKwdE9FyorgCy84ck7hkXBibC5zYCsxh6r7mMUN4"/>
      <security value="a8thiylGcI0nNbX7+8btwL9MmXY6K0MQ470AGYaWQd4=" />
    </database>

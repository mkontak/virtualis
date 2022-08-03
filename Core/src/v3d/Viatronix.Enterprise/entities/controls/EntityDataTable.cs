using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Data;
using System.Threading.Tasks;

namespace Viatronix.Enterprise.Entities
{

  
  /// <summary>
  /// Entity Data Table
  /// </summary>
  public class EntityDataTable : System.Data.DataTable
  {



    public EntityDataTable(string name) : base(name) { }


    /// <summary>
    /// Fired when the table is loaded
    /// </summary>
    public event EventHandler TableLoaded;

    /// <summary>
    /// Overrides the row creation tp 
    /// </summary>
    /// <param name="builder"></param>
    /// <returns></returns>
    protected override System.Data.DataRow NewRowFromBuilder(System.Data.DataRowBuilder builder)
    {
      return (DataRow)(new EntityDataRow(builder));
    }



    /// <summary>
    /// Loads the list into the table
    /// </summary>
    /// <param name="entities"></param>
    public void Load(List<IEntity> entities, bool readOnly)
    {

      if ( entities.Count > 0 )
      {
        IEntity item = entities[0];

        Type entityType = item.GetType();

        // ================================================================
        // Add all the column's based on the entities public properties
        // ================================================================
        foreach (var propInfo in entityType.GetProperties())
        {
          if (propInfo.Name != "Tag")
          {
            System.Data.DataColumn column = ( propInfo.PropertyType.IsEnum ? 
                                              this.Columns.Add(propInfo.Name, typeof(string) ) :
                                              this.Columns.Add(propInfo.Name, propInfo.PropertyType) );
            column.AllowDBNull = true;
            if (!propInfo.CanWrite || readOnly)
              column.ReadOnly = true;
          }
        } // END ... For each property

        // ========================================================================================
        // Loop though all objects returned and create the corresponding data rows and map entries
        // ==========================================================================================
        foreach (Viatronix.Enterprise.Entities.IEntity entity in entities)
        {



          EntityDataRow row = (EntityDataRow)this.NewRow();
          
          foreach (var propInfo in entityType.GetProperties())
          {
            if (propInfo.Name != "Tag")
            {
              if (propInfo.PropertyType.IsEnum)
                row[propInfo.Name] = propInfo.GetValue(entity, null).ToString();
              else
                row[propInfo.Name] = propInfo.GetValue(entity, null);
              row.Entity = entity;
            }
          }

          this.Rows.Add((DataRow)row);
        } // END ... For each entity

        // Fires the table loaded event
        if (TableLoaded != null)
          TableLoaded(this, EventArgs.Empty);

      } // END ... If the list contains items

    
    } // 


  } // class EntityDataTable



}

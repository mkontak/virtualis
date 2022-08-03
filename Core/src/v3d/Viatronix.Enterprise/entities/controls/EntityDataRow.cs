using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Data;
using System.Threading.Tasks;

namespace Viatronix.Enterprise.Entities
{
  
  /// <summary>
  /// Entity data row 
  /// </summary>
  public class EntityDataRow : System.Data.DataRow
  {
    private IEntity m_entity = null;


    public EntityDataRow(System.Data.DataRowBuilder builder) : base(builder) { }

    public EntityDataRow(IEntity entity, System.Data.DataRowBuilder builder) : base(builder)
    { m_entity = entity; }

    public IEntity Entity
    { 
      get { return m_entity; } 
      set { m_entity = value; }
    }
  } // class EntityDataRow


}

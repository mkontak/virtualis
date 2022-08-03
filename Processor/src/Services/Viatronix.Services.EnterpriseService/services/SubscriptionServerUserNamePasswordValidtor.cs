using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.IdentityModel.Tokens;
using System.IdentityModel.Selectors;

using Viatronix.Enterprise.Data;

namespace Viatronix.Enterprise.Service
{
  class SubscriptionServerUserNamePasswordValidtor : UserNamePasswordValidator
  {

    public override void Validate( string userName, string password )
    {
      if( string.IsNullOrWhiteSpace( userName ) || string.IsNullOrWhiteSpace( password ))
        throw new SecurityTokenException( "Unknown Username or Password" );

      bool hasContext = !password.StartsWith( "0814279F" );
      string id = hasContext ? password : password.Substring( 8 );
      try
      {
        SecurityGateway.Authenticate( userName, (hasContext) ? string.Empty : id, (hasContext) ? id : string.Empty );
      }
      catch( Exception e )
      {
        throw new SecurityTokenException( e.Message );
      }
    }
  }
}

// $Id: vxRendererPop.C,v 1.6 2004/10/05 14:33:12 frank Exp $
//
// Copyright © 2004, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form, 
// in whole or in part, without the prior written permission of Viatronix Inc. 
//
// Owner: Frank Dachille (mailto:frank@viatronix.com)


// includes
#include "stdafx.h"
#include "vxRendererPop.h"
#include "vxElementStack.h"
#include "vxRendererPush.h"
#include "vxEnvironment.h"
#include "vxDataRendererSoftware3d.h"


// defines
#define FILE_REVISION "$Revision: 1.6 $"


/**
 * Default constructor
 * @param environment the environment
 */
vxRendererPop::vxRendererPop( vxEnvironment & environment )
: vxRenderer( environment )
{
} // vxRendererPop()


/**
 * Pre-Render routine
 */
int4 vxRendererPop::PreRender()
{
  PopElements();
  return 1;
} // PreRender()


/**
 * Perform the main rendering function
 */
void vxRendererPop::Render()
{
  PopElements();
} // Render()


/**
 * Post-Render routine
 */
int4 vxRendererPop::PostRender()
{
  PopElements();
  return 1;
} // PostRender()


/**
 * Pop all the elements
 */
void vxRendererPop::PopElements()
{
  //
  // rendering mode
  //

  // check that there is something on the stack
  if (GetElementStack()->empty() == false)
  {

    // restore the rendering mode
    void * pVoid(GetElementStack()->top());
    GetElementStack()->pop();
    GetEnvironment().SetRenderingMode(reinterpret_cast<vxShareableObject<vxRenderingMode> *>(pVoid));

  } // if stack not empty

  GetEnvironment().GetRenderingMode()->SetRenderingAlgorithm(vxRenderingMode::vxRenderingAlgorithmEnum::DEFAULT);


  //
  // renderer CPU 3D data
  //

  // check that there is something on the stack
  if (GetElementStack()->empty() == false)
  {
    // restore the colormap
    void * pVoid(GetElementStack()->top());
    GetElementStack()->pop();
    GetEnvironment().SetDataRenderer(reinterpret_cast<vxShareableObject<vxDataRenderer> *>(pVoid));

  } // if stack not empty


  //
  // colormap
  //

  // check that there is something on the stack
  if ( GetElementStack()->empty() == false )
  {

    // restore the colormap
    void * pVoid = GetElementStack()->top();
    GetElementStack()->pop();
    GetEnvironment().SetColormap( reinterpret_cast< vxShareableObject< vxColormap > * >( pVoid ) );

  } // if stack not empty


  //
  // intensity volume
  //

  // check that there is something on the stack
  if ( GetElementStack()->empty() == false )
  {

    // restore the intensity volume
    void * pVoid = GetElementStack()->top();
    GetElementStack()->pop();
    GetEnvironment().SetIntensityVolume( reinterpret_cast< vxShareableObject<vxBlockVolume<uint2> > * >( pVoid ) );

  } // if stack not empty

} // PopElements()


/**
 * Safely get the element stack from the environment
 *
 * @return the element stack
 */
vxElementStack * vxRendererPop::GetElementStack()
{

  // get the fusion element stack element from the environment
  vxElementStack * pElementStack = dynamic_cast< vxElementStack * >( GetEnvironment().GetElement( vxIDs::FusionElementStack ) );
  if ( pElementStack == NULL )
  {
    // it was not there...add it
    GetEnvironment().AddElement( vxIDs::FusionElementStack, dynamic_cast< vxElement * >( new vxElementStack() ) );

    // get it again to make sure
    pElementStack = dynamic_cast< vxElementStack * >( GetEnvironment().GetElement( vxIDs::FusionElementStack ) );
    if ( pElementStack == NULL )
    {
      // still not there...big problems
      throw ex::AbortOperationException( LogRec( "unable to obtain the fusion element stack element", "vxRendererPush", "PreRender" ) );
    }

  } // if element is valid

  return pElementStack;

} // GetElementStack()


// undefines
#undef FILE_REVISION


// $Log: vxRendererPop.C,v $
// Revision 1.6  2004/10/05 14:33:12  frank
// moving the rendering mode out of the data renderer software 3D class
//
// Revision 1.5  2004/08/30 18:19:37  michael
// Data used by renderers is now polymorph but so far only on the vx side of things.
// Need to add the inheritance on the managed V3D side, too. Still to be done!!!
//
// Revision 1.4  2004/04/06 16:27:32  frank
// unified environment access naming
//
// Revision 1.3  2004/03/30 18:19:45  frank
// saved the rendering mode on the stack also
//
// Revision 1.2  2004/03/26 15:51:16  frank
// now volume, colormap, and cpu data are all replaced
//
// Revision 1.1  2004/03/26 13:49:28  frank
// working on PET fusion
//
// $Header: /CVS/cvsRepository/V3D/src/vxViewer/src/vxRendererPop.C,v 1.6 2004/10/05 14:33:12 frank Exp $
// $Id: vxRendererPop.C,v 1.6 2004/10/05 14:33:12 frank Exp $

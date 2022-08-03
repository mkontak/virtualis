// $Id: vxRendererPush.C,v 1.7 2005/05/24 13:50:02 michael Exp $
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
#include "vxRendererPush.h"
#include "vxElementStack.h"
#include "vxEnvironment.h"
#include "vxDataRendererSoftware3d.h"


// defines
#define FILE_REVISION "$Revision: 1.7 $"


/**
 * Default constructor
 * @param environment the environment
 */
vxRendererPush::vxRendererPush( vxEnvironment & environment )
: vxRenderer( environment )
{
} // vxRendererPush()


/**
 * Pre-Render routine
 *
 * @return unused
 */
int4 vxRendererPush::PreRender()
{
  PushElements();
  return 1;
} // PreRender()


/**
 * Perform the main rendering function
 */
void vxRendererPush::Render()
{
  PushElements();
} // Render()


/**
 * Post-Render routine
 */
int4 vxRendererPush::PostRender()
{
  PushElements();
  return 1;
} // PostRender()


/**
 * push all the elements
 */
void vxRendererPush::PushElements()
{

  //
  // intensity volume
  //

  // push the current volume onto the stack
  GetElementStack()->push( static_cast< void * >( GetEnvironment().GetIntensityVolume() ) );

  // get the new volume
  vxShareableObject< vxBlockVolume<uint2> > * pSecondaryVolume = dynamic_cast< vxShareableObject< vxBlockVolume<uint2> > * >( GetEnvironment().GetElement( vxIDs::FusionVolume ) );
  if ( pSecondaryVolume == NULL )
  {
    pSecondaryVolume = GetEnvironment().GetIntensityVolume();
  }

  // set the new volume
  GetEnvironment().SetIntensityVolume( pSecondaryVolume );


  //
  // colormap
  //

  // push the current colormap onto the stack
  GetElementStack()->push( static_cast< void * >( GetEnvironment().GetColormap() ) );

  // get the new colormap
  vxShareableObject< vxColormap > * pSecondaryColormap = dynamic_cast< vxShareableObject< vxColormap > * >( GetEnvironment().GetElement( vxIDs::FusionColormap ) );
  if ( pSecondaryColormap == NULL )
  {
    pSecondaryColormap = GetEnvironment().GetColormap();
  }

  // set the new colormap
  GetEnvironment().SetColormap( pSecondaryColormap );


  //
  // renderer CPU 3D data
  //

  // push the current data onto the stack
  GetElementStack()->push(static_cast<void *>(GetEnvironment().GetDataRenderer()));

  // get the new data
  vxShareableObject<vxDataRendererSoftware3d> * pSecondaryData = dynamic_cast<vxShareableObject<vxDataRendererSoftware3d> *>(GetEnvironment().GetElement(vxIDs::FusionRenderer3DData));
  if (pSecondaryData == NULL)
  {
    pSecondaryData = dynamic_cast<vxShareableObject<vxDataRendererSoftware3d> *>(GetEnvironment().GetDataRenderer());
  }

  // set the new data
  GetEnvironment().SetDataRenderer(reinterpret_cast<vxShareableObject<vxDataRenderer> *>(pSecondaryData));


  //
  // rendering mode
  //

  // push the current rendering mode onto the stack
  GetElementStack()->push( static_cast< void * >( GetEnvironment().GetRenderingMode() ) );

  // get the new rendering mode
  vxShareableObject< vxRenderingMode > * pSecondaryRenderingMode = dynamic_cast< vxShareableObject< vxRenderingMode > * >( GetEnvironment().GetElement( vxIDs::FusionRenderingMode ) );
  if ( pSecondaryRenderingMode == NULL )
  {
    pSecondaryRenderingMode = GetEnvironment().GetRenderingMode();
  }

  // set the new rendering mode
  GetEnvironment().SetRenderingMode( pSecondaryRenderingMode );

  GetEnvironment().GetRenderingMode()->SetRenderingAlgorithm( vxRenderingMode::vxRenderingAlgorithmEnum::MIP );


} // PushElements()


/**
 * Safely get the element stack from the environment
 *
 * @return the element stack
 */
vxElementStack * vxRendererPush::GetElementStack()
{

  // get the fusion element stack element from the environment
  vxElementStack * pElementStack = dynamic_cast< vxElementStack * >( GetEnvironment().GetElement( vxIDs::FusionElementStack ) );
  if ( pElementStack == NULL )
  {
    // it was not there...add it
    GetEnvironment().AddElement( vxIDs::FusionElementStack, dynamic_cast< vxElement * >( new vxElementStack() ) );

    // get it again to make sure
    pElementStack = dynamic_cast< vxElementStack * >( GetEnvironment().GetElement(  vxIDs::FusionElementStack ) );
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


// $Log: vxRendererPush.C,v $
// Revision 1.7  2005/05/24 13:50:02  michael
// Introduced concept of data objects to start with v3D_Data..../vxData...
// Has been done for DataColormapEditor, DataAnnotationCreate, and
// DataPlaneObliqueCreate
//
// Revision 1.6  2004/10/05 14:33:12  frank
// moving the rendering mode out of the data renderer software 3D class
//
// Revision 1.5  2004/08/30 18:19:37  michael
// Data used by renderers is now polymorph but so far only on the vx side of things.
// Need to add the inheritance on the managed V3D side, too. Still to be done!!!
//
// Revision 1.4  2004/04/06 16:27:40  frank
// unified environment access naming
//
// Revision 1.3  2004/03/30 18:19:45  frank
// saved the rendering mode on the stack also
//
// Revision 1.2  2004/03/26 15:51:17  frank
// now volume, colormap, and cpu data are all replaced
//
// Revision 1.1  2004/03/26 13:49:28  frank
// working on PET fusion
//
// $Header: /CVS/cvsRepository/V3D/src/vxViewer/src/vxRendererPush.C,v 1.7 2005/05/24 13:50:02 michael Exp $
// $Id: vxRendererPush.C,v 1.7 2005/05/24 13:50:02 michael Exp $

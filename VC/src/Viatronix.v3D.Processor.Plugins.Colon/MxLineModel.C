/************************************************************************

  MxLineModel

  Copyright (C) 1998 Michael Garland.  See "COPYING.txt" for details.
  
  $Id: MxLineModel.C,v 1.1 2005/04/14 12:50:05 geconomos Exp $

 ************************************************************************/

#include "stdmix.h"
#include "MxLineModel.h"


MxVertexID MxLineModel::add_vertex(float4 x, float4 y)
{
    MxVertexID id = vertices.add(MxVertex2D(x,y));
    return id;
}

MxFaceID MxLineModel::add_segment(MxVertexID v1, MxVertexID v2)
{
    MxFaceID id = segments.add(MxSegment2D(v1, v2));
    return id;
}

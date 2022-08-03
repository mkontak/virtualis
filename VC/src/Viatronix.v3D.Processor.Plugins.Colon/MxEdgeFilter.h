#ifndef MXEDGEFILTER_INCLUDED // -*- C++ -*-
#define MXEDGEFILTER_INCLUDED

/************************************************************************

  MxEdgeFilter

  Copyright (C) 1998 Michael Garland.  See "COPYING.txt" for details.
  
  $Id: MxEdgeFilter.h,v 1.1 2005/04/14 12:50:05 geconomos Exp $

 ************************************************************************/

#include "MxStdModel.h"
#include "MxHeap.h"

class MxRankedEdge : public MxEdge, public MxHeapable
{
public:
    MxRankedEdge(MxVertexID v1, MxVertexID v2) : MxEdge(v1,v2) { }
};

class MxEdgeFilter 
{
private:
    MxHeap heap;
    MxDynBlock<MxRankedEdge *> update_list;

    bool filter1();

protected:
    MxStdModel *m;

    MxRankedEdge *create_edge(MxVertexID, MxVertexID, bool will_rank=true);
    void collect_edges();
    void collect_star_for_update(MxVertexID);
    void rank_and_update_edge(MxRankedEdge *);

    MxVertexID split_edge(MxRankedEdge *);

    virtual float4 compute_edge_rank(MxRankedEdge *);
    virtual void filter_target_edge(MxRankedEdge *);

public:
    uint4 original_edge_count;
    uint4 current_edge_count;

public:
    MxEdgeFilter(MxStdModel *m0);

    void initialize();
    bool filter(uint4 target);
    bool filter_above_rank(float4);
};

// MXEDGEFILTER_INCLUDED
#endif

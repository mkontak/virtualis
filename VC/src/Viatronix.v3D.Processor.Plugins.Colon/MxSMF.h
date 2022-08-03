#ifndef MXSMF_INCLUDED // -*- C++ -*-
#define MXSMF_INCLUDED
#if !defined(__GNUC__)
#  pragma once
#endif

/************************************************************************

  MxSMF

  Copyright (C) 1998 Michael Garland.  See "COPYING.txt" for details.
  
  $Id: MxSMF.h,v 1.1 2005/04/14 12:50:05 geconomos Exp $

 ************************************************************************/


// Interim reader based on SMF 1.0
// This reader is designed to create MxStdModel models

#include "MxCmdParser.h"
#include "MxStack.h"
#include "MxStdModel.h"
#include "MxMat4.h"

class MxSMFReader : private MxCmdParser
{
public:
  typedef void (MxSMFReader::*read_cmd)(int4 argc, int1 *argv[], MxStdModel& m);
  struct cmd_entry { int1 *name; read_cmd cmd; };

private:
  MxStack<uint4> vfirst_stack;
  MxStack<int4>  vcorrect_stack;
  MxStack<Mat4> vx_stack;
  MxStack<Mat4> tx_stack;

  uint4 next_vertex;
  uint4 next_face;
  uint4 quad_count;	// # of quadrilaterals in input
  uint4 poly_count;	// # of 5+ sided polygons in input
  int1 *line_buffer;
  static cmd_entry read_cmds[];

private:
  bool execute_command(const MxCmd&, void *closure);
  void define_avars();
  void update_avars();

protected:
  void v_xform(Vec3& v);
  void t_xform(Vec2& v);
  uint4 vid_xform(int4 id);

  void vertex(int4 argc, int1 *argv[], MxStdModel&);
  void face(int4 argc, int1 *argv[], MxStdModel&);

  void prop_bind(int4 argc, int1 *argv[], MxStdModel&);
  void prop_normal(int4 argc, int1 *argv[], MxStdModel&);
  // void prop_color(int4 argc, int1 *argv[], MxStdModel&);
  // void prop_texcoord(int4 argc, int1 *argv[], MxStdModel&);
  // void read_texture(int4 argc, int1 *argv[], MxStdModel&);

  void begin(int4 argc, int1 *argv[], MxStdModel&);
  void end(int4 argc, int1 *argv[], MxStdModel&);
  void rot(int4 argc, int1 *argv[], MxStdModel&);
  void trans(int4 argc, int1 *argv[], MxStdModel&);
  void scale(int4 argc, int1 *argv[], MxStdModel&);
  void t_trans(int4 argc, int1 *argv[], MxStdModel&);
  void t_scale(int4 argc, int1 *argv[], MxStdModel&);

public:
  MxSMFReader();
  ~MxSMFReader();

  bool (*unparsed_hook)(int1 *cmd, int4 argc, int1 *argv[], MxStdModel& m);
  MxAspStore *asp_store() { return this->MxCmdParser::asp_store(); }
  MxStdModel *read(istream& in, MxStdModel *model=NULL);
  MxStdModel *read(MxStdModel *model=NULL, int1 *Buf=NULL);
#ifndef MIX_ANSI_IOSTREAMS
  MxStdModel *read(int4 fd, MxStdModel *model=NULL);
#endif
  void parse_line(int1 *, MxStdModel&);
};

class MxSMFWriter 
{
public:
  MxSMFWriter();
  
  void (*vertex_annotate)(ostream&, const MxStdModel&, MxVertexID);
  void (*face_annotate)(ostream&, const MxStdModel&, MxVertexID);
  void write(ostream& out, MxStdModel& m);
  int1 * write(MxStdModel& m, int4 * num_vertex, int4 * num_normal, int4 * num_face);
#ifndef MIX_ANSI_IOSTREAMS
  void write(int4 fd, MxStdModel& m);
#endif
};

// MXSMF_INCLUDED
#endif
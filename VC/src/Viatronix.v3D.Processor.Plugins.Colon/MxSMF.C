/************************************************************************

  MxSMF

  Copyright (C) 1998 Michael Garland.  See "COPYING.txt" for details.
  
  $Id: MxSMF.C,v 1.1 2005/04/14 12:50:05 geconomos Exp $

 ************************************************************************/

#include "stdmix.h"
#include "MxSMF.h"
#include "MxVector.h"
#ifdef __linux__
  #include <unistd.h> // dup
  #include <stdio.h>
#endif

#define SMF_MAXLINE 65536


MxSMFReader::cmd_entry MxSMFReader::read_cmds[] = {
    { "v", &MxSMFReader::vertex },
    { "t", &MxSMFReader::face },
    { "f", &MxSMFReader::face },

    // Properties and binding -- this is the new way to handle things
    { "bind", &MxSMFReader::prop_bind },
    { "n", &MxSMFReader::prop_normal },
		// { "c", &MxSMFReader::prop_color },
		// { "r", &MxSMFReader::prop_texcoord },

    { "begin", &MxSMFReader::begin },
    { "end", &MxSMFReader::end },

    { "trans", &MxSMFReader::trans },
    { "rot", &MxSMFReader::rot },
    { "scale", &MxSMFReader::scale },

    // { "tex", &MxSMFReader::read_texture },
    { "t_trans", &MxSMFReader::t_trans },
    { "t_scale", &MxSMFReader::t_scale },

    { NULL, NULL }
};


MxSMFReader::MxSMFReader()
    : vx_stack(Mat4::I, 2), tx_stack(Mat4::I, 2),
      vcorrect_stack(0, 2), vfirst_stack(1, 2)
{
  next_vertex=1;
  next_face=1;
  quad_count=0;
  poly_count=0;
  line_buffer = new int1[SMF_MAXLINE];
  unparsed_hook = NULL;

  define_avars();
}

MxSMFReader::~MxSMFReader()
{
  delete[] line_buffer;
}

void MxSMFReader::v_xform(Vec3& v)
{
  Vec4 v2 = vx_stack.top() * Vec4(v,1);

  v[X] = v2[X]/v2[W];
  v[Y] = v2[Y]/v2[W];
  v[Z] = v2[Z]/v2[W];
}

void MxSMFReader::t_xform(Vec2& v)
{
  Vec4 v2 = tx_stack.top() * Vec4(v[X],v[Y],0,1);

  v[X] = v2[X]/v2[W];
  v[Y] = v2[Y]/v2[W];
}

uint4 MxSMFReader::vid_xform(int4 id)
{
  if( id < 0 )
    id += next_vertex;
  else
    id += vcorrect_stack.top() + (vfirst_stack.top() - 1);

  return id;
}

////////////////////////////////////////////////////////////////////////

void MxSMFReader::define_avars()
{
  asp_store()->defvar("vertex_correction", MXASP_INT, &vcorrect_stack.top());
}

void MxSMFReader::update_avars()
{
  MxAspVar *avar;

  avar = asp_store()->lookup("vertex_correction");
  if ( avar )
    avar->bind("vertex_correction", MXASP_INT, &vcorrect_stack.top());
}

void MxSMFReader::begin(int4, int1 *[], MxStdModel&)
{
  vx_stack.push();
  tx_stack.push();
  vfirst_stack.push(next_vertex);
  vcorrect_stack.push();
  update_avars();
}

void MxSMFReader::end(int4, int1 *[], MxStdModel&)
{
  vx_stack.pop();
  tx_stack.pop();
  vfirst_stack.pop();
  vcorrect_stack.pop();
  update_avars();
}

void MxSMFReader::trans(int4, int1 *argv[], MxStdModel&)
{
  Mat4 M = Mat4::trans(atof(argv[0]), atof(argv[1]), atof(argv[2]));
  vx_stack.top() = vx_stack.top() * M;
}

void MxSMFReader::rot(int4, int1 *argv[], MxStdModel&)
{
  Mat4 M;
  switch( argv[0][0] )
  {
  case 'x':
    M = Mat4::xrot(atof(argv[1]) * M_PI/180.0);
    break;
  case 'y': 
    M = Mat4::yrot(atof(argv[1]) * M_PI/180.0);
    break;
  case 'z':
    M = Mat4::zrot(atof(argv[1]) * M_PI/180.0);
    break;
  default:
    MxMsg::signal(MXMSG_WARN, "Malformed SMF rotation command.");
    break;
  }
  vx_stack.top() = vx_stack.top() * M;
}

void MxSMFReader::scale(int4, int1 *argv[], MxStdModel&)
{
  Mat4 M = Mat4::scale(atof(argv[0]), atof(argv[1]), atof(argv[2]));
  vx_stack.top() = vx_stack.top() * M;
}

void MxSMFReader::t_trans(int4, int1 *argv[], MxStdModel&)
{
  Mat4 M = Mat4::trans(atof(argv[0]), atof(argv[1]), 0.0);
  tx_stack.top() = tx_stack.top() * M;
}

void MxSMFReader::t_scale(int4, int1 *argv[], MxStdModel&)
{
  Mat4 M = Mat4::scale(atof(argv[0]), atof(argv[1]), 1.0);
  tx_stack.top() = tx_stack.top() * M;
}

void MxSMFReader::vertex(int4, int1 *argv[], MxStdModel& m)
{
  Vec3 v(atof(argv[0]), atof(argv[1]), atof(argv[2]));

  v_xform(v);
  next_vertex++;
  m.add_vertex((float4)v[X], (float4)v[Y], (float4)v[Z]);
}

void MxSMFReader::prop_bind(int4 argc, int1 *argv[], MxStdModel& m)
{
  if ( argc<2 )  return;	// Not enough arguments

  switch( argv[0][0] )
  {
    case 'n': m.normal_binding(m.parse_binding(argv[1])); break;
    case 'c': m.color_binding(m.parse_binding(argv[1])); break;
    case 'r': m.texcoord_binding(m.parse_binding(argv[1])); break;
  }
}

void MxSMFReader::prop_normal(int4 /*argc*/, int1 *argv[], MxStdModel& m)
{
  Vec3 n(atof(argv[0]), atof(argv[1]), atof(argv[2]));
  //v_xform(n);  //!!BUG: Need to transform normals appropriately
  unitize(n);
  m.add_normal((float4)n[X], (float4)n[Y], (float4)n[Z]);
}

void MxSMFReader::face(int4 argc, int1 *argv[], MxStdModel& m)
{
  if( argc == 3 )
  {
    uint4 v0 = vid_xform(atoi(argv[0]));
    uint4 v1 = vid_xform(atoi(argv[1]));
    uint4 v2 = vid_xform(atoi(argv[2]));

    next_face++;
    m.add_face(v0 - 1, v1 - 1, v2 - 1);
  }
  else if ( argc == 4 )
  {
    cout << "reached here" << endl;
    
    // we only have triangles at this stage! If not, ignore it! 
    MxMsg::signalf(MXMSG_WARN, "Input polygon #%d has more than 3 sides. Ignoring it.", next_face);
    return;

    if( !quad_count ) 
    {
      MxMsg::signalf(MXMSG_NOTE, "Input polygon #%d is a quadrilateral. Splitting it.", next_face);
    }

    quad_count++;
    uint4 v[4];
    v[0] = vid_xform(atoi(argv[0])) - 1;
    v[1] = vid_xform(atoi(argv[1])) - 1;
    v[2] = vid_xform(atoi(argv[2])) - 1;
    v[3] = vid_xform(atoi(argv[3])) - 1;

    float4 e[4][3];
    mxv_unitize(mxv_sub(e[0], m.vertex(v[1]), m.vertex(v[0]), 3), 3);
    mxv_unitize(mxv_sub(e[1], m.vertex(v[2]), m.vertex(v[1]), 3), 3);
    mxv_unitize(mxv_sub(e[2], m.vertex(v[3]), m.vertex(v[2]), 3), 3);
    mxv_unitize(mxv_sub(e[3], m.vertex(v[0]), m.vertex(v[3]), 3), 3);

    float4 a_02 = (1-mxv_dot(e[0], e[3], 3)) + (1-mxv_dot(e[1], e[2], 3));
    float4 a_13 = (1-mxv_dot(e[1], e[0], 3)) + (1-mxv_dot(e[3], e[2], 3));

    if( a_02 <= a_13 )
    {
      next_face++;  m.add_face(v[0], v[1], v[2]);
      next_face++;  m.add_face(v[0], v[2], v[3]);
    }
    else
    {
      next_face++;  m.add_face(v[0], v[1], v[3]);
      next_face++;  m.add_face(v[1], v[2], v[3]);
    }
  }
  else
  {
    MxMsg::signalf(MXMSG_WARN, "Input polygon #%d has more than 4 sides.  Ignoring it.", next_face);
	  poly_count++;
  }
}

////////////////////////////////////////////////////////////////////////

bool MxSMFReader::execute_command(const MxCmd& cmd, void *closure)
{
  MxStdModel& m = *(MxStdModel *)closure;
  int4 argc = (*(cmd.phrases)).length();
  int1 **argv = (int1 **)&cmd.phrases[0][0];
  cmd_entry *entry = &read_cmds[0];
  bool handled = false;

  while ( entry->name && !handled )
  {
    if( streq(entry->name, cmd.op) )
    {
      (this->*(entry->cmd))(argc, argv, m);
      handled = true;
    }
    else
      entry++;
  }

  if ( !handled ) 
  {
    if ( !unparsed_hook || unparsed_hook && !(*unparsed_hook)(cmd.op, argc, argv, m) )
    {
      return false;
    }
  }

  return true;
}

void MxSMFReader::parse_line(int1 *line, MxStdModel& m)
{
  MxCmdParser::parse_line(line, &m);
}


MxStdModel *MxSMFReader::read(MxStdModel *m, int1 * Buf)
{
  if (!m)
  m = new MxStdModel(8, 16);

  begin(0, NULL, *m);
  int1 * Ptr = Buf;
  do 
  {
    if (Ptr[0]=='#' && Ptr[1]=='#') break;
    for (int4 i=0; i<28; i++) {
      line_buffer[i] = Ptr[i];
    }
    parse_line(line_buffer, *m);
    Ptr += 28;
  } while (true);
  end(0, NULL, *m);

  if ( quad_count )
    MxMsg::signalf(MXMSG_WARN,
          "Split %d input quadrilaterals.  "
          "Auto-splitting does not preserve properties!", quad_count);
  if ( poly_count )
    MxMsg::signalf(MXMSG_WARN,
          "Ignored %d input polygons of more than 4 sides.", poly_count);

  return m;
}


MxStdModel *MxSMFReader::read(istream& in, MxStdModel *m)
{
  if( !m )
  m = new MxStdModel(8, 16);

  begin(0, NULL, *m);
  while( !in.eof()) {
    in >> ws;
    if( in.peek() == '#' )
      in.ignore(SMF_MAXLINE, '\n');
    else if( in.getline(line_buffer, SMF_MAXLINE, '\n').good() ) 
    {
      parse_line(line_buffer, *m);
    }
  }
  end(0, NULL, *m);

  if( quad_count )
    MxMsg::signalf(MXMSG_WARN,
          "Split %d input quadrilaterals.  "
          "Auto-splitting does not preserve properties!",
          quad_count);
  if( poly_count )
    MxMsg::signalf(MXMSG_WARN,
          "Ignored %d input polygons of more than 4 sides.",
          poly_count);

  return m;
}

#ifndef MIX_ANSI_IOSTREAMS
MxStdModel *MxSMFReader::read(int4 fd, MxStdModel *m)
{
  ifstream in(dup(fd));	// Duplicate handle will be closed on exit
  return read(in, m);
}
#endif



MxSMFWriter::MxSMFWriter()
{
  vertex_annotate = NULL;
  face_annotate = NULL;
}

void MxSMFWriter::write(ostream& out, MxStdModel& m)
{
  uint4 i;
  out << "begin" << endl;

  for (uint4 v=0; v<m.vert_count(); v++) 
  {
    // We must output all vertices since numbering is implicit
    if( vertex_annotate )  (*vertex_annotate)(out, m, v);
    out << m.vertex(v) << endl;
  }

  for (uint4 f=0; f<m.face_count(); f++)
    if( m.face_is_valid(f) ) 
    {
      if( face_annotate ) (*face_annotate)(out, m, f);
      out << m.face(f) << endl;
    }

  if( m.normal_binding() != MX_UNBOUND ) 
  {
    out << "bind n " << m.binding_name(m.normal_binding()) << endl;
    for(i=0; i<m.normal_count(); i++) 
    {
      out << m.normal(i) << endl;
    }
  }

  if( m.color_binding() != MX_UNBOUND ) 
  {
    out << "bind c " << m.binding_name(m.color_binding()) << endl;
    for (i=0; i<m.color_count(); i++)
      out << m.color(i) << endl;
  }

  out << "end" << endl;
}

// write out to in-memory buffer.
int1 * MxSMFWriter::write(MxStdModel& m, int4 * num_vertex, int4 * num_normal, int4 * num_face)
{
  int1 *bufPtr;
  bufPtr = new int1 [(m.vert_count() + m.face_count() + m.normal_count() + 10) * 64];
  if (bufPtr == NULL) 
  {
    return NULL;
  }
  int1 *ptr = bufPtr;

  sprintf(ptr, "%s", "begin\n");	ptr += 64;
  *num_vertex = m.vert_count();
  for (uint4 v=0; v<m.vert_count(); v++) 
  {
    sprintf(ptr, "%c %.3f %.3f %.3f\n", 'v', m.vertex(v)(0), m.vertex(v)(1), m.vertex(v)(2));
    ptr += 64;
  }

  * num_face = 0;
  for (uint4 f=0; f<m.face_count(); f++)
    if( m.face_is_valid(f) ) 
    {
      * num_face += 1;
      sprintf(ptr, "%c %d %d %d\n", 'f', m.face(f)(0)+1, m.face(f)(1)+1, m.face(f)(2)+1);
      ptr += 64;
    }

  *num_normal = m.normal_count();
  if( m.normal_binding() != MX_UNBOUND ) 
  {
    sprintf(ptr, "%s", "bind n vertex\n");	
    ptr += 64;
    for (uint4 i=0; i<m.normal_count(); i++) 
    {
      sprintf(ptr, "%c %.3f %.3f %.3f\n", 'n', m.normal(i)[0], m.normal(i)[1], m.normal(i)[2]);
      ptr += 64;
    }
  }
  sprintf(ptr, "%s", "end\n");
  sprintf(ptr, "##\n");

  return bufPtr;
}

#ifndef MIX_ANSI_IOSTREAMS
void MxSMFWriter::write(int4 fd, MxStdModel& m)
{
  ofstream out(dup(fd));	// Duplicate handle will be closed on exit
  write(out, m);
}
#endif
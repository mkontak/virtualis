/************************************************************************

  MxRaster

  Copyright (C) 1998 Michael Garland.  See "COPYING.txt" for details.
  
  $Id: MxRaster.C,v 1.1 2005/04/14 12:50:05 geconomos Exp $

 ************************************************************************/

#include "stdmix.h"
#include "MxRaster.h"


////////////////////////////////////////////////////////////////////////
//
// Raster manipulation
//

// !!BUG: This could probably be made significantly faster.
//
void MxRaster::reverse(uint4 start, uint4 end)
{
    if(end >= length()) end = length() - channels();

    uint4 i=start;
    uint4 j=end;
    uint1 t;
    
    while(i<j)
    {
	for(uint4 k=0; k<channels(); k++)
	{
	    t=(*this)[i+k];
	    (*this)[i+k] = (*this)[j+k];
	    (*this)[j+k] = t;
	}
	i += channels();
	j -= channels();
    }
}

void MxRaster::hflip()
{
     uint4 i = 0;
     uint4 j = channels()*(width()-1);

     while( i<length() )
     {
	 reverse(i, i+j);
	 i += j + channels();
     }
}

void MxRaster::vflip()
{
    reverse();
    hflip();
}


////////////////////////////////////////////////////////////////////////
//
// PNM output
//
/*
ostream& pnm_write_header(ostream& out, const MxRaster& r, bool is_raw)
{
    return out << "P" << (is_raw?r.channels()*2:r.channels())
	       << " " << r.width() << " " << r.height() << " 255" << endl;
}
*/
/*
ostream& pnm_write_data(ostream& out, const MxRaster& r, bool is_raw)
{
    if( is_raw )
	out.write((const int1 *)(const uint1 *)r, r.length());
    else
    {
	for(uint4 i=0; i<r.length(); i++)
	{
	    out << (uint4)(r[i]);
	    if( i%6 ) out << " ";
	    else      out << endl;
	}
    }

    return out;
}
*/
////////////////////////////////////////////////////////////////////////
//
// PNM input
//

/*
static
istream& pnm_skip(istream& in)
{
    for(;;)
    {
	in >> ws;
	if( in.peek() == '#' )
	    in.ignore(65535, '\n');
	else
	    return in;
    }
}
*/
/*
static
void pnm_read_ascii(istream& in, MxRaster& img)
{
    uint1 *current = img;
    uint4 val;

    for(uint4 j=0; j<img.height(); j++) for(uint4 i=0; i<img.width(); i++)
        for(uint4 k=0; k<img.channels(); k++)
        {
            pnm_skip(in) >> val;
            *current++ = (uint1)val;
        }
}
*/
/*
static
void pnm_read_ascii(istream& in, MxRaster& img, float4 scale)
{
    uint1 *current = img;
    float4 val;

    for(uint4 j=0; j<img.height(); j++) for(uint4 i=0; i<img.width(); i++)
        for(uint4 k=0; k<img.channels(); k++)
        {
            pnm_skip(in) >> val;
            *current++ = (uint1)(val*scale);
        }
}
*/
/*
static
void pnm_read_raw(istream& in, MxRaster& img)
{
    int1 c;  in.get(c);  // extract 1 whitespace int1acter

    //
    // Is this guaranteed to read all the requested bytes?
    //
    in.read((int1 *)(uint1 *)img, img.length());
}

MxRaster *pnm_read(istream& in)
{
    uint1 P, N;

    in >> P >> N;

    if( P!='P' )  return NULL;

    uint4 width, height, maxval;
    pnm_skip(in) >> width;
    pnm_skip(in) >> height;
    pnm_skip(in) >> maxval;

    bool is_raw = false;
    uint4 channels = N - '0';
    if( channels > 3 )
    {
	channels /= 2;
	is_raw = true;
    }

    if( channels > 3 )  return NULL;

    MxRaster *img = new MxRaster(width, height, channels);

    if( is_raw )
    {
	if( maxval>255 )  return NULL;
	if( maxval < 255 )
	    MxMsg::signal(MXMSG_WARN, "Ignoring scaled raw PNM data.");

	pnm_read_raw(in, *img);
    }
    else if( maxval==255 )
	pnm_read_ascii(in, *img);
    else
	pnm_read_ascii(in, *img, 255.0f/(float4)maxval);

    return img;
}
*/
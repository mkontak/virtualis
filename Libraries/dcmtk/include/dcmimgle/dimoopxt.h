/*
 *
 *  Copyright (C) 1996-2010, OFFIS e.V.
 *  All rights reserved.  See COPYRIGHT file for details.
 *
 *  This software and supporting documentation were developed by
 *
 *    OFFIS e.V.
 *    R&D Division Health
 *    Escherweg 2
 *    D-26121 Oldenburg, Germany
 *
 *
 *  Module:  dcmimgle
 *
 *  Author:  Joerg Riesmeier
 *
 *  Purpose: DicomMonoOutputPixelTemplate (Header)
 *
 *  Last Update:      $Author: joergr $
 *  Update Date:      $Date: 2010-10-28 10:58:38 $
 *  CVS/RCS Revision: $Revision: 1.54 $
 *  Status:           $State: Exp $
 *
 *  CVS/RCS Log at end of file
 *
 */


#ifndef DIMOOPXT_H
#define DIMOOPXT_H

#include "config/osconfig.h"

#include "ofstd/ofcast.h"
#include "ofstd/ofbmanip.h"

#include "dcmimgle/dimoopx.h"
#include "dcmimgle/dimopx.h"
#include "dcmimgle/diluptab.h"
#include "dcmimgle/diovlay.h"
#include "dcmimgle/dipxrept.h"
#include "dcmimgle/didispfn.h"
#include "dcmimgle/didislut.h"

#ifdef PASTEL_COLOR_OUTPUT
#include "dimcopxt.h"
#endif

#define INCLUDE_CMATH
#include "ofstd/ofstdinc.h"


/*---------------------*
 *  class declaration  *
 *---------------------*/

/** Template class to create monochrome output data
 */
template<class T1, class T2, class T3>
class DiMonoOutputPixelTemplate
  : public DiMonoOutputPixel,
    public DiPixelRepresentationTemplate<T3>
{

 public:

    /** constructor
     *
     ** @param  buffer    storage area for the output pixel data (optional, maybe NULL)
     *  @param  pixel     pointer to intermediate pixel representation
     *  @param  overlays  array of overlay management objects
     *  @param  vlut      VOI LUT (optional, maybe NULL)
     *  @param  plut      presentation LUT (optional, maybe NULL)
     *  @param  disp      display function (optional, maybe NULL)
     *  @param  vfunc     VOI LUT function (optional)
     *  @param  center    window center (optional, invalid if 'width' < 1)
     *  @param  width     window width (optional, invalid if < 1)
     *  @param  low       lowest pixel value for the output data (e.g. 0)
     *  @param  high      highest pixel value for the output data (e.g. 255)
     *  @param  columns   image's width (in pixels)
     *  @param  rows      image's height
     *  @param  frame     frame to be rendered
     * (#)param frames    total number of frames present in intermediate representation
     *  @param  pastel    flag indicating whether to use not only 'real' grayscale values (optional, experimental)
     */
    DiMonoOutputPixelTemplate(void *buffer,
                              const DiMonoPixel *pixel,
                              DiOverlay *overlays[2],
                              const DiLookupTable *vlut,
                              const DiLookupTable *plut,
                              DiDisplayFunction *disp,
                              const EF_VoiLutFunction vfunc,
                              const double center,
                              const double width,
                              const Uint32 low,
                              const Uint32 high,
                              const Uint16 columns,
                              const Uint16 rows,
                              const unsigned long frame,
#ifdef PASTEL_COLOR_OUTPUT
                              const unsigned long frames,
#else
                              const unsigned long /*frames*/,
#endif
                              const int pastel = 0)
      : DiMonoOutputPixel(pixel, OFstatic_cast(unsigned long, columns) * OFstatic_cast(unsigned long, rows), frame,
                          OFstatic_cast(unsigned long, fabs(OFstatic_cast(double, high - low)))),
        Data(NULL),
        DeleteData(buffer == NULL),
        ColorData(NULL)
    {
        if ((pixel != NULL) && (Count > 0) && (FrameSize >= Count))
        {
            if (pastel)
#ifdef PASTEL_COLOR_OUTPUT
                color(buffer, pixel, frame, frames);
#else
                DCMIMGLE_ERROR("pastel color output not supported");
#endif
            else
            {
                DCMIMGLE_TRACE("monochrome output image - columns: " << columns << ", rows: " << rows << ", frame: " << frame);
                DCMIMGLE_TRACE("monochrome output values - low: " << OFstatic_cast(unsigned long, low) << ", high: "
                    << OFstatic_cast(unsigned long, high) << ((low > high) ? " (inverted)" : ""));
                Data = OFstatic_cast(T3 *, buffer);
                if ((vlut != NULL) && (vlut->isValid()))            // valid VOI LUT ?
                    voilut(pixel, frame * FrameSize, vlut, plut, disp, OFstatic_cast(T3, low), OFstatic_cast(T3, high));
                else
                {
                    if (width < 1)                                  // no valid window according to supplement 33
                        nowindow(pixel, frame * FrameSize, plut, disp, OFstatic_cast(T3, low), OFstatic_cast(T3, high));
                    else if (vfunc == EFV_Sigmoid)
                        sigmoid(pixel, frame * FrameSize, plut, disp, center, width, OFstatic_cast(T3, low), OFstatic_cast(T3, high));
                    else // linear
                        window(pixel, frame * FrameSize, plut, disp, center, width, OFstatic_cast(T3, low), OFstatic_cast(T3, high));
                }
                overlay(overlays, disp, columns, rows, frame);      // add (visible) overlay planes to output bitmap
            }
        }
    }

    /** destructor
     */
    virtual ~DiMonoOutputPixelTemplate()
    {
        if (DeleteData)
            delete[] Data;
        delete ColorData;
    }

    /** get integer representation of output data
     *
     ** @return integer representation
     */
    inline EP_Representation getRepresentation() const
    {
        return DiPixelRepresentationTemplate<T3>::getRepresentation();
    }

    /** get size of one pixel / item in the pixel array
     *
     ** @return item size
     */
    inline size_t getItemSize() const
    {
        return (ColorData != NULL) ? ColorData->getItemSize() : sizeof(T3);
    }

    /** get pointer to output pixel data
     *
     ** @return pointer to pixel data
     */
    inline const void *getData() const
    {
        return (ColorData != NULL) ? ColorData->getData() : OFstatic_cast(const void *, Data);
    }

    /** get pointer to output pixel data
     *
     ** @return pointer to pixel data
     */
    virtual void *getDataPtr()
    {
        return (ColorData != NULL) ? ColorData->getDataPtr() : OFstatic_cast(void *, Data);
    }

    /** remove reference to (internally handled) pixel data (abstract)
     */
    inline void removeDataReference()
    {
        Data = NULL;
        DeleteData = 0;
    }

    /** write pixel data of selected frame to PPM/ASCII file
     *
     ** @param  stream  open C++ output stream
     *
     ** @return status, true if successful, false otherwise
     */
    inline int writePPM(STD_NAMESPACE ostream& stream) const
    {
        if (Data != NULL)
        {
            register unsigned long i;
            for (i = 0; i < FrameSize; ++i)
                stream << OFstatic_cast(unsigned long, Data[i]) << " ";    // typecast to resolve problems with 'char'
            return 1;
        }
        if (ColorData != NULL)
            return ColorData->writePPM(stream);
        return 0;
    }

    /** write pixel data of selected frame to PPM/ASCII file
     *
     ** @param  stream  open C file stream
     *
     ** @return status, true if successful, false otherwise
     */
    inline int writePPM(FILE *stream) const
    {
        if (Data != NULL)
        {
            register unsigned long i;
            for (i = 0; i < FrameSize; ++i)
                fprintf(stream, "%lu ", OFstatic_cast(unsigned long, Data[i]));
            return 1;
        }
        if (ColorData != NULL)
            return ColorData->writePPM(stream);
        return 0;
    }


 protected:

    /** examine which pixel values are actually used
     */
    inline void determineUsedValues()
    {
        if ((UsedValues == NULL) && (MaxValue > 0) && (MaxValue < MAX_TABLE_ENTRY_COUNT))
        {
            UsedValues = new Uint8[MaxValue + 1];
            if (UsedValues != NULL)
            {
                OFBitmanipTemplate<Uint8>::zeroMem(UsedValues, MaxValue + 1); // initialize array
                register const T3 *p = Data;
                register Uint8 *q = UsedValues;
                register unsigned long i;
                for (i = Count; i != 0; --i)
                    *(q + *(p++)) = 1;                                        // mark used entries
            }
        }
    }


 private:

    /** create a display LUT with the specified number of input bits
     *
     ** @param  dlut  reference to storage area where the display LUT should be stored
     *  @param  disp  pointer to object describing the current display function
     *  @param  bits  number of bits defining the input width of the display LUT
     */
    inline void createDisplayLUT(const DiDisplayLUT *&dlut,
                                 DiDisplayFunction *disp,
                                 const int bits)
    {
        if ((disp != NULL) && (disp->isValid()))
        {                                                                     // create Display LUT
            dlut = disp->getLookupTable(bits);
            if ((dlut != NULL) && (dlut->isValid()))                          // LUT is valid
            {
                DCMIMGLE_DEBUG("using display transformation");
            } else {
                DCMIMGLE_WARN("can't create display LUT ... ignoring display transformation");
                dlut = NULL;
            }
        }
    }

    /** initialize an optimization LUT if the optimization criteria is fulfilled
     *
     ** @param  lut   reference to storage area where the optimization LUT should be stored
     *  @param  ocnt  number of entries for the optimization LUT
     */
    inline int initOptimizationLUT(T3 *&lut,
                                   const unsigned long ocnt)
    {
        int result = 0;
        if ((sizeof(T1) <= 2) && (Count > 3 * ocnt))                          // optimization criteria
        {                                                                     // use LUT for optimization
            lut = new T3[ocnt];
            if (lut != NULL)
            {
                DCMIMGLE_DEBUG("using optimized routine with additional LUT (" << ocnt << " entries)");
                result = 1;
            }
        }
        return result;
    }

#ifdef PASTEL_COLOR_OUTPUT
    void color(void *buffer,                               // create true color pastel image
               const DiMonoPixel *inter,
               const unsigned long frame,
               const unsigned long frames)
    {
        ColorData = new DiMonoColorOutputPixelTemplate<T1, T3>(buffer, inter, frame, frames);
        if (ColorData != NULL)
            DCMIMGLE_DEBUG(">>> COLOR <<<");
    }
#endif

    /** apply the currently active VOI LUT to the output data
     *
     ** @param  inter  pointer to intermediate pixel representation
     *  @param  start  offset of the first pixel to be processed
     *  @param  vlut   VOI LUT
     *  @param  plut   presentation LUT (optional, maybe NULL)
     *  @param  disp   display function (optional, maybe NULL)
     *  @param  low    lowest pixel value for the output data (e.g. 0)
     *  @param  high   highest pixel value for the output data (e.g. 255)
     */
    void voilut(const DiMonoPixel *inter,
                const Uint32 start,
                const DiLookupTable *vlut,
                const DiLookupTable *plut,
                DiDisplayFunction *disp,
                const T3 low,
                const T3 high)
    {
        const T1 *pixel = OFstatic_cast(const T1 *, inter->getData());
        if ((pixel != NULL) && (vlut != NULL))
        {
            if (Data == NULL)
                Data = new T3[FrameSize];
            if (Data != NULL)
            {
                DCMIMGLE_DEBUG("applying VOI transformation with LUT (" << vlut->getCount() << " entries)");
                const DiDisplayLUT *dlut = NULL;
                const double minvalue = vlut->getMinValue();
                const double outrange = OFstatic_cast(double, high) - OFstatic_cast(double, low) + 1;
                register unsigned long i;
                if (minvalue == vlut->getMaxValue())                                    // LUT has only one entry or all entries are equal
                {
                    T3 value;
                    if ((plut != NULL) && (plut->isValid()))                            // has presentation LUT
                    {
                        DCMIMGLE_DEBUG("applying presentation LUT transformation");
                        createDisplayLUT(dlut, disp, plut->getBits());
                        const Uint32 value2 = OFstatic_cast(Uint32, (minvalue / OFstatic_cast(double, vlut->getAbsMaxRange())) * plut->getCount());
                        if (dlut != NULL)                                               // perform display transformation
                        {
                            DCMIMGLE_TRACE("monochrome rendering: VOI LUT #1 - UNTESTED");
                            if (low > high)                                             // invers
                                value = OFstatic_cast(T3, dlut->getValue(OFstatic_cast(Uint16, plut->getAbsMaxRange() - plut->getValue(value2) - 1)));
                            else                                                        // normal
                                value = OFstatic_cast(T3, dlut->getValue(OFstatic_cast(Uint16, plut->getValue(value2))));
                        } else {                                                        // don't use display: invalid or absent
                            DCMIMGLE_TRACE("monochrome rendering: VOI LUT #2");
                            value = OFstatic_cast(T3, OFstatic_cast(double, low) + OFstatic_cast(double, plut->getValue(value2)) * outrange / OFstatic_cast(double, plut->getAbsMaxRange()));
                        }
                    } else {                                                            // has no presentation LUT
                        createDisplayLUT(dlut, disp, vlut->getBits());
                        if (dlut != NULL)                                               // perform display transformation
                        {
                            DCMIMGLE_TRACE("monochrome rendering: VOI LUT #3 - UNTESTED");
                            if (low > high)                                             // invers
                                value = OFstatic_cast(T3, dlut->getValue(OFstatic_cast(Uint16, vlut->getAbsMaxRange() - minvalue - 1)));
                            else                                                        // normal
                                value = OFstatic_cast(T3, dlut->getValue(OFstatic_cast(Uint16, minvalue)));
                        } else {                                                        // don't use display: invalid or absent
                            DCMIMGLE_TRACE("monochrome rendering: VOI LUT #4");
                            value = OFstatic_cast(T3, OFstatic_cast(double, low) + (minvalue / OFstatic_cast(double, vlut->getAbsMaxRange())) * outrange);
                        }
                    }
                    OFBitmanipTemplate<T3>::setMem(Data, value, Count);                 // set output pixels to LUT value
                } else {
                    register T2 value = 0;
                    const T2 absmin = OFstatic_cast(T2, inter->getAbsMinimum());
                    const T2 firstentry = vlut->getFirstEntry(value);                   // choose signed/unsigned method
                    const T2 lastentry = vlut->getLastEntry(value);
                    const unsigned long ocnt = OFstatic_cast(unsigned long, inter->getAbsMaxRange());  // number of LUT entries
                    register const T1 *p = pixel + start;
                    register T3 *q = Data;
                    T3 *lut = NULL;
                    if ((plut != NULL) && (plut->isValid()))                            // has presentation LUT
                    {
                        DCMIMGLE_DEBUG("applying presentation LUT transformation");
                        createDisplayLUT(dlut, disp, plut->getBits());
                        register Uint32 value2;                                         // presentation LUT is always unsigned
                        const Uint32 pcnt = plut->getCount();
                        const double gradient1 = OFstatic_cast(double, pcnt) / OFstatic_cast(double, vlut->getAbsMaxRange());
                        const Uint32 firstvalue = OFstatic_cast(Uint32, OFstatic_cast(double, vlut->getFirstValue()) * gradient1);
                        const Uint32 lastvalue = OFstatic_cast(Uint32, OFstatic_cast(double, vlut->getLastValue()) * gradient1);
                        if (initOptimizationLUT(lut, ocnt))
                        {                                                                 // use LUT for optimization
                            q = lut;
                            if (dlut != NULL)                                             // perform display transformation
                            {
                                DCMIMGLE_TRACE("monochrome rendering: VOI LUT #5");
                                if (low > high)                                           // inverse
                                {
                                    const Uint16 maxvalue = OFstatic_cast(Uint16, plut->getAbsMaxRange() - 1);
                                    for (i = 0; i < ocnt; ++i)
                                    {
                                        value = OFstatic_cast(T2, i) + absmin;
                                        if (value <= firstentry)
                                            value2 = firstvalue;
                                        else if (value >= lastentry)
                                            value2 = lastvalue;
                                        else
                                            value2 = OFstatic_cast(Uint32, OFstatic_cast(double, vlut->getValue(value)) * gradient1);
                                        *(q++) = OFstatic_cast(T3, dlut->getValue(OFstatic_cast(Uint16, maxvalue - plut->getValue(value2))));
                                    }
                                } else {                                                  // normal
                                    for (i = 0; i < ocnt; ++i)
                                    {
                                        value = OFstatic_cast(T2, i) + absmin;
                                        if (value <= firstentry)
                                            value2 = firstvalue;
                                        else if (value >= lastentry)
                                            value2 = lastvalue;
                                        else
                                            value2 = OFstatic_cast(Uint32, OFstatic_cast(double, vlut->getValue(value)) * gradient1);
                                        *(q++) = OFstatic_cast(T3, dlut->getValue(plut->getValue(value2)));
                                    }
                                }
                            } else {                                                      // don't use display: invalid or absent
                                DCMIMGLE_TRACE("monochrome rendering: VOI LUT #6");
                                const double gradient2 = outrange / OFstatic_cast(double, plut->getAbsMaxRange());
                                for (i = 0; i < ocnt; ++i)
                                {
                                    value = OFstatic_cast(T2, i) + absmin;
                                    if (value <= firstentry)
                                        value2 = firstvalue;
                                    else if (value >= lastentry)
                                        value2 = lastvalue;
                                    else
                                        value2 = OFstatic_cast(Uint32, OFstatic_cast(double, vlut->getValue(value)) * gradient1);
                                    *(q++) = OFstatic_cast(T3, OFstatic_cast(double, low) + OFstatic_cast(double, plut->getValue(value2)) * gradient2);
                                }
                            }
                            const T3 *lut0 = lut - OFstatic_cast(T2, inter->getAbsMinimum());  // points to 'zero' entry
                            q = Data;
                            for (i = Count; i != 0; --i)                                  // apply LUT
                                *(q++) = *(lut0 + (*(p++)));
                        }
                        if (lut == NULL)                                                  // use "normal" transformation
                        {
                            if (dlut != NULL)                                             // perform display transformation
                            {
                                DCMIMGLE_TRACE("monochrome rendering: VOI LUT #7");
                                if (low > high)                                           // inverse
                                {
                                    const Uint16 maxvalue = OFstatic_cast(Uint16, vlut->getAbsMaxRange() - 1);
                                    for (i = Count; i != 0; --i)
                                    {
                                        value = OFstatic_cast(T2, *(p++));                // pixel value
                                        if (value <= firstentry)
                                            value2 = firstvalue;
                                        else if (value >= lastentry)
                                            value2 = lastvalue;
                                        else
                                            value2 = OFstatic_cast(Uint32, OFstatic_cast(double, vlut->getValue(value)) * gradient1);
                                        *(q++) = OFstatic_cast(T3, dlut->getValue(OFstatic_cast(Uint16, maxvalue - plut->getValue(value2))));
                                    }
                                } else {                                                  // normal
                                    for (i = Count; i != 0; --i)
                                    {
                                        value = OFstatic_cast(T2, *(p++));                // pixel value
                                        if (value <= firstentry)
                                            value2 = firstvalue;
                                        else if (value >= lastentry)
                                            value2 = lastvalue;
                                        else
                                            value2 = OFstatic_cast(Uint32, OFstatic_cast(double, vlut->getValue(value)) * gradient1);
                                        *(q++) = OFstatic_cast(T3, dlut->getValue(plut->getValue(value2)));
                                    }
                                }
                            } else {                                                      // don't use display: invalid or absent
                                DCMIMGLE_TRACE("monochrome rendering: VOI LUT #8");
                                const double gradient2 = outrange / OFstatic_cast(double, plut->getAbsMaxRange());
                                for (i = Count; i != 0; --i)
                                {
                                    value = OFstatic_cast(T2, *(p++));                    // pixel value
                                    if (value <= firstentry)
                                        value2 = firstvalue;
                                    else if (value >= lastentry)
                                        value2 = lastvalue;
                                    else
                                        value2 = OFstatic_cast(Uint32, OFstatic_cast(double, vlut->getValue(value)) * gradient1);
                                    *(q++) = OFstatic_cast(T3, OFstatic_cast(double, low) + OFstatic_cast(double, plut->getValue(value2)) * gradient2);
                                }
                            }
                        }
                    } else {                                                              // has no presentation LUT
                        createDisplayLUT(dlut, disp, vlut->getBits());
                        const double gradient = outrange / OFstatic_cast(double, vlut->getAbsMaxRange());
                        const T3 firstvalue = OFstatic_cast(T3, OFstatic_cast(double, low) + OFstatic_cast(double, vlut->getFirstValue()) * gradient);
                        const T3 lastvalue = OFstatic_cast(T3, OFstatic_cast(double, low) + OFstatic_cast(double, vlut->getLastValue()) * gradient);
                        if (initOptimizationLUT(lut, ocnt))
                        {                                                                 // use LUT for optimization
                            q = lut;
                            if (dlut != NULL)                                             // perform display transformation
                            {
                                DCMIMGLE_TRACE("monochrome rendering: VOI LUT #9");
                                if (low > high)                                           // inverse
                                {
                                    const Uint16 maxvalue = OFstatic_cast(Uint16, vlut->getAbsMaxRange() - 1);
                                    for (i = 0; i < ocnt; ++i)
                                    {
                                        value = OFstatic_cast(T2, i) + absmin;
                                        if (value < firstentry)
                                            value = firstentry;
                                        else if (value > lastentry)
                                            value = lastentry;
                                        *(q++) = OFstatic_cast(T3, dlut->getValue(OFstatic_cast(Uint16, maxvalue - vlut->getValue(value))));
                                    }
                                } else {                                                  // normal
                                    for (i = 0; i < ocnt; ++i)
                                    {
                                        value = OFstatic_cast(T2, i) + absmin;
                                        if (value < firstentry)
                                            value = firstentry;
                                        else if (value > lastentry)
                                            value = lastentry;
                                        *(q++) = OFstatic_cast(T3, dlut->getValue(vlut->getValue(value)));
                                    }
                                }
                            } else {                                                      // don't use display: invalid or absent
                                DCMIMGLE_TRACE("monochrome rendering: VOI LUT #10");
                                for (i = 0; i < ocnt; ++i)                                // calculating LUT entries
                                {
                                    value = OFstatic_cast(T2, i) + absmin;
                                    if (value <= firstentry)
                                        *(q++) = firstvalue;
                                    else if (value >= lastentry)
                                        *(q++) = lastvalue;
                                    else
                                        *(q++) = OFstatic_cast(T3, OFstatic_cast(double, low) + OFstatic_cast(double, vlut->getValue(value)) * gradient);
                                }
                            }
                            const T3 *lut0 = lut - OFstatic_cast(T2, inter->getAbsMinimum());   // points to 'zero' entry
                            q = Data;
                            for (i = Count; i != 0; --i)                                  // apply LUT
                                *(q++) = *(lut0 + (*(p++)));
                        }
                        if (lut == NULL)                                                  // use "normal" transformation
                        {
                            if (dlut != NULL)                                             // perform display transformation
                            {
                                DCMIMGLE_TRACE("monochrome rendering: VOI LUT #11");
                                if (low > high)                                           // inverse
                                {
                                    const Uint16 maxvalue = OFstatic_cast(Uint16, vlut->getAbsMaxRange() - 1);
                                    for (i = Count; i != 0; --i)
                                    {
                                        value = OFstatic_cast(T2, *(p++));
                                        if (value < firstentry)
                                            value = firstentry;
                                        else if (value > lastentry)
                                            value = lastentry;
                                        *(q++) = OFstatic_cast(T3, dlut->getValue(OFstatic_cast(Uint16, maxvalue - vlut->getValue(value))));
                                    }
                                } else {                                                  // normal
                                    for (i = Count; i != 0; --i)
                                    {
                                        value = OFstatic_cast(T2, *(p++));
                                        if (value < firstentry)
                                            value = firstentry;
                                        else if (value > lastentry)
                                            value = lastentry;
                                        *(q++) = OFstatic_cast(T3, dlut->getValue(vlut->getValue(value)));
                                    }
                                }
                            } else {                                                      // don't use display: invalid or absent
                                DCMIMGLE_TRACE("monochrome rendering: VOI LUT #12");
                                for (i = 0; i < Count; ++i)
                                {
                                    value = OFstatic_cast(T2, *(p++));
                                    if (value <= firstentry)
                                        *(q++) = firstvalue;
                                    else if (value >= lastentry)
                                        *(q++) = lastvalue;
                                    else
                                        *(q++) = OFstatic_cast(T3, OFstatic_cast(double, low) + OFstatic_cast(double, vlut->getValue(value)) * gradient);
                                }
                            }
                        }
                    }
                    delete[] lut;
                }
                if (Count < FrameSize)
                    OFBitmanipTemplate<T3>::zeroMem(Data + Count, FrameSize - Count);     // set remaining pixels of frame to zero
            }
        } else
            Data = NULL;
    }

    /** perform linear scaling to the output data (no windowing)
     *
     ** @param  inter  pointer to intermediate pixel representation
     *  @param  start  offset of the first pixel to be processed
     *  @param  plut   presentation LUT (optional, maybe NULL)
     *  @param  disp   display function (optional, maybe NULL)
     *  @param  low    lowest pixel value for the output data (e.g. 0)
     *  @param  high   highest pixel value for the output data (e.g. 255)
     */
    void nowindow(const DiMonoPixel *inter,
                  const Uint32 start,
                  const DiLookupTable *plut,
                  DiDisplayFunction *disp,
                  const T3 low,
                  const T3 high)
    {
        const DiDisplayLUT *dlut = NULL;
        const T1 *pixel = OFstatic_cast(const T1 *, inter->getData());
        if (pixel != NULL)
        {
            if (Data == NULL)                                                         // create new output buffer
                Data = new T3[FrameSize];
            if (Data != NULL)
            {
                DCMIMGLE_DEBUG("applying no VOI transformation (linear scaling)");
                const double absmin = inter->getAbsMinimum();
                const double absmax = inter->getAbsMaximum();
                const double outrange = OFstatic_cast(double, high) - OFstatic_cast(double, low) + 1;
                const unsigned long ocnt = OFstatic_cast(unsigned long, inter->getAbsMaxRange());  // number of LUT entries
                register const T1 *p = pixel + start;
                register T3 *q = Data;
                register unsigned long i;
                T3 *lut = NULL;
                if ((plut != NULL) && (plut->isValid()))                              // has presentation LUT
                {
                    DCMIMGLE_DEBUG("applying presentation LUT transformation");
                    createDisplayLUT(dlut, disp, plut->getBits());
                    register Uint32 value;                                            // presentation LUT is always unsigned
                    const double gradient1 = OFstatic_cast(double, plut->getCount()) / inter->getAbsMaxRange();
                    const double gradient2 = outrange / OFstatic_cast(double, plut->getAbsMaxRange());
                    if (initOptimizationLUT(lut, ocnt))
                    {                                                                 // use LUT for optimization
                        q = lut;
                        if (dlut != NULL)                                             // perform display transformation
                        {
                            DCMIMGLE_TRACE("monochrome rendering: VOI NONE #1");
                            if (low > high)                                           // inverse
                            {
                                const Uint16 maxvalue = OFstatic_cast(Uint16, plut->getAbsMaxRange() - 1);
                                for (i = 0; i < ocnt; ++i)
                                {
                                    value = OFstatic_cast(Uint32, OFstatic_cast(double, i) * gradient1);
                                    *(q++) = OFstatic_cast(T3, dlut->getValue(OFstatic_cast(Uint16, maxvalue - plut->getValue(value))));
                                }
                            } else {                                                  // normal
                                for (i = 0; i < ocnt; ++i)
                                {
                                    value = OFstatic_cast(Uint32, OFstatic_cast(double, i) * gradient1);
                                    *(q++) = OFstatic_cast(T3, dlut->getValue(plut->getValue(value)));
                                }
                            }
                        } else {                                                      // don't use display: invalid or absent
                            DCMIMGLE_TRACE("monochrome rendering: VOI NONE #2");
                            for (i = 0; i < ocnt; ++i)
                            {
                                value = OFstatic_cast(Uint32, OFstatic_cast(double, i) * gradient1);
                                *(q++) = OFstatic_cast(T3, OFstatic_cast(double, low) + OFstatic_cast(double, plut->getValue(value)) * gradient2);
                            }
                        }
                        const T3 *lut0 = lut - OFstatic_cast(T2, inter->getAbsMinimum());  // points to 'zero' entry
                        q = Data;
                        for (i = Count; i != 0; --i)                                  // apply LUT
                            *(q++) = *(lut0 + (*(p++)));
                    }
                    if (lut == NULL)                                                  // use "normal" transformation
                    {
                        if (dlut != NULL)                                             // perform display transformation
                        {
                            DCMIMGLE_TRACE("monochrome rendering: VOI NONE #3");
                            if (low > high)                                           // inverse
                            {
                                const Uint16 maxvalue = OFstatic_cast(Uint16, plut->getAbsMaxRange() - 1);
                                for (i = Count; i != 0; --i)
                                {
                                    value = OFstatic_cast(Uint32, (OFstatic_cast(double, *(p++)) - absmin) * gradient1);
                                    *(q++) = OFstatic_cast(T3, dlut->getValue(OFstatic_cast(Uint16, maxvalue - plut->getValue(value))));
                                }
                            } else {                                                  // normal
                                for (i = Count; i != 0; --i)
                                {
                                    value = OFstatic_cast(Uint32, (OFstatic_cast(double, *(p++)) - absmin) * gradient1);
                                    *(q++) = OFstatic_cast(T3, dlut->getValue(plut->getValue(value)));
                                }
                            }
                        } else {                                                      // don't use display: invalid or absent
                            DCMIMGLE_TRACE("monochrome rendering: VOI NONE #4");
                            for (i = Count; i != 0; --i)
                            {
                                value = OFstatic_cast(Uint32, (OFstatic_cast(double, *(p++)) - absmin) * gradient1);
                                *(q++) = OFstatic_cast(T3, OFstatic_cast(double, low) + OFstatic_cast(double, plut->getValue(value)) * gradient2);
                            }
                        }
                    }
                } else {                                                              // has no presentation LUT
                    createDisplayLUT(dlut, disp, inter->getBits());
                    register const double gradient = outrange / (inter->getAbsMaxRange());
                    if (initOptimizationLUT(lut, ocnt))
                    {                                                                 // use LUT for optimization
                        q = lut;
                        if (dlut != NULL)                                             // perform display transformation
                        {
                            DCMIMGLE_TRACE("monochrome rendering: VOI NONE #5");
                            if (low > high)                                           // inverse
                            {
                                for (i = ocnt; i != 0; --i)                           // calculating LUT entries
                                    *(q++) = OFstatic_cast(T3, dlut->getValue(OFstatic_cast(Uint16, i - 1)));
                            } else {                                                  // normal
                                for (i = 0; i < ocnt; ++i)                            // calculating LUT entries
                                    *(q++) = OFstatic_cast(T3, dlut->getValue(OFstatic_cast(Uint16, i)));
                            }
                        } else {                                                      // don't use display: invalid or absent
                            DCMIMGLE_TRACE("monochrome rendering: VOI NONE #6");
                            for (i = 0; i < ocnt; ++i)                                // calculating LUT entries
                                *(q++) = OFstatic_cast(T3, OFstatic_cast(double, low) + OFstatic_cast(double, i) * gradient);
                        }
                        const T3 *lut0 = lut - OFstatic_cast(T2, inter->getAbsMinimum());  // points to 'zero' entry
                        q = Data;
                        for (i = Count; i != 0; --i)                                  // apply LUT
                            *(q++) = *(lut0 + (*(p++)));
                    }
                    if (lut == NULL)                                                  // use "normal" transformation
                    {
                        if (dlut != NULL)                                             // perform display transformation
                        {
                            DCMIMGLE_TRACE("monochrome rendering: VOI NONE #7");
                            if (low > high)                                           // inverse
                            {
                                for (i = Count; i != 0; --i)
                                    *(q++) = OFstatic_cast(T3, dlut->getValue(OFstatic_cast(Uint16, absmax - (OFstatic_cast(double, *(p++)) - absmin))));
                            } else {                                                  // normal
                                for (i = Count; i != 0; --i)
                                    *(q++) = OFstatic_cast(T3, dlut->getValue(OFstatic_cast(Uint16, OFstatic_cast(double, *(p++)) - absmin)));
                            }
                        } else {                                                      // don't use display: invalid or absent
                            DCMIMGLE_TRACE("monochrome rendering: VOI NONE #8");
                            for (i = Count; i != 0; --i)
                                *(q++) = OFstatic_cast(T3, OFstatic_cast(double, low) + (OFstatic_cast(double, *(p++)) - absmin) * gradient);
                        }
                    }
                }
                delete[] lut;
                if (Count < FrameSize)
                    OFBitmanipTemplate<T3>::zeroMem(Data + Count, FrameSize - Count); // set remaining pixels of frame to zero
            }
        } else
            Data = NULL;
    }

    /** apply the currently active sigmoid VOI window to the output data
     *
     ** @param  inter   pointer to intermediate pixel representation
     *  @param  start   offset of the first pixel to be processed
     *  @param  plut    presentation LUT (optional, maybe NULL)
     *  @param  disp    display function (optional, maybe NULL)
     *  @param  center  window center
     *  @param  width   window width (>= 1)
     *  @param  low     lowest pixel value for the output data (e.g. 0)
     *  @param  high    highest pixel value for the output data (e.g. 255)
     */
    void sigmoid(const DiMonoPixel *inter,
                 const Uint32 start,
                 const DiLookupTable *plut,
                 DiDisplayFunction *disp,
                 const double center,
                 const double width,
                 const T3 low,
                 const T3 high)
    {
        const T1 *pixel = OFstatic_cast(const T1 *, inter->getData());
        if (pixel != NULL)
        {
            if (Data == NULL)
                Data = new T3[FrameSize];                                             // create new output buffer
            if (Data != NULL)
            {
                DCMIMGLE_DEBUG("applying sigmoid VOI transformation with window center = " << center << ", width = " << width);
                const DiDisplayLUT *dlut = NULL;
                const double absmin = inter->getAbsMinimum();
                const double outrange = OFstatic_cast(double, high) - OFstatic_cast(double, low);  // output range
                const unsigned long ocnt = OFstatic_cast(unsigned long, inter->getAbsMaxRange());  // number of LUT entries
                register const T1 *p = pixel + start;
                register T3 *q = Data;
                register unsigned long i;
                register double value;
                T3 *lut = NULL;
                if ((plut != NULL) && (plut->isValid()))                              // has presentation LUT
                {
                    DCMIMGLE_DEBUG("applying presentation LUT transformation");
                    createDisplayLUT(dlut, disp, plut->getBits());
                    register Uint32 value2;                                           // presentation LUT is always unsigned
                    const double plutcnt_1 = OFstatic_cast(double, plut->getCount() - 1);
                    const double plutmax_1 = OFstatic_cast(double, plut->getAbsMaxRange() - 1);
                    if (initOptimizationLUT(lut, ocnt))
                    {                                                                 // use LUT for optimization
                        q = lut;
                        if (dlut != NULL)                                             // perform display transformation
                        {
                            DCMIMGLE_TRACE("monochrome rendering: VOI SIGMOID #1");
                            const double maxvalue = OFstatic_cast(double, dlut->getCount() - 1);
                            const double offset = (low > high) ? maxvalue : 0;
                            const double gradient = (low > high) ? (-maxvalue / plutmax_1) : (maxvalue / plutmax_1);
                            for (i = 0; i < ocnt; ++i)
                            {
                                value = OFstatic_cast(double, i) + absmin;
                                value2 = OFstatic_cast(Uint32, plutcnt_1 / (1 + exp(-4 * (value - center) / width)));
                                *(q++) = OFstatic_cast(T3, dlut->getValue(OFstatic_cast(Uint16, offset + OFstatic_cast(double, plut->getValue(value2)) * gradient)));
                            }
                        } else {                                                      // don't use display: invalid or absent
                            DCMIMGLE_TRACE("monochrome rendering: VOI SIGMOID #2");
                            const double gradient = outrange / plutmax_1;
                            for (i = 0; i < ocnt; ++i)
                            {
                                value = OFstatic_cast(double, i) + absmin;
                                value2 = OFstatic_cast(Uint32, plutcnt_1 / (1 + exp(-4 * (value - center) / width)));
                                *(q++) = OFstatic_cast(T3, OFstatic_cast(double, low) + OFstatic_cast(double, plut->getValue(value2)) * gradient);
                            }
                        }
                        const T3 *lut0 = lut - OFstatic_cast(T2, absmin);             // points to 'zero' entry
                        q = Data;
                        for (i = Count; i != 0; --i)                                  // apply LUT
                            *(q++) = *(lut0 + (*(p++)));
                    }
                    if (lut == NULL)                                                  // use "normal" transformation
                    {
                        if (dlut != NULL)                                             // perform display transformation
                        {
                            DCMIMGLE_TRACE("monochrome rendering: VOI SIGMOID #3");
                            const double maxvalue = OFstatic_cast(double, dlut->getCount() - 1);
                            const double offset = (low > high) ? maxvalue : 0;
                            const double gradient = (low > high) ? (-maxvalue / plutmax_1) : (maxvalue / plutmax_1);
                            for (i = Count; i != 0; --i)
                            {
                                value = OFstatic_cast(double, *(p++));
                                value2 = OFstatic_cast(Uint32, plutcnt_1 / (1 + exp(-4 * (value - center) / width)));
                                *(q++) = OFstatic_cast(T3, dlut->getValue(OFstatic_cast(Uint16, offset + OFstatic_cast(double, plut->getValue(value2)) * gradient)));
                            }
                        } else {                                                      // don't use display: invalid or absent
                            DCMIMGLE_TRACE("monochrome rendering: VOI SIGMOID #4");
                            const double gradient = outrange / plutmax_1;
                            for (i = Count; i != 0; --i)
                            {
                                value = OFstatic_cast(double, *(p++));
                                value2 = OFstatic_cast(Uint32, plutcnt_1 / (1 + exp(-4 * (value - center) / width)));
                                *(q++) = OFstatic_cast(T3, OFstatic_cast(double, low) + OFstatic_cast(double, plut->getValue(value2)) * gradient);
                            }
                        }
                    }
                } else {                                                              // has no presentation LUT
                    createDisplayLUT(dlut, disp, bitsof(T1));
                    if (initOptimizationLUT(lut, ocnt))
                    {                                                                 // use LUT for optimization
                        q = lut;
                        if (dlut != NULL)                                             // perform display transformation
                        {
                            DCMIMGLE_TRACE("monochrome rendering: VOI SIGMOID #5");
                            const double maxvalue = OFstatic_cast(double, dlut->getCount() - 1);
                            const double outrange2 = (low > high) ? -maxvalue : maxvalue;
                            const double offset = (low > high) ? maxvalue : 0;
                            for (i = 0; i < ocnt; ++i)                                // calculating LUT entries
                            {
                                value = OFstatic_cast(double, i) + absmin;
                                *(q++) = OFstatic_cast(T3, dlut->getValue(OFstatic_cast(Uint16, offset + outrange2 / (1 + exp(-4 * (value - center) / width)))));
                            }
                        } else {                                                      // don't use display: invalid or absent
                            DCMIMGLE_TRACE("monochrome rendering: VOI SIGMOID #6");
                            for (i = 0; i < ocnt; ++i)                                // calculating LUT entries
                            {
                                value = OFstatic_cast(double, i) + absmin;
                                *(q++) = OFstatic_cast(T3, outrange / (1 + exp(-4 * (value - center) / width)));
                            }
                        }
                        const T3 *lut0 = lut - OFstatic_cast(T2, absmin);             // points to 'zero' entry
                        q = Data;
                        for (i = Count; i != 0; --i)                                  // apply LUT
                            *(q++) = *(lut0 + (*(p++)));
                    }
                    if (lut == NULL)                                                  // use "normal" transformation
                    {
                        if (dlut != NULL)                                             // perform display transformation
                        {
                            DCMIMGLE_TRACE("monochrome rendering: VOI SIGMOID #7");
                            const double maxvalue = OFstatic_cast(double, dlut->getCount() - 1);
                            const double outrange2 = (low > high) ? -maxvalue : maxvalue;
                            const double offset = (low > high) ? maxvalue : 0;
                            for (i = Count; i != 0; --i)
                            {
                                value = OFstatic_cast(double, *(p++));
                                *(q++) = OFstatic_cast(T3, dlut->getValue(OFstatic_cast(Uint16, offset + outrange2 / (1 + exp(-4 * (value - center) / width)))));
                            }
                        } else {                                                      // don't use display: invalid or absent
                            DCMIMGLE_TRACE("monochrome rendering: VOI SIGMOID #8");
                            for (i = Count; i != 0; --i)
                            {
                                value = OFstatic_cast(double, *(p++));
                                *(q++) = OFstatic_cast(T3, outrange / (1 + exp(-4 * (value - center) / width)));
                            }
                        }
                    }
                }
                delete[] lut;
                if (Count < FrameSize)
                    OFBitmanipTemplate<T3>::zeroMem(Data + Count, FrameSize - Count);        // set remaining pixels of frame to zero
            }
        } else
            Data = NULL;
    }

    /** apply the currently active linear VOI window to the output data
     *
     ** @param  inter   pointer to intermediate pixel representation
     *  @param  start   offset of the first pixel to be processed
     *  @param  plut    presentation LUT (optional, maybe NULL)
     *  @param  disp    display function (optional, maybe NULL)
     *  @param  center  window center
     *  @param  width   window width (>= 1)
     *  @param  low     lowest pixel value for the output data (e.g. 0)
     *  @param  high    highest pixel value for the output data (e.g. 255)
     */
    void window(const DiMonoPixel *inter,
                const Uint32 start,
                const DiLookupTable *plut,
                DiDisplayFunction *disp,
                const double center,
                const double width,
                const T3 low,
                const T3 high)
    {
        const T1 *pixel = OFstatic_cast(const T1 *, inter->getData());
        if (pixel != NULL)
        {
            if (Data == NULL)
                Data = new T3[FrameSize];                                             // create new output buffer
            if (Data != NULL)
            {
                DCMIMGLE_DEBUG("applying linear VOI transformation with window center = " << center << ", width = " << width);
                const DiDisplayLUT *dlut = NULL;
                const double absmin = inter->getAbsMinimum();
                const double width_1 = width - 1;
                const double leftBorder = center - 0.5 - width_1 / 2;                 // window borders, according to supplement 33
                const double rightBorder = center - 0.5 + width_1 / 2;
                const double outrange = OFstatic_cast(double, high) - OFstatic_cast(double, low);  // output range
                const unsigned long ocnt = OFstatic_cast(unsigned long, inter->getAbsMaxRange());  // number of LUT entries
                register const T1 *p = pixel + start;
                register T3 *q = Data;
                register unsigned long i;
                register double value;
                T3 *lut = NULL;
                if ((plut != NULL) && (plut->isValid()))                              // has presentation LUT
                {
                    DCMIMGLE_DEBUG("applying presentation LUT transformation");
                    createDisplayLUT(dlut, disp, plut->getBits());
                    register Uint32 value2;                                           // presentation LUT is always unsigned
                    const Uint32 pcnt = plut->getCount();
                    const double plutmax_1 = OFstatic_cast(double, plut->getAbsMaxRange()) - 1;
                    const double gradient1 = (width_1 == 0) ? 0 : OFstatic_cast(double, pcnt - 1) / width_1;
                    if (initOptimizationLUT(lut, ocnt))
                    {                                                                 // use LUT for optimization
                        q = lut;
                        if (dlut != NULL)                                             // perform display transformation
                        {
                            DCMIMGLE_TRACE("monochrome rendering: VOI LINEAR #1");
                            const double maxvalue = OFstatic_cast(double, dlut->getCount() - 1);
                            const double offset = (low > high) ? maxvalue : 0;
                            const double gradient2 = (low > high) ? (-maxvalue / plutmax_1) : (maxvalue / plutmax_1);
                            for (i = 0; i < ocnt; ++i)
                            {
                                value = OFstatic_cast(double, i) + absmin;            // pixel value
                                if (value <= leftBorder)
                                    value2 = 0;                                       // first LUT index
                                else if (value > rightBorder)
                                    value2 = pcnt - 1;                                // last LUT index
                                else
                                    value2 = OFstatic_cast(Uint32, (value - leftBorder) * gradient1);
                                *(q++) = OFstatic_cast(T3, dlut->getValue(OFstatic_cast(Uint16, offset + OFstatic_cast(double, plut->getValue(value2)) * gradient2)));
                            }
                        } else {                                                      // don't use display: invalid or absent
                            DCMIMGLE_TRACE("monochrome rendering: VOI LINEAR #2");
                            const double gradient2 = outrange / plutmax_1;
                            for (i = 0; i < ocnt; ++i)
                            {
                                value = OFstatic_cast(double, i) + absmin;            // pixel value
                                if (value <= leftBorder)
                                    value2 = 0;                                       // first LUT index
                                else if (value > rightBorder)
                                    value2 = pcnt - 1;                                // last LUT index
                                else
                                    value2 = OFstatic_cast(Uint32, (value - leftBorder) * gradient1);
                                *(q++) = OFstatic_cast(T3, OFstatic_cast(double, low) + OFstatic_cast(double, plut->getValue(value2)) * gradient2);
                            }
                        }
                        const T3 *lut0 = lut - OFstatic_cast(T2, absmin);             // points to 'zero' entry
                        q = Data;
                        for (i = Count; i != 0; --i)                                  // apply LUT
                            *(q++) = *(lut0 + (*(p++)));
                    }
                    if (lut == NULL)                                                  // use "normal" transformation
                    {
                        if (dlut != NULL)                                             // perform display transformation
                        {
                            DCMIMGLE_TRACE("monochrome rendering: VOI LINEAR #3");
                            const double maxvalue = OFstatic_cast(double, dlut->getCount() - 1);
                            const double offset = (low > high) ? maxvalue : 0;
                            const double gradient2 = (low > high) ? (-maxvalue / plutmax_1) : (maxvalue / plutmax_1);
                            for (i = Count; i != 0; --i)
                            {
                                value = OFstatic_cast(double, *(p++));                // pixel value
                                if (value <= leftBorder)
                                    value2 = 0;                                       // first LUT index
                                else if (value > rightBorder)
                                    value2 = pcnt - 1;                                // last LUT index
                                else
                                    value2 = OFstatic_cast(Uint32, (value - leftBorder) * gradient1);
                                *(q++) = OFstatic_cast(T3, dlut->getValue(OFstatic_cast(Uint16, offset + OFstatic_cast(double, plut->getValue(value2)) * gradient2)));
                            }
                        } else {                                                      // don't use display: invalid or absent
                            DCMIMGLE_TRACE("monochrome rendering: VOI LINEAR #4");
                            const double gradient2 = outrange / plutmax_1;
                            for (i = Count; i != 0; --i)
                            {
                                value = OFstatic_cast(double, *(p++));                // pixel value
                                if (value <= leftBorder)
                                    value2 = 0;                                       // first LUT index
                                else if (value > rightBorder)
                                    value2 = pcnt - 1;                                // last LUT index
                                else
                                    value2 = OFstatic_cast(Uint32, (value - leftBorder) * gradient1);
                                *(q++) = OFstatic_cast(T3, OFstatic_cast(double, low) + OFstatic_cast(double, plut->getValue(value2)) * gradient2);
                            }
                        }
                    }
                } else {                                                              // has no presentation LUT
                    createDisplayLUT(dlut, disp, bitsof(T1));
                    if (initOptimizationLUT(lut, ocnt))
                    {                                                                 // use LUT for optimization
                        q = lut;
                        if (dlut != NULL)                                             // perform display transformation
                        {
                            DCMIMGLE_TRACE("monochrome rendering: VOI LINEAR #5");
                            const double maxvalue = OFstatic_cast(double, dlut->getCount() - 1);
                            const double offset = (low > high) ? maxvalue : 0;
                            const double gradient = (width_1 == 0) ? 0 : ((low > high) ? (-maxvalue / width_1) : (maxvalue / width_1));
                            for (i = 0; i < ocnt; ++i)                                // calculating LUT entries
                            {
                                value = OFstatic_cast(double, i) + absmin - leftBorder;
                                if (value < 0)                                               // left border
                                    value = 0;
                                else if (value > width_1)                                    // right border
                                    value = width_1;
                                *(q++) = OFstatic_cast(T3, dlut->getValue(OFstatic_cast(Uint16, offset + value * gradient)));  // calculate value
                            }
                        } else {                                                       // don't use display: invalid or absent
                            DCMIMGLE_TRACE("monochrome rendering: VOI LINEAR #6");
                            const double offset = (width_1 == 0) ? 0 : (high - ((center - 0.5) / width_1 + 0.5) * outrange);
                            const double gradient = (width_1 == 0) ? 0 : outrange / width_1;
                            for (i = 0; i < ocnt; ++i)                                 // calculating LUT entries
                            {
                                value = OFstatic_cast(double, i) + absmin;
                                if (value <= leftBorder)
                                    *(q++) = low;                                            // black/white
                                else if (value > rightBorder)
                                    *(q++) = high;                                           // white/black
                                else
                                    *(q++) = OFstatic_cast(T3, offset + value * gradient);   // gray value
                            }
                        }
                        const T3 *lut0 = lut - OFstatic_cast(T2, absmin);             // points to 'zero' entry
                        q = Data;
                        for (i = Count; i != 0; --i)                                  // apply LUT
                            *(q++) = *(lut0 + (*(p++)));
                    }
                    if (lut == NULL)                                                  // use "normal" transformation
                    {
                        if (dlut != NULL)                                             // perform display transformation
                        {
                            DCMIMGLE_TRACE("monochrome rendering: VOI LINEAR #7");
                            const double maxvalue = OFstatic_cast(double, dlut->getCount() - 1);
                            const double offset = (low > high) ? maxvalue : 0;
                            const double gradient = (width_1 == 0) ? 0 : ((low > high) ? (-maxvalue / width_1) : (maxvalue / width_1));
                            for (i = Count; i != 0; --i)                              // calculating LUT entries
                            {
                                value = OFstatic_cast(double, *(p++)) - leftBorder;
                                if (value < 0)                                               // left border
                                    value = 0;
                                else if (value > width_1)                                    // right border
                                    value = width_1;
                                *(q++) = OFstatic_cast(T3, dlut->getValue(OFstatic_cast(Uint16, offset + value * gradient)));  // calculate value
                            }
                        } else {                                                      // don't use display: invalid or absent
                            DCMIMGLE_TRACE("monochrome rendering: VOI LINEAR #8");
                            const double offset = (width_1 == 0) ? 0 : (high - ((center - 0.5) / width_1 + 0.5) * outrange);
                            const double gradient = (width_1 == 0) ? 0 : outrange / width_1;
                            for (i = Count; i != 0; --i)
                            {
                                value = OFstatic_cast(double, *(p++));
                                if (value <= leftBorder)
                                    *(q++) = low;                                            // black/white
                                else if (value > rightBorder)
                                    *(q++) = high;                                           // white/black
                                else
                                    *(q++) = OFstatic_cast(T3, offset + value * gradient);   // gray value
                            }
                        }
                    }
                }
                delete[] lut;
                if (Count < FrameSize)
                    OFBitmanipTemplate<T3>::zeroMem(Data + Count, FrameSize - Count);        // set remaining pixels of frame to zero
            }
        } else
            Data = NULL;
    }


    /** apply the currently active overlay planes to the output data
     *
     ** @param  overlays  array of overlay management objects
     *  @param  disp      display function (optional, maybe NULL)
     *  @param  columns   image's width (in pixels)
     *  @param  rows      image's height (in pixels)
     *  @param  frame     number of frame to be rendered
     */
    void overlay(DiOverlay *overlays[2],
                 DiDisplayFunction *disp,
                 const Uint16 columns,
                 const Uint16 rows,
                 const unsigned long frame)
    {
        if ((Data != NULL) && (overlays != NULL))
        {
            for (unsigned int j = 0; j < 2; ++j)
            {
                if (overlays[j] != NULL)
                {
                    if (overlays[j]->getCount() > 0)
                        DCMIMGLE_DEBUG("applying " << ((j == 0) ? "built-in" : "additional") << " overlay planes");
                    const signed long left_pos = overlays[j]->getLeft();
                    const signed long top_pos = overlays[j]->getTop();
                    register DiOverlayPlane *plane;
                    for (unsigned int i = 0; i < overlays[j]->getCount(); ++i)
                    {
                        plane = overlays[j]->getPlane(i);
                        if ((plane != NULL) && plane->isVisible() && plane->reset(frame))
                        {
                            register T3 *q;
                            register Uint16 x;
                            register Uint16 y;
                            const Uint16 xmin = (plane->getLeft(left_pos) > 0) ? plane->getLeft(left_pos) : 0;
                            const Uint16 ymin = (plane->getTop(top_pos) > 0) ? plane->getTop(top_pos) : 0;
                            const Uint16 xmax = (plane->getRight(left_pos) < columns) ? plane->getRight(left_pos) : columns;
                            const Uint16 ymax = (plane->getBottom(top_pos) < rows) ? plane->getBottom(top_pos) : rows;
                            const T3 maxvalue = OFstatic_cast(T3, DicomImageClass::maxval(bitsof(T3)));
                            switch (plane->getMode())
                            {
                                case EMO_Replace:
                                {
                                    DCMIMGLE_DEBUG("applying overlay plane " << (i + 1) << " with 'replace' mode");
                                    const T3 fore = OFstatic_cast(T3, plane->getForeground() * maxvalue);
                                    for (y = ymin; y < ymax; ++y)
                                    {
                                        plane->setStart(OFstatic_cast(Uint16, left_pos + xmin), OFstatic_cast(Uint16, top_pos + y));
                                        q = Data + OFstatic_cast(unsigned long, y) * OFstatic_cast(unsigned long, columns) + OFstatic_cast(unsigned long, xmin);
                                        for (x = xmin; x < xmax; ++x, ++q)
                                        {
                                            if (plane->getNextBit())
                                                *q = fore;
                                        }
                                    }
                                    break;
                                }
                                case EMO_ThresholdReplace:
                                {
                                    DCMIMGLE_DEBUG("applying overlay plane " << (i + 1) << " with 'threshold replace' mode");
                                    const T3 fore = OFstatic_cast(T3, plane->getForeground() * maxvalue);
                                    const T3 thresh = OFstatic_cast(T3, plane->getThreshold() * maxvalue);
                                    for (y = ymin; y < ymax; ++y)
                                    {
                                        plane->setStart(OFstatic_cast(Uint16, left_pos + xmin), OFstatic_cast(Uint16, top_pos + y));
                                        q = Data + OFstatic_cast(unsigned long, y) * OFstatic_cast(unsigned long, columns) + OFstatic_cast(unsigned long, xmin);
                                        for (x = xmin; x < xmax; ++x, ++q)
                                        {
                                            if (plane->getNextBit())
                                                *q = (*q <= thresh) ? fore : 1;
                                        }
                                    }
                                    break;
                                }
                                case EMO_Complement:
                                {
                                    DCMIMGLE_DEBUG("applying overlay plane " << (i + 1) << " with 'complement' mode");
                                    const T3 thresh = OFstatic_cast(T3, DicomImageClass::maxval(bitsof(T3) / 2));
                                    for (y = ymin; y < ymax; ++y)
                                    {
                                        plane->setStart(OFstatic_cast(Uint16, left_pos + xmin), OFstatic_cast(Uint16, top_pos + y));
                                        q = Data + OFstatic_cast(unsigned long, y) * OFstatic_cast(unsigned long, columns) + OFstatic_cast(unsigned long, xmin);
                                        for (x = xmin; x < xmax; ++x, ++q)
                                        {
                                            if (plane->getNextBit())
                                                *q = (*q <= thresh) ? maxvalue : 0;
                                        }
                                    }
                                    break;
                                }
                                case EMO_InvertBitmap:
                                {
                                    DCMIMGLE_DEBUG("applying overlay plane " << (i + 1) << " with 'invert bitmap' mode");
                                    const T3 fore = OFstatic_cast(T3, plane->getForeground() * maxvalue);
                                    for (y = ymin; y < ymax; ++y)
                                    {
                                        plane->setStart(OFstatic_cast(Uint16, left_pos + xmin), OFstatic_cast(Uint16, top_pos + y));
                                        q = Data + OFstatic_cast(unsigned long, y) * OFstatic_cast(unsigned long, columns) + OFstatic_cast(unsigned long, xmin);
                                        for (x = xmin; x < xmax; ++x, ++q)
                                        {
                                            if (!plane->getNextBit())
                                                *q = fore;
                                        }
                                    }
                                    break;
                                }
                                case EMO_RegionOfInterest:
                                {
                                    DCMIMGLE_DEBUG("applying overlay plane " << (i + 1) << " with 'region of interest' mode");
                                    const int dim = bitsof(T3) / 2;
                                    for (y = ymin; y < ymax; ++y)
                                    {
                                        plane->setStart(OFstatic_cast(Uint16, left_pos + xmin), OFstatic_cast(Uint16, top_pos + y));
                                        q = Data + OFstatic_cast(unsigned long, y) * OFstatic_cast(unsigned long, columns) + OFstatic_cast(unsigned long, xmin);
                                        for (x = xmin; x < xmax; ++x, ++q)
                                        {
                                            if (!plane->getNextBit())
                                                *q = *q >> dim;
                                        }
                                    }
                                    break;
                                }
                                case EMO_BitmapShutter:
                                {
                                    DCMIMGLE_DEBUG("applying overlay plane " << (i + 1) << " with 'bitmap shutter' mode");
                                    register T3 fore = OFstatic_cast(T3, OFstatic_cast(double, maxvalue) * OFstatic_cast(double, plane->getPValue()) / OFstatic_cast(double, DicomImageClass::maxval(WIDTH_OF_PVALUES)));
                                    if ((disp != NULL) && (disp->isValid()))
                                    {
                                        const DiDisplayLUT *dlut = disp->getLookupTable(WIDTH_OF_PVALUES);
                                        if ((dlut != NULL) && (dlut->isValid()))
                                            fore = OFstatic_cast(T3, dlut->getValue(plane->getPValue()));
                                    }
                                    for (y = ymin; y < ymax; ++y)
                                    {
                                        plane->setStart(OFstatic_cast(Uint16, left_pos + xmin), OFstatic_cast(Uint16, top_pos + y));
                                        q = Data + OFstatic_cast(unsigned long, y) * OFstatic_cast(unsigned long, columns) + OFstatic_cast(unsigned long, xmin);
                                        for (x = xmin; x < xmax; ++x, ++q)
                                        {
                                            if (plane->getNextBit())
                                                *q = fore;
                                        }
                                    }
                                    break;
                                }
                                default: /* e.g. EMO_Default */
                                    DCMIMGLE_WARN("unhandled overlay mode (" << OFstatic_cast(int, plane->getMode()) << ")");
                            }
                        }
                    }
                }
            }
        }
    }


    /// pointer to the storage area where the output data should be stored
    T3 *Data;
    /// flag indicating whether the output data buffer should be deleted in the destructor
    int DeleteData;

#ifdef PASTEL_COLOR_OUTPUT
    DiMonoColorOutputPixelTemplate<T1, T3> *ColorData;
#else
    // dummy variable
    DiMonoOutputPixel *ColorData;
#endif

 // --- declarations to avoid compiler warnings

    DiMonoOutputPixelTemplate(const DiMonoOutputPixelTemplate<T1,T2,T3> &);
    DiMonoOutputPixelTemplate<T1,T2,T3> &operator=(const DiMonoOutputPixelTemplate<T1,T2,T3> &);
};


#endif


/*
 *
 * CVS/RCS Log:
 * $Log: dimoopxt.h,v $
 * Revision 1.54  2010-10-28 10:58:38  joergr
 * Implemented missing rendering code for sigmoid VOI LUT function.
 * Added more trace log messages which might be helpful for detailed debugging.
 *
 * Revision 1.53  2010-10-14 13:16:26  joergr
 * Updated copyright header. Added reference to COPYRIGHT file.
 *
 * Revision 1.52  2010-10-05 15:25:10  joergr
 * Added preliminary support for VOI LUT function. Please note, however, that
 * the sigmoid transformation is not yet implemented.
 * In debug mode, output more details on overlay plane to the logger.
 *
 * Revision 1.51  2010-03-01 09:08:47  uli
 * Removed some unnecessary include directives in the headers.
 *
 * Revision 1.50  2009-11-25 16:08:26  joergr
 * Removed inclusion of header file "ofconsol.h".
 * Revised logging messages. Added more logging messages.
 *
 * Revision 1.49  2009-10-28 14:38:16  joergr
 * Fixed minor issues in log output.
 *
 * Revision 1.48  2009-10-28 09:53:40  uli
 * Switched to logging mechanism provided by the "new" oflog module.
 *
 * Revision 1.47  2006-08-15 16:30:11  meichel
 * Updated the code in module dcmimgle to correctly compile when
 *   all standard C++ classes remain in namespace std.
 *
 * Revision 1.46  2005/12/08 16:47:54  meichel
 * Changed include path schema for all DCMTK header files
 *
 * Revision 1.45  2005/03/09 17:30:42  joergr
 * Added support for new overlay mode "invert bitmap".
 *
 * Revision 1.44  2004/02/06 11:07:50  joergr
 * Distinguish more clearly between const and non-const access to pixel data.
 *
 * Revision 1.43  2003/12/23 15:53:22  joergr
 * Replaced post-increment/decrement operators by pre-increment/decrement
 * operators where appropriate (e.g. 'i++' by '++i').
 *
 * Revision 1.42  2003/12/23 10:51:52  joergr
 * Updated documentation to get rid of doxygen warnings.
 *
 * Revision 1.41  2003/12/09 16:49:11  joergr
 * Adapted type casts to new-style typecast operators defined in ofcast.h.
 * Removed leading underscore characters from preprocessor symbols (reserved
 * symbols). Updated copyright header.
 *
 * Revision 1.40  2003/06/12 15:08:34  joergr
 * Fixed inconsistent API documentation reported by Doxygen.
 *
 * Revision 1.39  2002/12/09 13:32:53  joergr
 * Renamed parameter/local variable to avoid name clashes with global
 * declaration left and/or right (used for as iostream manipulators).
 *
 * Revision 1.38  2002/11/27 14:08:06  meichel
 * Adapted module dcmimgle to use of new header file ofstdinc.h
 *
 * Revision 1.37  2002/06/19 08:12:01  meichel
 * Added typecasts to avoid ambiguity with built-in functions on gcc 3.2
 *
 * Revision 1.36  2001/06/01 15:49:46  meichel
 * Updated copyright header
 *
 * Revision 1.35  2000/05/03 09:46:28  joergr
 * Removed most informational and some warning messages from release built
 * (#ifndef DEBUG).
 *
 * Revision 1.34  2000/04/28 12:32:31  joergr
 * DebugLevel - global for the module - now derived from OFGlobal (MF-safe).
 *
 * Revision 1.33  2000/04/27 13:08:40  joergr
 * Dcmimgle library code now consistently uses ofConsole for error output.
 *
 * Revision 1.32  2000/03/08 16:24:20  meichel
 * Updated copyright header.
 *
 * Revision 1.31  2000/03/07 16:15:12  joergr
 * Added explicit type casts to make Sun CC 2.0.1 happy.
 *
 * Revision 1.30  2000/03/06 18:19:36  joergr
 * Moved get-method to base class, renamed method and made method virtual to
 * avoid hiding of methods (reported by Sun CC 4.2).
 *
 * Revision 1.29  2000/03/03 14:09:13  meichel
 * Implemented library support for redirecting error messages into memory
 *   instead of printing them to stdout/stderr for GUI applications.
 *
 * Revision 1.28  2000/02/01 10:52:37  meichel
 * Avoiding to include <stdlib.h> as extern "C" on Borland C++ Builder 4,
 *   workaround for bug in compiler header files.
 *
 * Revision 1.27  1999/10/11 20:14:14  joergr
 * Fixed bug in window() routine for cases where presentation LUT is active.
 *
 * Revision 1.26  1999/10/06 13:42:03  joergr
 * Added method to remove reference to (internally handled) pixel data.
 *
 * Revision 1.25  1999/09/17 12:40:45  joergr
 * Added/changed/completed DOC++ style comments in the header files.
 * Enhanced efficiency of some "for" loops.
 *
 * Revision 1.24  1999/09/10 08:45:19  joergr
 * Added support for CIELAB display function.
 *
 * Revision 1.23  1999/08/25 16:41:53  joergr
 * Added new feature: Allow clipping region to be outside the image
 * (overlapping).
 *
 * Revision 1.22  1999/08/17 10:26:08  joergr
 * Commented unused parameter names to avoid compiler warnings.
 *
 * Revision 1.21  1999/07/23 14:08:44  joergr
 * Changed implementation/interpretation of windows center/width (according to
 * new letter ballot of supplement 33).
 * Enhanced handling of corrupted pixel data (wrong length).
 *
 * Revision 1.20  1999/05/03 15:43:21  joergr
 * Replaced method applyOptimizationLUT by its contents (method body) to avoid
 * warnings (and possible errors) on Sun CC 2.0.1 :-/
 *
 * Revision 1.18  1999/04/30 16:10:50  meichel
 * Minor code purifications to keep IBM xlC quiet
 *
 * Revision 1.17  1999/04/29 16:46:46  meichel
 * Minor code purifications to keep DEC cxx 6 quiet.
 *
 * Revision 1.16  1999/04/29 09:38:34  joergr
 * Changed position of "#ifdef" to avoid compiler warnings.
 *
 * Revision 1.15  1999/04/29 09:20:01  joergr
 * Removed color related image files from public toolkit part.
 *
 * Revision 1.14  1999/04/28 18:56:07  joergr
 * Removed support for pastel color output from public DCMTK part.
 *
 * Revision 1.13  1999/04/28 14:51:44  joergr
 * Added experimental support to create grayscale images with more than 256
 * shades of gray to be displayed on a consumer monitor (use pastel colors).
 * Introduced new scheme for the debug level variable: now each level can be
 * set separately (there is no "include" relationship).
 *
 * Revision 1.12  1999/03/24 17:20:14  joergr
 * Added/Modified comments and formatting.
 *
 * Revision 1.11  1999/03/02 12:03:52  joergr
 * Corrected bug in output routine of monochrome pixel data (wrong scaling when
 * Barten transformation and windowing are active).
 *
 * Revision 1.10  1999/02/28 16:41:01  joergr
 * Corrected bug: the output bits for bitmaps shutters were inverted (this was
 * done due to an error in the used test images).
 *
 * Revision 1.9  1999/02/11 16:40:19  joergr
 * Added routine to check whether particular grayscale values are unused in
 * the output data.
 * Removed two small memory leaks reported by dmalloc library.
 *
 * Revision 1.8  1999/02/05 16:44:52  joergr
 * Corrected calculation of DDL value for bitmaps shutters (overlays).
 *
 * Revision 1.7  1999/02/05 15:13:36  joergr
 * Added conversion P-Value to DDL when display function is absent.
 *
 * Revision 1.6  1999/02/03 17:32:43  joergr
 * Added optimization LUT to transform pixel data.
 * Added support for calibration according to Barten transformation (incl.
 * a DISPLAY file describing the monitor characteristic).
 *
 * Revision 1.5  1999/01/20 15:11:05  joergr
 * Replaced invocation of getCount() by member variable Count where possible.
 * Added new output method to fill external memory buffer with rendered pixel
 * data.
 * Added new overlay plane mode for bitmap shutters.
 * Added optimization to modality and VOI transformation (using additional
 * LUTs).
 *
 * Revision 1.4  1998/12/23 12:40:01  joergr
 * Removed unused parameter (BitsPerSample).
 *
 * Revision 1.3  1998/12/22 14:32:49  joergr
 * Improved implementation of presentation LUT application (and other gray
 * scale transformations). Tested with ECR test images from David Clunie.
 *
 * Revision 1.2  1998/12/14 17:25:55  joergr
 * Added support for correct scaling of input/output values for grayscale
 * transformations.
 *
 * Revision 1.1  1998/11/27 15:29:53  joergr
 * Added copyright message.
 * Introduced global debug level for dcmimage module to control error output.
 * Corrected bug in VOI LUT transformation method.
 * Changed behaviour: now window width of 0 is valid and negative width
 * is invalid.
 *
 * Revision 1.6  1998/07/01 08:39:24  joergr
 * Minor changes to avoid compiler warnings (gcc 2.8.1 with additional
 * options), e.g. add copy constructors.
 *
 * Revision 1.5  1998/05/11 14:53:22  joergr
 * Added CVS/RCS header to each file.
 *
 *
 */

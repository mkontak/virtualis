/*
 *
 *  Copyright (C) 1994-2010, OFFIS e.V.
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
 *  Module:  dcmdata
 *
 *  Author:  Marco Eichelberg
 *
 *  Purpose: DcmOutputBufferStream and related classes,
 *    implements output to blocks of memory as needed in the dcmnet module.
 *
 *  Last Update:      $Author: joergr $
 *  Update Date:      $Date: 2010-10-14 13:15:41 $
 *  CVS/RCS Revision: $Revision: 1.5 $
 *  Status:           $State: Exp $
 *
 *  CVS/RCS Log at end of file
 *
 */

#ifndef DCOSTRMB_H
#define DCOSTRMB_H

#include "config/osconfig.h"
#include "dcmdata/dcostrma.h"

/** consumer class that stores data in a buffer provided by the caller.
 *  Used for DICOM network communication.
 */
class DcmBufferConsumer: public DcmConsumer
{
public:

  /** constructor
   *  @param buf buffer in which data is stored. Must be allocated
   *    by caller and remain valid during the lifetime of this object.
   *  @param bufLen buffer length, must be even number > 0.
   */
  DcmBufferConsumer(void *buf, offile_off_t bufLen);

  /// destructor
  virtual ~DcmBufferConsumer();

  /** returns the status of the consumer. Unless the status is good,
   *  the consumer will not permit any operation.
   *  @return status, true if good
   */
  virtual OFBool good() const;

  /** returns the status of the consumer as an OFCondition object.
   *  Unless the status is good, the consumer will not permit any operation.
   *  @return status, EC_Normal if good
   */
  virtual OFCondition status() const;

  /** returns true if the consumer is flushed, i.e. has no more data
   *  pending in it's internal state that needs to be flushed before
   *  the stream is closed.
   *  @return true if consumer is flushed, false otherwise
   */
  virtual OFBool isFlushed() const;

  /** returns the minimum number of bytes that can be written with the
   *  next call to write(). The DcmObject write methods rely on avail
   *  to return a value > 0 if there is no I/O suspension since certain
   *  data such as tag and length are only written "en bloc", i.e. all
   *  or nothing.
   *  @return minimum of space available in consumer
   */
  virtual offile_off_t avail() const;

  /** processes as many bytes as possible from the given input block.
   *  @param buf pointer to memory block, must not be NULL
   *  @param buflen length of memory block
   *  @return number of bytes actually processed. 
   */
  virtual offile_off_t write(const void *buf, offile_off_t buflen);

  /** instructs the consumer to flush its internal content until
   *  either the consumer becomes "flushed" or I/O suspension occurs.
   *  After a call to flush(), a call to write() will produce undefined
   *  behaviour.
   */
  virtual void flush();

  /** retrieves and flushes the underlying buffer.
   *  After return of this method, the buffer is considered to have
   *  been flushed (copied, stored) by the caller and is reused
   *  by the next write operation.
   *  @param buffer pointer to user provided buffer returned in this parameter
   *  @param length number of bytes in buffer returned in this parameter
   */
  virtual void flushBuffer(void *& buffer, offile_off_t& length);

private:

  /// private unimplemented copy constructor
  DcmBufferConsumer(const DcmBufferConsumer&);

  /// private unimplemented copy assignment operator
  DcmBufferConsumer& operator=(const DcmBufferConsumer&);

  /// the buffer we're actually writing to
  unsigned char *buffer_;

  /// size of the buffer, in bytes
  offile_off_t bufSize_;

  /// number of bytes filled in buffer
  offile_off_t filled_;

  /// status
  OFCondition status_;
};


/** output stream that writes into a buffer of fixed length
 *  which must be provided by the caller.
 */
class DcmOutputBufferStream: public DcmOutputStream
{
public:
  /** constructor
   *  @param buf buffer in which data is stored. Must be allocated
   *    by caller and remain valid during the lifetime of this object.
   *  @param bufLen buffer length, must be even number > 0.
   */
  DcmOutputBufferStream(void *buf, offile_off_t bufLen);

  /// destructor
  virtual ~DcmOutputBufferStream();

  /** retrieves and flushes the underlying buffer.
   *  After return of this method, the buffer is considered to have
   *  been flushed (copied, stored) by the caller and is reused
   *  by the next write operation.
   *  @param buffer pointer to user provided buffer returned in this parameter
   *  @param length number of bytes in buffer returned in this parameter
   */
  virtual void flushBuffer(void *& buffer, offile_off_t& length);

private:

  /// private unimplemented copy constructor
  DcmOutputBufferStream(const DcmOutputBufferStream&);

  /// private unimplemented copy assignment operator
  DcmOutputBufferStream& operator=(const DcmOutputBufferStream&);

  /// the final consumer of the filter chain
  DcmBufferConsumer consumer_;

};


#endif

/*
 * CVS/RCS Log:
 * $Log: dcostrmb.h,v $
 * Revision 1.5  2010-10-14 13:15:41  joergr
 * Updated copyright header. Added reference to COPYRIGHT file.
 *
 * Revision 1.4  2009-11-04 09:58:07  uli
 * Switched to logging mechanism provided by the "new" oflog module
 *
 * Revision 1.3  2007-02-19 16:06:09  meichel
 * Class DcmOutputStream and related classes are now safe for use with
 *   large files (2 GBytes or more) if supported by compiler and operating system.
 *
 * Revision 1.2  2005/12/08 16:28:25  meichel
 * Changed include path schema for all DCMTK header files
 *
 * Revision 1.1  2002/08/27 16:55:36  meichel
 * Initial release of new DICOM I/O stream classes that add support for stream
 *   compression (deflated little endian explicit VR transfer syntax)
 *
 *
 */

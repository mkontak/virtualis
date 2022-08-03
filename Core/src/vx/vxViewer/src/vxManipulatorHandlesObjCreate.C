// $Id: vxManipulatorHandlesObjCreate.C,v 1.1.1.1 2004/03/02 03:58:42 geconomos Exp $
//
// Copyright©2002, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form,
// in whole or in part, without the prior written permission Viatronix Inc.
//
// Author: Manjushree Nulkar (manju@viatronix.com)
//
// Complete history at bottom of file.


// includes
#include "vxManipulatorHandlesObjCreate.h"
#include "vxEnvironment.h"
#include "vxMouseCursor.h"
#include "vxUtils.h"
#include "vxInput.h"
#include "AnnotationArrow.h"
#include "AnnotationPoint.h"
#include "AnnotationLinear.h"
#include "AnnotationAngle.h"
#include "AnnotationVOI.h"
#include "AnnotationROI.h"
#include "AnnotationEllipse.h"
#include "AnnotationRectangle.h"
#include "GLContext.h"
#include "vxBlockVolumeIterator.h"


// defines
#define FILE_REVISION "$Revision: 1.1.1.1 $"


/**
 * Constructor
 */
vxManipulatorHandlesObjCreate::vxManipulatorHandlesObjCreate(vxEnvironment & environment)
:vxManipulator(environment),
m_bFlippedVertical(false),
m_bFlippedHorizontal(false),
m_bCrossVert(false),
m_bCrossHorz(false)                              
{  
  GetEnvironment().GetHandlesSetPtr()->RemoveLinearSplitter();
} // vxManipulatorHandlesObjCreate()


/**
 * Event handler if the mouse is pressed
 * @param position mouse position.
 * @param eButtonId mouse button.
 * @param uKeyboardState state of the mouse.
 * @return void.
 */ 
bool vxManipulatorHandlesObjCreate::OnButtonDown(const Point2D<int2> & position, 
                                                 const ButtonEnum eButtonId, 
                                                 const uint4 uKeyboardState)
{
  bool bIfHandled(false);
  //    bIfHandled = true;
  if (eButtonId == BUTTON_LEFT)
  {    
    // get the world coordinates for the mouse position. 
    glReadBuffer(GL_BACK);

    Point2D<int2> screenPos(position);
    screenPos.m_x = Bound((int2)(GetEnvironment().GetWinRect().GetReferencePoint().first), (int2)(screenPos.m_x), 
                          (int2)(GetEnvironment().GetWinRect().GetViewportSize().first - 1));
    screenPos.m_y = Bound((int2)(GetEnvironment().GetWinRect().GetReferencePoint().second), (int2)(screenPos.m_y), 
                          (int2)(GetEnvironment().GetWinRect().GetViewportSize().second - 1));

    Point<float4> mouseWorldCoord;
    GetWorldCoordinates(screenPos, mouseWorldCoord, false);
  
    // Start creation of the selected annotation.
    bool bResult(false);
    Annotation * pMeasurement = NULL;
    
    // ANGLE Annotation.  
    if ((GetEnvironment().GetHandlesSetPtr())->GetAnnotationMode() == HandlesSet::ANNOTATION_ANGLE )
    {
      // Angle Measurement mode selected.
      // getting last point of the annotation, end point of second line segment.
      if((GetEnvironment().GetHandlesSetPtr())->m_currentMeasurement != NULL)
      {      
        HandlesObj *pHandlesObj = *((GetEnvironment().GetHandlesSetPtr())->m_currentMeasurement);
        pMeasurement = reinterpret_cast<Annotation *>(pHandlesObj);
        
        // end point of second line segment of the angle is being set.
        if (typeid(*pMeasurement) == typeid(AnnotationAngle))
        {
          GetEnvironment().GetHandlesSetPtr()->Modified(vxModIndInfo(HandlesSet, 0, HandlesSet::EVENT_MODIFIED));
        }
        else
        {
          (GetEnvironment().GetHandlesSetPtr())->m_currentMeasurement = HandlesSetIterator(NULL);
        }      
      } // end if
      // create a new angle annotation.
      if ((GetEnvironment().GetHandlesSetPtr())->m_currentMeasurement == NULL)
      {      
        // create a new annotation.
        (GetEnvironment().GetHandlesSetPtr())->m_currentMeasurement = HandlesSetIterator(NULL);
        AnnotationAngle *ameasurement = new AnnotationAngle;
        pMeasurement = ameasurement;
        HandlesObj *pHandlesObj = pMeasurement;
        Annotation::LineEnds<Point<float4> > measureEnds;
        
        // store the end points of one line segment of the angle in 3D.
        measureEnds.m_start = measureEnds.m_end = mouseWorldCoord;      
        ameasurement->SetFirstLineSegmentEndPoints(Annotation::MEASUREFUNC_START, measureEnds);
        ameasurement->SetUsedPoints(1);
        
        (GetEnvironment().GetHandlesSetPtr())->push_front(pHandlesObj);
        (GetEnvironment().GetHandlesSetPtr())->m_currentMeasurement = (GetEnvironment().GetHandlesSetPtr())->begin();
        bResult = true;
      } // end else
    }// End Angle Annotation mode.
    
    // LINEAR Annotation.
    else if ((GetEnvironment().GetHandlesSetPtr())->GetAnnotationMode() == HandlesSet::ANNOTATION_LINEAR )
    {
      (GetEnvironment().GetHandlesSetPtr())->m_currentMeasurement = HandlesSetIterator(NULL);
      // Linear measurement mode selected.
      Annotation::LineEnds<Point<float4> > measureEnds;
      AnnotationLinear *lmeasurement = new AnnotationLinear;
      pMeasurement = lmeasurement;
      HandlesObj *pHandlesObj = pMeasurement;    
      
      // store the end points in 3D.
      measureEnds.m_start = measureEnds.m_end = mouseWorldCoord;
      
      // Create a linear annotation and calculate the distance between the end points.
      lmeasurement->CalculateLinear(Annotation::MEASUREFUNC_START, measureEnds);
      
      // store the annotation in the list.
      (GetEnvironment().GetHandlesSetPtr())->push_front(pHandlesObj);
      (GetEnvironment().GetHandlesSetPtr())->m_currentMeasurement = (GetEnvironment().GetHandlesSetPtr())->begin();
      bResult = true;      
    } // End Linear mode.
    
    // POINT Annotation.
    else if ((GetEnvironment().GetHandlesSetPtr())->GetAnnotationMode() == HandlesSet::ANNOTATION_POINT)
    {
      GetEnvironment().GetHandlesSetPtr()->m_currentMeasurement = HandlesSetIterator(NULL);
      // Point Annotation mode selected.
      AnnotationPoint * point = new AnnotationPoint;
      pMeasurement = point;
      HandlesObj * pHandlesObj = pMeasurement;
      
      // Set the radius of the point as a function of the volume size, so it does not appear too
      // large on bigger datasets.
      Triple<float4> volSize(static_cast<vxBlockVolume<uint2> *>(GetEnvironment().GetIntensityVolumePtr())->GetHeader().GetWorldDim()); 

      float maxDim(Max(volSize.m_x, volSize.m_y, volSize.m_z));
      float4 fRadius(point->GetRadius() * (maxDim / 258.0));      

      // set the radius of the sphere.
      point->SetRadius(fRadius);
      // store the center point in 3D and create a point annotation as a sphere.
      point->CreatePoint(mouseWorldCoord);
      
      // store the annotation in the annotation list.
      GetEnvironment().GetHandlesSetPtr()->push_front(pHandlesObj);
      GetEnvironment().GetHandlesSetPtr()->m_currentMeasurement = (GetEnvironment().GetHandlesSetPtr())->begin();
      
      bResult = true;
    }// End Point Annotation mode.    
    
    // ELLIPSE Annotation.
    else if ((GetEnvironment().GetHandlesSetPtr())->GetAnnotationMode() == HandlesSet::ANNOTATION_ELLIPSE )
    {
      (GetEnvironment().GetHandlesSetPtr())->m_currentMeasurement = HandlesSetIterator(NULL);
      // Ellipse annotation mode selected.
      AnnotationEllipse *rmeasurement = new AnnotationEllipse;
      pMeasurement = rmeasurement;
      HandlesObj *pHandlesObj = pMeasurement;
      
      // set this position as the last mouse position which will be used in the move event.
      SetLastMousePosition(screenPos);

      Point<float4> majorAxisPt;
      if (GetWorldCoordinates(Point2D<int2>(screenPos.m_x + 2.0, screenPos.m_y), majorAxisPt) == false)
        return false;
      Point<float4> minorAxisPt;
      if (GetWorldCoordinates(Point2D<int2>(screenPos.m_x, screenPos.m_y + 2.0), minorAxisPt) == false)
        return false;

      // Get the major and minor axes in world coordinates.
      Vector<float4> majorAxis = majorAxisPt - mouseWorldCoord;
      Vector<float4> minorAxis = minorAxisPt - mouseWorldCoord;  
      
      m_bFlippedVertical = false;  m_bFlippedHorizontal = false;
      m_bCrossHorz = false; m_bCrossVert = false;
      
      // create an ellipse annotation.
      rmeasurement->CreateEllipse(mouseWorldCoord, majorAxis, minorAxis);
      //calculate ellipse parameters.
      rmeasurement->CalculateEllipseStatistics(static_cast<vxBlockVolume<uint2> *>(GetEnvironment().GetIntensityVolumePtr())); 
      
      m_bFlippedVertical = false;  m_bFlippedHorizontal = false;

      // store the measurement in the list.
      (GetEnvironment().GetHandlesSetPtr())->push_front(pHandlesObj);
      (GetEnvironment().GetHandlesSetPtr())->m_currentMeasurement = (GetEnvironment().GetHandlesSetPtr())->begin();
      bResult = true;      
    } // End Ellipse Annotation.
    
    // Rectangle Annotation.
    else if(GetEnvironment().GetHandlesSetPtr()->GetAnnotationMode() == HandlesSet::ANNOTATION_RECTANGLE)
    {
      (GetEnvironment().GetHandlesSetPtr())->m_currentMeasurement = HandlesSetIterator(NULL);
      // Rectangle annotation mode selected.
      AnnotationRectangle *rect = new AnnotationRectangle;
      pMeasurement = rect;
      HandlesObj *pHandlesObj = pMeasurement;

      // set this position as the last mouse position which will be used in the move event.
      SetLastMousePosition(screenPos); 
      
      // get the end points of the axes in world coordinates.
      Point<float4> majorAxisPt;
      if (GetWorldCoordinates(Point2D<int2>(screenPos.m_x + 2.0, screenPos.m_y), majorAxisPt) == false)
        return false;
      Point<float4> minorAxisPt;
      if (GetWorldCoordinates(Point2D<int2>(screenPos.m_x, screenPos.m_y + 2.0), minorAxisPt) == false)
        return false;
      
      // Get the major and minor axes in world coordinates.
      Vector<float4> majorAxis = majorAxisPt - mouseWorldCoord;
      Vector<float4> minorAxis = minorAxisPt - mouseWorldCoord;

      // create a rectangle annotation.
      rect->CreateRectangle(mouseWorldCoord, majorAxis, minorAxis);                             
      // calculate rectangle statistics.
      rect->CalculateRectangleStatistics(static_cast<vxBlockVolume<uint2> *>(GetEnvironment().GetIntensityVolumePtr()));

      m_bFlippedVertical = false;  m_bFlippedHorizontal = false;
      m_bCrossHorz = false; m_bCrossVert = false;
            
      // store the annotation in the list.
      (GetEnvironment().GetHandlesSetPtr())->push_front(pHandlesObj);
      (GetEnvironment().GetHandlesSetPtr())->m_currentMeasurement = (GetEnvironment().GetHandlesSetPtr())->begin();
      bResult = true;
    } // End Rectangle Annotation.
    
    // ARROW Annotation.
    else if ((GetEnvironment().GetHandlesSetPtr())->GetAnnotationMode() == HandlesSet::ANNOTATION_ARROW)
    {
      (GetEnvironment().GetHandlesSetPtr())->m_currentMeasurement = HandlesSetIterator(NULL);
      AnnotationArrow *arrow = new AnnotationArrow;
      pMeasurement = arrow;
      HandlesObj *pHandlesObj = pMeasurement;        
      
      // getting the end point of the arrow, such that the arrow by default makes a 45 deg. angle with the
      // X-axis.
      // get the world coordinates of the end point of the arrow.
      Point<float4>arrowTailPt3D;
      if (GetWorldCoordinates(Point2D<int2>(screenPos.m_x + 30.0, screenPos.m_y +30.0), arrowTailPt3D) == false)
        return false;

      // direction of arrow.
      Vector<float4> dir(Vector<float4>(arrowTailPt3D - mouseWorldCoord));

      // 258 is the size of the smallest volume i came across and i hardcoded 12 based on the small volume.
      // so for different volumes, length = size of the volume /258 * 12. this makes the arrow relative 
      // to the volume size and does not incraes of decrease drastically.
      Triple<float4> volSize(static_cast<vxBlockVolume<uint2> *>(GetEnvironment().GetIntensityVolumePtr())->GetHeader().GetWorldDim());
      
      float maxDim(Max(volSize.m_x, volSize.m_y, volSize.m_z));
      float4 fLength(arrow->GetLength() * (maxDim / 258.0));                   

      // create an arrow at the mouse location.
      arrow->CreateArrow(mouseWorldCoord, fLength, Normal<float4>(dir));
      // store the annotation in the annotation list.
      (GetEnvironment().GetHandlesSetPtr())->push_front(pHandlesObj);
      (GetEnvironment().GetHandlesSetPtr())->m_currentMeasurement = (GetEnvironment().GetHandlesSetPtr())->begin();
      
      bResult = true;
    } // end Arrow Annotation.
    
    // LASSO Annotation.
    else if ((GetEnvironment().GetHandlesSetPtr())->GetAnnotationMode() == HandlesSet::ANNOTATION_ROI )
    {
      (GetEnvironment().GetHandlesSetPtr())->m_currentMeasurement = HandlesSetIterator(NULL);
      // Lasso mode selected.
      AnnotationROI *pLasso = new AnnotationROI;
      pMeasurement = pLasso;
      HandlesObj *pHandlesObj = pMeasurement;
      
      // store the point as the starting point of the contour.
      pLasso->AddPoint(mouseWorldCoord);      
      // set the plane orientation.
      pLasso->SetOrientation(GetEnvironment().GetPlaneOrthoPtr()->GetNormal().GetMaxAbsComponentCoord());
      
      // store the annotation in the list.
      (GetEnvironment().GetHandlesSetPtr())->push_front(pHandlesObj);
      (GetEnvironment().GetHandlesSetPtr())->m_currentMeasurement = (GetEnvironment().GetHandlesSetPtr())->begin();
      bResult = true;   
    } // End Lasso mode.
    
    // VOLUME Annotation.
    else if ((GetEnvironment().GetHandlesSetPtr())->GetAnnotationMode() == HandlesSet::ANNOTATION_VOLUME)
    {     
      // check if the label volume is null.
      if (GetEnvironment().GetLabelVolumePtr() == NULL)
      {
        bResult = false;
      }     
      else
      {
        // Get the volume coordinates of the mouse position.
        Triple<float4> units(static_cast<vxBlockVolume<uint2> *>(GetEnvironment().GetIntensityVolumePtr())->GetHeader().GetVoxelUnits());
        Point<float4> volCoord(mouseWorldCoord.m_x / units.m_x, 
                               mouseWorldCoord.m_y / units.m_y,
                               mouseWorldCoord.m_z / units.m_z);
        
        const Vector3D<int4> uvPos(volCoord.m_x + 0.5, volCoord.m_y + 0.5, volCoord.m_z +0.5);
        vxBlockVolumeIterator<uint2> iterDensity(*(static_cast<vxBlockVolume<uint2> *>(GetEnvironment().GetLabelVolumePtr())));
        iterDensity.SetPos(uvPos);
        
        if (iterDensity.GetVoxel() > 0)
        {      
          // check if there has been a volume annotation performed with the same label as this seed point.
          // if so return.
          bool bFound = CheckForSimilarPreviousVOI(uvPos);
                  
          if(bFound == true)
          {
            return bIfHandled;
          }
          
          AnnotationVOI *voi = new AnnotationVOI;
          pMeasurement = voi;
          HandlesObj *pHandlesObj = pMeasurement;

          vxMouseCursor cursor("CURSOR_WAIT");
          // find out the volume from the seed point and tag volume.      
          voi->CalculateVOI(uvPos, static_cast<vxBlockVolume<uint2> *>(GetEnvironment().GetLabelVolumePtr()),
                            static_cast<vxBlockVolume<uint2> *>(GetEnvironment().GetIntensityVolumePtr()));           
          
          // store the annotation in the annotation list.
          (GetEnvironment().GetHandlesSetPtr())->push_front(pHandlesObj);
          (GetEnvironment().GetHandlesSetPtr())->m_currentMeasurement = (GetEnvironment().GetHandlesSetPtr())->begin();        
          bResult = true; 

        }// end if value at the position is not 0.
        else
        {     
          bResult = false;
        }
      } // end tag volume is valid.
    } // end Volume Annotation mode.
    
    if (bResult == true)
    {
      HandlesObj *pHObj = *((GetEnvironment().GetHandlesSetPtr())->m_currentMeasurement);
      Annotation *pAnno = reinterpret_cast<Annotation *>(pHObj);
      pAnno->SetAdjustStatus(Annotation::MEASUREMENT_CREATE);
      
      // setting a dummy label for the current annotation created.
      char str[10] = "Default";
      std::string sLabel(str);
      pAnno->SetAnnotationLabelText(sLabel);

      GetEnvironment().GetHandlesSetPtr()->Modified(vxModIndInfo(HandlesSet, 0, HandlesSet::EVENT_ADDED));
      bIfHandled = true;
    }
  } // if left button.

  return bIfHandled;
} // OnButtonDown()


/**
 * Event handler if the mouse is moved.
 * @param position Relative Mouse position.
 * @param uKeyboardState mouse state.
 * @return void.
 */ 
bool vxManipulatorHandlesObjCreate::OnMouseMove(const Point2D<int2> & position, const uint4 uKeyboardState)
{
  bool bIfHandled(false);
  //    bIfHandled = true;
  //return if list is empty.
  if ((GetEnvironment().GetHandlesSetPtr())->m_currentMeasurement == HandlesSetIterator(NULL) )
  {
    return bIfHandled;
  }  

  HandlesObj *pHandlesObj = *((GetEnvironment().GetHandlesSetPtr())->m_currentMeasurement);
  Annotation *pMeasurement = reinterpret_cast<Annotation *>(pHandlesObj);

  // return if this move event was not a continuation of the button down event in creation of an annotation.
  if (pMeasurement->GetAdjustStatus() != Annotation::MEASUREMENT_CREATE)
  {
    return bIfHandled;
  }
  
  glReadBuffer(GL_BACK);

  Point2D<int2> screenPos(position);
  screenPos.m_x = Bound((int2)(GetEnvironment().GetWinRect().GetReferencePoint().first), (int2)screenPos.m_x, 
                        (int2)(GetEnvironment().GetWinRect().GetViewportSize().first - 1));
  screenPos.m_y = Bound((int2)(GetEnvironment().GetWinRect().GetReferencePoint().second), (int2)screenPos.m_y, 
                        (int2)(GetEnvironment().GetWinRect().GetViewportSize().second - 1));
  
  // Get the world coordinates of the mouse position.
  Point<float4> mouseWorldCoord;
  if (GetWorldCoordinates(screenPos, mouseWorldCoord, false) == false)
    return false;
  
  // ANGLE Annotation.
  if((GetEnvironment().GetHandlesSetPtr())->GetAnnotationMode() == HandlesSet::ANNOTATION_ANGLE)
  {
    // return if the annotation is not of type angle.
    if(typeid(*pMeasurement) != typeid(AnnotationAngle))
    {
      return bIfHandled;
    }
    AnnotationAngle *ameasurement = reinterpret_cast<AnnotationAngle*>(pMeasurement);
    
    // setting the mouse position as the third point, or end point of the second line segment.
    if(ameasurement->GetUsedPoints() > 1)
    {
      ameasurement->SetFinalPoint(mouseWorldCoord);   
      ameasurement->CalculateAngle();
      if(ameasurement->GetUsedPoints() != 3)
        ameasurement->SetUsedPoints(3);
    }
    // setting the mouse position as the center point.
    else if(ameasurement->GetUsedPoints() == 1)
    {    
      Annotation::LineEnds<Point<float4> > measureEnds;      
      measureEnds.m_end = mouseWorldCoord;      
      // take the new location and store it as the center point of the angle, where the angle is subtended.
      ameasurement->SetFirstLineSegmentEndPoints(Annotation::MEASUREFUNC_MOVE, measureEnds);
    }
    // Refresh all the views so as to incorporate the changes occured in the move event.
    GetEnvironment().GetHandlesSetPtr()->Modified(vxModIndInfo(HandlesSet, 0, HandlesSet::EVENT_MODIFIED));
    bIfHandled = true;
  } // End Angle Annotation.
  
  // LINEAR Annotation.
  if ((GetEnvironment().GetHandlesSetPtr())->GetAnnotationMode() == HandlesSet::ANNOTATION_LINEAR )
  {
    // return if the current measurement is not linear.
    if ( typeid(*pMeasurement) != typeid(AnnotationLinear) )
      return bIfHandled;
    
    Annotation::LineEnds<Point<float4> > measureEnds;      
    measureEnds.m_end = mouseWorldCoord;
    AnnotationLinear *lmeasurement = reinterpret_cast<AnnotationLinear *>(pMeasurement);
    // take the new location and recalculate the distance.
    lmeasurement->CalculateLinear(Annotation::MEASUREFUNC_MOVE, measureEnds);
    
    // Refresh all the views so as to incorporate the changes occured in the move event.
    GetEnvironment().GetHandlesSetPtr()->Modified(vxModIndInfo(HandlesSet, 0, HandlesSet::EVENT_MODIFIED));
    bIfHandled = true;
  }// end linear mode.
  
  
  // ELLIPSE Annotation.
  if ((GetEnvironment().GetHandlesSetPtr())->GetAnnotationMode() == HandlesSet::ANNOTATION_ELLIPSE )
  {    
    // return if the current Annotation is not Ellipse.
    if (typeid(*pMeasurement) != typeid(AnnotationEllipse))
    {
      return bIfHandled;
    }
    AnnotationEllipse *rmeasurement = reinterpret_cast<AnnotationEllipse *>(pMeasurement);
    
    // get the major and minor axes of the ellipse.
    Vector<float4> majorAxis(rmeasurement->GetMajorAxis());
    Vector<float4> minorAxis(rmeasurement->GetMinorAxis());

    // modify the axes depending on the slice orientation.
    RectEllipseOnMouseMove(pMeasurement, majorAxis, minorAxis, rmeasurement->GetTopLeftPoint(), mouseWorldCoord);

    // set the last mouse position to be used in the move event here.
    SetLastMousePosition(screenPos);

     // create an ellipse annotation.
    rmeasurement->CreateEllipse(rmeasurement->GetTopLeftPoint(), majorAxis, minorAxis);
    rmeasurement->CalculateEllipseStatistics(static_cast<vxBlockVolume<uint2> *>(GetEnvironment().GetIntensityVolumePtr()));
        
    // Refresh all the views so as to incorporate the changes occured in the move event.
    GetEnvironment().GetHandlesSetPtr()->Modified(vxModIndInfo(HandlesSet, 0, HandlesSet::EVENT_MODIFIED));
    bIfHandled = true;
  }// end Ellipse mode.
  
  // RECTANGLE Annotation.
  if ((GetEnvironment().GetHandlesSetPtr())->GetAnnotationMode() == HandlesSet::ANNOTATION_RECTANGLE )
  {    
    // return if the current Annotation is not Rectangle.
    if (typeid(*pMeasurement) != typeid(AnnotationRectangle))
    {
      return bIfHandled;
    }
    AnnotationRectangle *rect = reinterpret_cast<AnnotationRectangle *>(pMeasurement);
   
    // get the major and minor axes of the rectangle.
    Vector<float4> majorAxis(rect->GetMajorAxis());
    Vector<float4> minorAxis(rect->GetMinorAxis());

    // modify the axes depending on the slice orientation.
    RectEllipseOnMouseMove(pMeasurement, majorAxis, minorAxis, rect->GetTopLeftPoint(), mouseWorldCoord);

     // set the last mouse position to be used in the move event here.
    SetLastMousePosition(screenPos);

    // create an ellipse annotation.
    rect->CreateRectangle(rect->GetTopLeftPoint(), majorAxis, minorAxis);
    rect->CalculateRectangleStatistics(static_cast<vxBlockVolume<uint2> *>(GetEnvironment().GetIntensityVolumePtr()));
        
    // Refresh all the views so as to incorporate the changes occured in the move event.
    GetEnvironment().GetHandlesSetPtr()->Modified(vxModIndInfo(HandlesSet, 0, HandlesSet::EVENT_MODIFIED));
    bIfHandled = true;
  }// end Rectangle mode.

  // ARROW Annotation.
  if ((GetEnvironment().GetHandlesSetPtr())->GetAnnotationMode() == HandlesSet::ANNOTATION_ARROW)
  {  
    // return if the current measurement is not ROI.
    if (typeid(*pMeasurement) != typeid(AnnotationArrow))
    {
      return bIfHandled;
    }
    AnnotationArrow * arrow = reinterpret_cast<AnnotationArrow*>(pMeasurement);
    
    Point2D<int2> arrowTailPt;      
   
    // getting the end point of the arrow. 
    // perform boundary checks with the viewport to get the correct direction of the arrow at the boundary.
    if (screenPos.m_y > (GetEnvironment().GetWinRect().GetViewportSize().second - 30))
      arrowTailPt = Point2D<int2>(screenPos.m_x + 30.0, screenPos.m_y - 30.0);
    else if (screenPos.m_x > (GetEnvironment().GetWinRect().GetViewportSize().first - 30))
      arrowTailPt = Point2D<int2>(screenPos.m_x - 30.0, screenPos.m_y);
    else
      arrowTailPt = Point2D<int2>(screenPos.m_x + 30.0, screenPos.m_y + 30.0);

    Point<float4> arrowTailPt3D;
    if (GetWorldCoordinates(arrowTailPt, arrowTailPt3D) == false)
      return false;

    // get the direction and length of the arrow.
    Vector<float4> dir(Vector<float4>(arrowTailPt3D - mouseWorldCoord));                
   
    // relocate the arrow to the desired location.
    arrow->CreateArrow(mouseWorldCoord, arrow->GetLength(), Normal<float4>(dir));
    
    // Refresh all the views so as to incorporate the changes occured in the move event.
    GetEnvironment().GetHandlesSetPtr()->Modified(vxModIndInfo(HandlesSet, 0, HandlesSet::EVENT_MODIFIED));
    bIfHandled = true;
  }// end arrow annotation mode.
  
  
  // POINT Annotation.
  if ((GetEnvironment().GetHandlesSetPtr())->GetAnnotationMode() == HandlesSet::ANNOTATION_POINT)
  {
    // return if the current measurement is not Point.
    if (typeid(*pMeasurement) != typeid(AnnotationPoint))
    {
      return bIfHandled;
    }
    AnnotationPoint * point = reinterpret_cast<AnnotationPoint*>(pMeasurement);

    // Move the sphere to the new location.
    point->CreatePoint(mouseWorldCoord);
    
    // Refresh all the views so as to incorporate the changes occured in the move event.
    GetEnvironment().GetHandlesSetPtr()->Modified(vxModIndInfo(HandlesSet, 0, HandlesSet::EVENT_MODIFIED));
    bIfHandled = true;
  }// end point annotation mode.
  
  
  // LASSO Annotation.
  if ((GetEnvironment().GetHandlesSetPtr())->GetAnnotationMode() == HandlesSet::ANNOTATION_ROI)
  {
    // return if the current measurement is not lasso.
    if (typeid(*pMeasurement) != typeid(AnnotationROI))
      return bIfHandled;
    
    AnnotationROI *pLasso = reinterpret_cast<AnnotationROI *>(pMeasurement);
    // add the point to the contour.
    pLasso->AddPoint(mouseWorldCoord);
    
    // Refresh all the views so as to incorporate the changes occured in the move event.
    GetEnvironment().GetHandlesSetPtr()->Modified(vxModIndInfo(HandlesSet, 0, HandlesSet::EVENT_MODIFIED));
    bIfHandled = true;
  }// end area mode.

  return bIfHandled;
} // OnMouseMove()


/**
 * Get the major and minor axes depending on the mouse movement in 
 * creation of ellipse or rectangle.
 * @param pMeasurement the annotation object.
 * @param vMajorAxis major axis of ellipse or rectangle.
 * @param vMinorAxis minor axis of ellipse or rectangle.
 * @param fLeftTopPoint left top corner of ellipse or rectangle.
 * @param mousePos mouse position in world coordinates.
 * @return modified major and minor axes.
 */
void vxManipulatorHandlesObjCreate::RectEllipseOnMouseMove(Annotation *pMeasurement, Vector<float4> & vMajorAxis,
                                                           Vector<float4> & vMinorAxis, Point<float4> fLeftTopPoint, 
                                                           Point<float4> mousePos)
{  
  // get the last mouse position.
  Point<float4> lastPosWorld;
  if (GetWorldCoordinates(GetLastMousePosition(), lastPosWorld) == false)
    return;
  
  // get the plane orientation.
  uint4 uIndex(GetEnvironment().GetPlaneOrthoPtr()->GetNormal().GetMaxAbsComponentCoord());
 
  switch(uIndex)
  {
  case 0: // SAGITTAL
    RectEllipseSagittal(pMeasurement, mousePos, lastPosWorld, vMajorAxis, vMinorAxis);
  break;
  case 1: // CORONAL
    RectEllipseCoronal(pMeasurement, mousePos, lastPosWorld, vMajorAxis, vMinorAxis);
  break;
  case 2:   // AXIAL.  
    RectEllipseAxial(pMeasurement, mousePos, lastPosWorld, vMajorAxis, vMinorAxis);
  break;
  }
} // RectEllipseOnMouseMove()


/**
 * For Sagittal plane get the major and minor axes depending on the mouse movement in 
 * creation of ellipse or rectangle . 
 * @param pMeasurement the annotation object.
 * @param mousePos mouse position in world coordinates.
 * @param lastPosWorld last mouse position in world coordinates.
 * @param vMajorAxis major axis of ellipse or rectangle.
 * @param vMinorAxis minor axis of ellipse or rectangle.
 * @return modified major and minor axes.
 */
void vxManipulatorHandlesObjCreate::RectEllipseSagittal(Annotation * pMeasurement, const Point<float4> & mousePos, 
                                                        const Point<float4> & lastPosWorld, 
                                                        Vector<float4> & vMajorAxis, Vector<float4> & vMinorAxis)
{
  AnnotationRectangle *rect = NULL;
  AnnotationEllipse *ellipse = NULL;

  Point<float4> fLeftTopPoint;
  uint1 uType(0);
  if(typeid(*pMeasurement) == typeid(AnnotationRectangle))
  {
    rect = static_cast<AnnotationRectangle*>(pMeasurement);
    fLeftTopPoint = rect->GetTopLeftPoint();
    uType = 1;
  }
  if(typeid(*pMeasurement) == typeid(AnnotationEllipse))
  {
    ellipse = static_cast<AnnotationEllipse*>(pMeasurement);
    fLeftTopPoint = ellipse->GetTopLeftPoint();
    uType = 2;
  }

  // stor the original major and minor axes.
  Vector<float4> vOrigMajorAxis(vMajorAxis), vOrigMinorAxis(vMinorAxis);
  // difference between the current and previous mouse position.
  Vector<float4> diff(mousePos - lastPosWorld);
  
  if(fabs(mousePos.m_y - lastPosWorld.m_y) > 0.001)
  {
    if((mousePos.m_y > fLeftTopPoint.m_y) && (mousePos.m_z <= fLeftTopPoint.m_z))       
    { 
      if(m_bFlippedHorizontal == false) vMajorAxis = vOrigMajorAxis + Vector<float4>(0.0, diff.m_y, 0.0);       
      else
      {
        vMajorAxis = vOrigMajorAxis - Vector<float4>(0.0, diff.m_y, 0.0);
        if(((mousePos.m_y - pMeasurement->GetHandlePosition(2).m_y) > 0.0 ||
          (mousePos.m_y - pMeasurement->GetHandlePosition(4).m_y) > 0.0) && m_bCrossHorz == false)
        {
          m_bFlippedHorizontal = false;
          m_bCrossHorz = true;
        }
        else
        {
          if(uType == 1) rect->SetTopLeftPoint(Point<float4>(rect->GetHandlePosition(2) - vMajorAxis)); 
          else if(uType == 2) ellipse->SetTopLeftPoint(ellipse->GetHandlePosition(2) - vMajorAxis);            
        }
      }
    } 
    // flip the rectangle in horizontal direction.
    if(mousePos.m_y < fLeftTopPoint.m_y)
    {
      vMajorAxis = vOrigMajorAxis - Vector<float4>(0.0, diff.m_y, 0.0);
      if(uType == 1) rect->SetTopLeftPoint(rect->GetHandlePosition(2) - vMajorAxis);
      else if(uType == 2) ellipse->SetTopLeftPoint(ellipse->GetHandlePosition(2) - vMajorAxis);
      m_bFlippedHorizontal = true;
      m_bCrossHorz = false;
    }
  }
  // modify minor axis.
  if(fabs(mousePos.m_z - lastPosWorld.m_z) > 0.001)
  {
    if((mousePos.m_z < fLeftTopPoint.m_z) && (mousePos.m_y >= fLeftTopPoint.m_y))       
    {  
      if(m_bFlippedVertical == false) vMinorAxis = vOrigMinorAxis + Vector<float4>(0.0, 0.0, diff.m_z);         
      else
      {
        vMinorAxis = vOrigMinorAxis - Vector<float4>(0.0, 0.0, diff.m_z);
        if((mousePos.m_z - pMeasurement->GetHandlePosition(4).m_z) < 0.0 && m_bCrossVert == false)
        {
          m_bFlippedVertical = false;
          m_bCrossVert = true;
        }
        else
        {
          if(uType == 1) rect->SetTopLeftPoint(rect->GetHandlePosition(6) - vMinorAxis);
          else if(uType == 2) ellipse->SetTopLeftPoint(ellipse->GetHandlePosition(6) - vMinorAxis);
        }
      }
    }   
    // flip the object in the vertical direction.
    if(mousePos.m_z > fLeftTopPoint.m_z)
    {
      vMinorAxis = vOrigMinorAxis - Vector<float4>(0.0, 0.0, diff.m_z);
      if(uType == 1) rect->SetTopLeftPoint(rect->GetHandlePosition(6) - vMinorAxis);
      else if(uType == 2) ellipse->SetTopLeftPoint(ellipse->GetHandlePosition(6) - vMinorAxis);
      m_bFlippedVertical = true;
      m_bCrossVert = false;
    }
  }// end else if.  
} // RectEllipseSagittal()


/**
 * For Coronal plane get the major and minor axes depending on the mouse movement in 
 * creation of ellipse or rectangle . 
 * @param pMeasurement the annotation object.
 * @param mousePos mouse position in world coordinates.
 * @param lastPosWorld last mouse position in world coordinates.
 * @param vMajorAxis major axis of ellipse or rectangle.
 * @param vMinorAxis minor axis of ellipse or rectangle.
 * @return modified major and minor axes.
 */
void vxManipulatorHandlesObjCreate::RectEllipseCoronal(Annotation * pMeasurement, const Point<float4> & mousePos, 
                                                      const Point<float4> & lastPosWorld, Vector<float4> & vMajorAxis,
                                                      Vector<float4> & vMinorAxis)
{
  AnnotationRectangle *rect = NULL;
  AnnotationEllipse *ellipse = NULL;

  Point<float4> fLeftTopPoint;
  uint1 uType(0);
  if(typeid(*pMeasurement) == typeid(AnnotationRectangle))
  {
    rect = static_cast<AnnotationRectangle*>(pMeasurement);
    fLeftTopPoint = rect->GetTopLeftPoint();
    uType = 1;
  }
  if(typeid(*pMeasurement) == typeid(AnnotationEllipse))
  {
    ellipse = static_cast<AnnotationEllipse*>(pMeasurement);
    fLeftTopPoint = ellipse->GetTopLeftPoint();
    uType = 2;
  }

  // stor the original major and minor axes.
  Vector<float4> vOrigMajorAxis(vMajorAxis), vOrigMinorAxis(vMinorAxis);
  // difference between the current and previous mouse position.
  Vector<float4> diff(mousePos - lastPosWorld);
  
   // modify major axis.
  if(fabs(mousePos.m_x - lastPosWorld.m_x) > 0.001)
  {
    if((mousePos.m_x > fLeftTopPoint.m_x) && (mousePos.m_z <= fLeftTopPoint.m_z))       
    { 
      if(m_bFlippedHorizontal == false) vMajorAxis = vOrigMajorAxis + Vector<float4>(diff.m_x, 0.0, 0.0);       
      else
      {
        vMajorAxis = vOrigMajorAxis - Vector<float4>(diff.m_x, 0.0, 0.0);
        if(((mousePos.m_x - pMeasurement->GetHandlePosition(2).m_x) > 0.0 || 
            (mousePos.m_x - pMeasurement->GetHandlePosition(4).m_x) > 0.0) && m_bCrossHorz == false)
        {
          m_bFlippedHorizontal = false;
          m_bCrossHorz = true;
        }
        else
        {
          if(uType == 1) rect->SetTopLeftPoint(rect->GetHandlePosition(2) - vMajorAxis);        
          else if (uType == 2) ellipse->SetTopLeftPoint(ellipse->GetHandlePosition(2) - vMajorAxis);
        }
      }
    } 
    // flip the rectangle in horizontal direction.
    if(mousePos.m_x < fLeftTopPoint.m_x)
    {
      vMajorAxis = vOrigMajorAxis - Vector<float4>(diff.m_x, 0.0, 0.0);
      if(uType == 1) rect->SetTopLeftPoint(rect->GetHandlePosition(2) - vMajorAxis);
      else if(uType == 2) ellipse->SetTopLeftPoint(ellipse->GetHandlePosition(2) - vMajorAxis);
      m_bFlippedHorizontal = true;
      m_bCrossHorz = false;
    }
  }
  // modify minor axis.
  if(fabs(mousePos.m_z - lastPosWorld.m_z) > 0.001)
  {
    if((mousePos.m_z < fLeftTopPoint.m_z) && (mousePos.m_y >= fLeftTopPoint.m_y))       
    {  
      if(m_bFlippedVertical == false) vMinorAxis = vOrigMinorAxis + Vector<float4>(0.0, 0.0, diff.m_z);         
      else
      {
        vMinorAxis = vOrigMinorAxis - Vector<float4>(0.0, 0.0, diff.m_z);
        if((mousePos.m_z - pMeasurement->GetHandlePosition(4).m_z) < 0.0 && m_bCrossVert == false)
        {
          m_bFlippedVertical = false;
          m_bCrossVert = true;
        }
        else
        {
          if(uType == 1) rect->SetTopLeftPoint(rect->GetHandlePosition(6) - vMinorAxis);
          else if(uType == 2) ellipse->SetTopLeftPoint(ellipse->GetHandlePosition(6) - vMinorAxis);
        }
      }
    }  
      
    // flip the object in the vertical direction.
    if(mousePos.m_z > fLeftTopPoint.m_z)
    {
      vMinorAxis = vOrigMinorAxis - Vector<float4>(0.0, 0.0, diff.m_z);
      if(uType == 1) rect->SetTopLeftPoint(rect->GetHandlePosition(6) - vMinorAxis);
      else if (uType == 2) ellipse->SetTopLeftPoint(ellipse->GetHandlePosition(6) - vMinorAxis);
      m_bFlippedVertical = true;
      m_bCrossVert = false;
    }
  }// end else if.  
} // RectEllipseCoronal()


/**
 * For Axial plane get the major and minor axes depending on the mouse movement in 
 * creation of ellipse or rectangle . 
 * @param pMeasurement the annotation object.
 * @param mousePos mouse position in world coordinates.
 * @param lastPosWorld last mouse position in world coordinates.
 * @param vMajorAxis major axis of ellipse or rectangle.
 * @param vMinorAxis minor axis of ellipse or rectangle.
 * @return modified major and minor axes.
 */
void vxManipulatorHandlesObjCreate::RectEllipseAxial(Annotation * pMeasurement, const Point<float4> & mousePos, 
                                                     const Point<float4> & lastPosWorld, Vector<float4> & vMajorAxis,
                                                     Vector<float4> & vMinorAxis)
{
  AnnotationRectangle *rect = NULL;
  AnnotationEllipse *ellipse = NULL;

  Point<float4> fLeftTopPoint;

  uint1 uType(0);
  if(typeid(*pMeasurement) == typeid(AnnotationRectangle))
  {
    rect = static_cast<AnnotationRectangle*>(pMeasurement);
    fLeftTopPoint = rect->GetTopLeftPoint();
    uType = 1;
  }
  if(typeid(*pMeasurement) == typeid(AnnotationEllipse))
  {
    ellipse = static_cast<AnnotationEllipse*>(pMeasurement);
    fLeftTopPoint = ellipse->GetTopLeftPoint();
    uType = 2;
  }

  // stor the original major and minor axes.
  Vector<float4> vOrigMajorAxis(vMajorAxis), vOrigMinorAxis(vMinorAxis);
  // difference between the current and previous mouse position.
  Vector<float4> diff(mousePos - lastPosWorld);
    
  // modify major axis.
  if(fabs(mousePos.m_x - lastPosWorld.m_x) > 0.001)
  {
    if((mousePos.m_x >= fLeftTopPoint.m_x) && (mousePos.m_y >= fLeftTopPoint.m_y))       
    { 
      if(m_bFlippedHorizontal == false) vMajorAxis = vOrigMajorAxis + Vector<float4>(diff.m_x, 0.0, 0.0);       
      else
      {
        vMajorAxis = vOrigMajorAxis - Vector<float4>(diff.m_x, 0.0, 0.0);
        if(((mousePos.m_x - pMeasurement->GetHandlePosition(2).m_x) > 0.0 ||
             (mousePos.m_x - pMeasurement->GetHandlePosition(4).m_x) > 0.0) && m_bCrossHorz == false)
        {           
          m_bFlippedHorizontal = false;
          m_bCrossHorz = true;
        }
        else
        {
          if(uType == 1) rect->SetTopLeftPoint(rect->GetHandlePosition(2) - vMajorAxis);                                        
          else if(uType == 2) ellipse->SetTopLeftPoint(ellipse->GetHandlePosition(2) - vMajorAxis);        
        }
      } 
    } 
   
    // flip the rectangle in horizontal direction.
    if(mousePos.m_x < fLeftTopPoint.m_x)
    {
      vMajorAxis = vOrigMajorAxis - Vector<float4>(diff.m_x, 0.0, 0.0);
      
      if(uType == 1) rect->SetTopLeftPoint(rect->GetHandlePosition(2) - vMajorAxis);      
      else if(uType == 2) ellipse->SetTopLeftPoint(ellipse->GetHandlePosition(2) - vMajorAxis);
      
      m_bFlippedHorizontal = true;
      m_bCrossHorz = false;
    }
  }
  // modify minor axis.
  if(fabs(mousePos.m_y - lastPosWorld.m_y) > 0.001)
  {
    if((mousePos.m_y >= fLeftTopPoint.m_y) && (mousePos.m_x >= fLeftTopPoint.m_x))       
    {  
      if(m_bFlippedVertical == false) 
      {
        vMinorAxis = vOrigMinorAxis + Vector<float4>(0.0, diff.m_y, 0.0);                 
      }
      else
      {        
        vMinorAxis = vOrigMinorAxis - Vector<float4>(0.0, diff.m_y, 0.0);
        if((mousePos.m_y - pMeasurement->GetHandlePosition(4).m_y) > 0.0 && m_bCrossVert == false)
        {           
          m_bFlippedVertical = false;
          m_bCrossVert = true;
        }
        else
        {
          if(uType == 1) rect->SetTopLeftPoint(rect->GetHandlePosition(6) - vMinorAxis);                                                    
          else if(uType == 2) ellipse->SetTopLeftPoint(ellipse->GetHandlePosition(6) - vMinorAxis);            
        }
      }         
    }
     
    // flip the object in the vertical direction.
    if(mousePos.m_y < fLeftTopPoint.m_y)
    {
      vMinorAxis = vOrigMinorAxis - Vector<float4>(0.0, diff.m_y, 0.0);
      
      if(uType == 1) rect->SetTopLeftPoint(rect->GetHandlePosition(6) - vMinorAxis);      
      
      else if(uType == 2) ellipse->SetTopLeftPoint(ellipse->GetHandlePosition(6) - vMinorAxis);      
      
      m_bFlippedVertical = true;
      m_bCrossVert = false;
    }
  }// end else if.  
} // RectEllipseAxial()


/**
 * Check if there has been a volume annotation performed with the same label as this seed point.
 * @param fCurSeedPoint. the current seed point.
 * @return true if similar VOI is found, else false.
 */
bool vxManipulatorHandlesObjCreate::CheckForSimilarPreviousVOI(const Vector3D<int4> & vCurSeedPoint)
{
  bool bResult(false);

  HandlesSetIterator listStart = HandlesSetIterator(NULL);
  HandlesSetIterator listEnd = GetEnvironment().GetHandlesSetPtr()->end();

  // get the label of the current seed point.
  vxBlockVolumeIterator<uint2> iterLabel(*static_cast<vxBlockVolume<uint2> *>(GetEnvironment().GetLabelVolumePtr()));
  iterLabel.SetPos(vCurSeedPoint);
  float4 fLabelValue(iterLabel.GetVoxel());

  // go over the entire list to find the previous similar VOIs.
  for(listStart = GetEnvironment().GetHandlesSetPtr()->begin(); listStart != listEnd; listStart++)
  {
    HandlesObj *hObj = *listStart;
    if(typeid(*hObj) == typeid(AnnotationVOI))
    {
      AnnotationVOI *voi = reinterpret_cast<AnnotationVOI*>(hObj);
      Point<uint2> uSeedPos(voi->GetSeedPoint().m_x, voi->GetSeedPoint().m_y, voi->GetSeedPoint().m_z);
      iterLabel.SetPos(Vector3D<int4>(uSeedPos.m_x, uSeedPos.m_y, uSeedPos.m_z));

      if(iterLabel.GetVoxel() == fLabelValue)
      {
        bResult = true;
        break;
      }
    }      
  }
  return bResult;
} // CheckForSimilarPreviousVOI()


/**
 * Event handler if the mouse is released.
 * @param position mouse position.
 * @param eButtonId mouse button.
 * @param uKeyboardState mouse state.
 * @return void.
 */ 
bool vxManipulatorHandlesObjCreate::OnButtonUp(const Point2D<int2> & position, const ButtonEnum eButtonId,
                                               const uint4 uKeyboardState)
{
  bool bIfHandled(false);
  //    bIfHandled = true;
  if ( eButtonId == BUTTON_LEFT )
  {
    // if the annotations list is not empty.
    if (!(GetEnvironment().GetHandlesSetPtr())->empty())
    {      
      if ((GetEnvironment().GetHandlesSetPtr())->m_currentMeasurement != HandlesSet::iterator(NULL) )
      {   
        // bound the mouse position to the viewport.
        Point2D<int2> screenPos(position);
        screenPos.m_x = Bound((int2)(GetEnvironment().GetWinRect().GetReferencePoint().first), (int2)(screenPos.m_x), 
                              (int2)(GetEnvironment().GetWinRect().GetViewportSize().first));
        screenPos.m_y = Bound((int2)(GetEnvironment().GetWinRect().GetReferencePoint().second), (int2)(screenPos.m_y), 
                              (int2)(GetEnvironment().GetWinRect().GetViewportSize().second));

        Annotation *pMeasurement = reinterpret_cast<Annotation *>(*(GetEnvironment().GetHandlesSetPtr()->m_currentMeasurement));
        if(typeid(*pMeasurement) == typeid(AnnotationRectangle))
        {
          AnnotationRectangle * rect = reinterpret_cast<AnnotationRectangle *>(pMeasurement);
          if(rect->GetMajorDiameter() < 5.3F)
          {
            Point<float4> mouseWorld;
            if (GetWorldCoordinates(screenPos, mouseWorld) == false)
              return false;

            if(fabs(mouseWorld.m_x - rect->GetTopLeftPoint().m_x) < 0.001 &&
               fabs(mouseWorld.m_y - rect->GetTopLeftPoint().m_y) < 0.001 &&
               fabs(mouseWorld.m_z - rect->GetTopLeftPoint().m_z) < 0.001)
            { 
              // get the end points of the axes in world coordinates.
              Point<float4> majorAxisPt;
              if (GetWorldCoordinates(Point2D<int2>(screenPos.m_x + 20, screenPos.m_y), majorAxisPt) == false)
                return false;
              Point<float4> minorAxisPt;
              if (GetWorldCoordinates(Point2D<int2>(screenPos.m_x, screenPos.m_y + 20), minorAxisPt) == false)
                return false;
      
              // Get the major and minor axes in world coordinates.
              Vector<float4> vMajorAxis = majorAxisPt - mouseWorld;
              Vector<float4> vMinorAxis = minorAxisPt - mouseWorld;
            
              rect->CreateRectangle(rect->GetTopLeftPoint(), Vector<float4>(vMajorAxis), Vector<float4>(vMinorAxis));
              rect->CalculateRectangleStatistics(static_cast<vxBlockVolume<uint2> *>(GetEnvironment().GetIntensityVolumePtr()));
            }
          }
        }
        if(typeid(*pMeasurement) == typeid(AnnotationEllipse))
        {
          AnnotationEllipse * ellipse = reinterpret_cast<AnnotationEllipse *>(pMeasurement);
          if(ellipse->GetMajorDiameter() < 5.3F)
          {
            Point<float4> mouseWorld;
            if (GetWorldCoordinates(screenPos, mouseWorld) == false)
              return false;
            if(fabs(mouseWorld.m_x - ellipse->GetTopLeftPoint().m_x) < 0.001 &&
               fabs(mouseWorld.m_y - ellipse->GetTopLeftPoint().m_y) < 0.001 &&
               fabs(mouseWorld.m_z - ellipse->GetTopLeftPoint().m_z) < 0.001)
            { 
              // get the end points of the axes in world coordinates.
              Point<float4> majorAxisPt;
              if (GetWorldCoordinates(Point2D<int2>(screenPos.m_x + 20, screenPos.m_y), majorAxisPt) == false)
                return false;
              Point<float4> minorAxisPt;
              if (GetWorldCoordinates(Point2D<int2>(screenPos.m_x, screenPos.m_y + 20), minorAxisPt) == false)
                return false;
      
              // Get the major and minor axes in world coordinates.
              Vector<float4> vMajorAxis = majorAxisPt - mouseWorld;
              Vector<float4> vMinorAxis = minorAxisPt - mouseWorld;
            
              ellipse->CreateEllipse(ellipse->GetTopLeftPoint(), Vector<float4>(vMajorAxis), Vector<float4>(vMinorAxis));
              ellipse->CalculateEllipseStatistics(static_cast<vxBlockVolume<uint2> *>(GetEnvironment().GetIntensityVolumePtr()));
            }
          }
        }
        // Angle annotation.
        if (typeid(*pMeasurement) == typeid(AnnotationAngle))
        {
          if (pMeasurement->GetUsedPoints() == 1)
          {
            Point<float4> worldPos;
            if (GetWorldCoordinates(screenPos, worldPos) == false)
              return false;

            AnnotationAngle *angle = reinterpret_cast<AnnotationAngle*>(pMeasurement);
            angle->SetUsedPoints(2); // center point of the angle has been set.
            //initlialize the end point of the second line segment to be the center point.
            angle->SetFinalPoint(worldPos);
            return bIfHandled;
          }
          if(pMeasurement->GetUsedPoints() == 3)
          {
            (GetEnvironment().GetHandlesSetPtr())->m_currentMeasurement = HandlesSetIterator(NULL);
          }
        } // if angle annotation.
        // Lasso Annotation.
        if(typeid(*pMeasurement) ==typeid(AnnotationROI))
        {          
          AnnotationROI *lasso = reinterpret_cast<AnnotationROI *>(pMeasurement);
          // calculate statsistics and close the contour.
          LassoOnButtonUp(lasso, screenPos);       
        }// if lasso annotation.
        
        pMeasurement->SetAdjustStatus(Annotation::STARTING_ADJUST);

        // notify the feedback so that manipulator could be removed
        GetEnvironment().GetInputFeedback().Add(vxFeedback(typeid(*this), vxFeedback::ActionType::DONE));
        GetEnvironment().GetHandlesSetPtr()->Modified(vxModIndInfo(HandlesSet, 0, HandlesSet::EVENT_COMPLETED));
        bIfHandled = true;
        LogDbg("done with annotation creation", "vxManipulatorHandlesObjCreate", "OnButtonUp");
      }      
    } //  end if list is not empty.
  } //  end if left button.

  return bIfHandled;
} // OnButtonUp()


/**
 * Calculate statsitics and wind up lasso on button up.
 * @param lasso the lasso annotation object.
 * @param position mouse position.
 * @return void.
 */
void vxManipulatorHandlesObjCreate::LassoOnButtonUp(AnnotationROI * pLasso, const Point2D<int2> & position)
{ 
  // get the world coordinates of the mouse position.
  glReadBuffer(GL_BACK);
  Point<float4> worldPos;
  if (GetWorldCoordinates(position, worldPos) == false)
    return;
      
  pLasso->AddPoint(worldPos);
             
  //close the contour
  Point<float4> firstPkt = pLasso->GetPoint(0);  //definitely there is at least one when button down
  pLasso->AddPoint(firstPkt);

  Point<int2> iMaskTopLeft; 
  Image<bool> bMask(false);  
  
  Triple<float4> units(static_cast<vxBlockVolume<uint2> *>(GetEnvironment().GetIntensityVolumePtr())->GetHeader().GetVoxelUnits());


  // calculate statistics of the ROI.
  pLasso->CalculateArea(units, iMaskTopLeft, bMask); 
                      

  // calculate the statistics of the region covered by the lasso.
  pLasso->CalculateStatistics(units, iMaskTopLeft, bMask, 
                             *static_cast<vxBlockVolume<uint2> *>(GetEnvironment().GetIntensityVolumePtr())); 
    

  
   // indicate that the statistics are now calculated and can be displayed.
  pLasso->SetStatFlag(true);
} // LassoOnButtonUp()


/**
 * Event handler if a key is pressed.
 * @param uKeyId the key pressed.
 * @param uKeyboardState key state.
 * @return void.
 */ 
bool vxManipulatorHandlesObjCreate::OnKeyPressed(const uint1 uKeyId, const uint4 uKeyboardState)
{
  bool bIfHandled(false);
  //    bIfHandled = true;
  switch(uKeyId)
  {
  case vxInput::KEY_ESCAPE: // Esc key
    // remove an annotation if this key is pressed while creation of that annotation.
    if(GetEnvironment().GetHandlesSetPtr()->m_currentMeasurement != NULL)
    {
      Annotation *pAnno = reinterpret_cast<Annotation *>(*(GetEnvironment().GetHandlesSetPtr()->m_currentMeasurement));
      if(pAnno->GetAdjustStatus() == Annotation::MEASUREMENT_CREATE)
      {
        // remove the annotation for the list.
        GetEnvironment().GetHandlesSetPtr()->erase(GetEnvironment().GetHandlesSetPtr()->m_currentMeasurement);
        // set the current iterator to null.
        GetEnvironment().GetHandlesSetPtr()->m_currentMeasurement = HandlesSetIterator(NULL);
      
        GetEnvironment().GetHandlesSetPtr()->Modified(vxModIndInfo(HandlesSet, 0, HandlesSet::EVENT_MODIFIED));
      } // end if not in create state. 
    } // if current iterator is not null.
    break;

  case vxInput::KEY_H: // "h" for toggling of histogram display.
    GetEnvironment().GetHandlesSetPtr()->SetProfileFlag(!GetEnvironment().GetHandlesSetPtr()->GetProfileFlag());
  break;
  //////////////////////////////////////////////////////////
  //
  // The following is Unit Test stuff that should move out
  //
  //////////////////////////////////////////////////////////
  /*
  case KEY_M: // "M" key
    // unit test for annotations.
    GetEnvironment().GetHandlesSetPtr()->UnitTest();  
    break;
    */
  } // end switch key
  return bIfHandled;
} // OnKeyPressed()


/**
 * Called when the mouse cursor is to be set
 *
 * @return true if cursor was set, false otherwise
 */
bool vxManipulatorHandlesObjCreate::OnSetCursor()
{
  bool bHandled = true;

  switch (GetEnvironment().GetHandlesSetPtr()->GetAnnotationMode())
  {
  case HandlesSet::ANNOTATION_ANGLE:     vxMouseCursor::SetCursor("CURSOR_ANGLE");          break;
  case HandlesSet::ANNOTATION_ARROW:     vxMouseCursor::SetCursor("CURSOR_ARROW");          break;
  case HandlesSet::ANNOTATION_LINEAR:    vxMouseCursor::SetCursor("CURSOR_LINEAR");         break;
  case HandlesSet::ANNOTATION_POINT:     vxMouseCursor::SetCursor("CURSOR_POINT");          break;
  case HandlesSet::ANNOTATION_ELLIPSE:   vxMouseCursor::SetCursor("CURSOR_ROI");            break;
  case HandlesSet::ANNOTATION_ROI:       vxMouseCursor::SetCursor("CURSOR_LASSO");          break;
  case HandlesSet::ANNOTATION_VOLUME:    vxMouseCursor::SetCursor("CURSOR_VOLUME_MEASURE"); break;
  case HandlesSet::ANNOTATION_RECTANGLE: vxMouseCursor::SetCursor("CURSOR_SQUARE");         break;
  default:
    bHandled = false;    
    break;
  }
  return bHandled;
} // OnSetCursor()



#undef FILE_REVISION



// Revision History:
// $Log: vxManipulatorHandlesObjCreate.C,v $
// Revision 1.1.1.1  2004/03/02 03:58:42  geconomos
// Moved and restructured code from the old xp module. This helps 
// recompiling everything in one go and also reduces the compile
// time for the individual projects due to the new library structure.
//
// Revision 1.52  2004/02/09 15:47:58  dongqing
// replace ID number to Enum name for all keys.
//
// Revision 1.51  2003/06/18 15:19:56  geconomos
// Changed all instances of vxBlockVolume<int2> to vxBlockVolume<uint2>
//
// Revision 1.50  2003/05/16 11:45:25  michael
// code review
//
// Revision 1.49  2003/03/13 16:30:13  frank
// spelling
//
// Revision 1.48  2003/02/28 22:09:34  michael
// removed vxWinRect from vxManipulatorr. Is now only in vxViewer and is set in
// vxEnvironment when needed (during rendering or manipulation).
//
// Revision 1.47  2003/01/22 21:56:41  ingmar
// synchronized to new Point3D Vector3D and Normal3D classes
//
// Revision 1.46  2003/01/13 20:00:19  michael
// fixed bugs in creation of annotations, need to adjust the rendering
//
// Revision 1.45  2003/01/06 17:13:55  michael
// formatting
//
// Revision 1.44  2002/12/20 19:02:12  ingmar
// removed single scalar constructor from PixelRGBA
//
// Revision 1.43  2002/12/17 14:38:56  michael
// OpenGL back projection now returns bool to indicate success or not
//
// Revision 1.42  2002/11/25 13:44:56  geconomos
// New mouse cursor handling.
//
// Revision 1.41  2002/11/15 15:57:53  michael
// some clean up. Commented out the unit test stuff from HandleSet & manipulator.
// This really needs to be moved in a parallel solution for unit tests only
//
// Revision 1.40  2002/11/12 00:11:11  michael
// cosmetics
//
// Revision 1.39  2002/10/03 19:09:45  dmitry
// Rendering mode not concidered when calculating the world coordinates.
//
// Revision 1.38  2002/09/16 20:02:05  dmitry
// Active state manipulated and validated.
//
// Revision 1.37  2002/09/04 19:20:47  dmitry
// RemoveLinearSplitter called in the constructor.
//
// Revision 1.36  2002/08/20 22:33:19  uday
// Manipulator returns true/false  if the event has been handled or not.
//
// Revision 1.35  2002/08/08 21:08:39  dmitry
// EVENT_COMPLETED flag set on modified in button up.
//
// Revision 1.34  2002/07/22 23:17:22  michael
// Merged vxBase of branch "XP_1-0" tagged as "XP_1-0_merge_7-22-2002"
// into the main trunc
//
// Revision 1.33  2002/07/22 18:24:11  michael
// Merged vxBase of branch "XP_1-0" tagged as "XP_1-0_merge_7-15-2002"
// into the main trunc
//
// Revision 1.32.2.1  2002/07/11 22:37:05  michael
// took out the MakeCurrent() since this is ensured by V3D_Viewer and vxViewer
//
// Revision 1.32  2002/06/20 02:19:01  dmitry
// Using MouseCursor from the Environment.
//
// Revision 1.31  2002/06/18 14:57:39  manju
// Got rid of memory leak when creating AnnotationVOI in OnButtonDown
// event.
//
// Revision 1.30  2002/06/14 20:14:03  geconomos
// Changde feedback typeids to be classes from pointer to classes.
//
// Revision 1.29  2002/06/14 18:04:30  manju
// Moved the key press handler for unit test for annotations to
// vxManipulatorHandlesObjCreate.
//
// Revision 1.28  2002/06/14 16:57:02  manju
// Modified key press handler to delete annotation while creation, for the
// feedback mecahnism to work correctly.
//
// Revision 1.27  2002/06/14 15:55:12  geconomos
// Reimplemented mouse cursors.
//
// Revision 1.26  2002/06/14 15:49:49  manju
// Changed the Boundary check in OnButtonDown().
//
// Revision 1.25  2002/06/13 23:22:36  michael
// added feedback information from the manipulators so that they can be removed
// from VB once operation is completed ...
//
// Revision 1.24  2002/06/07 19:36:51  manju
// Modified creation of ellipse and rectangle annotation.
//
// Revision 1.23  2002/06/04 20:10:56  manju
// Changed the shortcut for displaying histogram from 'p' to 'h'.
//
// Revision 1.22  2002/06/04 19:12:41  manju
// Coding standard
//
// Revision 1.21  2002/05/29 19:28:15  manju
// Coding standard.
//
// Revision 1.20  2002/05/23 16:05:49  manju
// Bounding mouse position in OnButtonUp event.
// Changed return type of CreateEllipse and CalculateLinear methods to void.
//
// Revision 1.19  2002/05/20 21:40:07  dmitry
// _BOX changed to _RECTANGLE
//
// Revision 1.18  2002/05/17 20:39:27  manju
// Changing mouse cursor for volume annotation.
//
// Revision 1.17  2002/05/10 15:11:52  manju
// Removed measurement status as argument to CalculateArea.
//
// Revision 1.16  2002/05/08 20:03:36  manju
// Modified the boundary check for the mouse position.
// Removed the checks for ellipse and rectangle annotation in OnMouseMove
// event.
//
// Revision 1.15  2002/05/08 13:33:35  manju
// Modified VOI annotation in OnButtonDown event.
//
// Revision 1.14  2002/05/06 22:16:31  manju
// Modified volume annotation.
//
// Revision 1.13  2002/05/06 19:36:35  manju
// Bounds check for rectangle and ellipse annotation.
//
// Revision 1.12  2002/05/06 14:02:19  manju
// Removed modality as an argument to the CalculateStatistics method for the
// lasso.
//
// Revision 1.11  2002/05/03 18:29:11  manju
// Not passing modality to CalculaetEllipseStatistics method.
//
// Revision 1.10  2002/05/02 18:52:38  manju
// Removed orientation as argument to CreateRectangle.
//
// Revision 1.9  2002/05/02 15:29:40  michael
// completed transition from old MouseCursor to vxMouseCursor
//
// Revision 1.8  2002/05/01 21:38:55  michael
// Adjusted GetDataModality() to GetDataModalityEnum()
//
// Revision 1.7  2002/05/01 21:16:36  jenny
// commented out if (voi->CalculateVOI(volCoord,  and so on since it still uses volumeset
//
// Revision 1.6  2002/05/01 20:24:54  manju
// Modified rectangle annotation creation.
//
// Revision 1.5  2002/05/01 19:44:06  jenny
// getting rid of volumeset
//
// Revision 1.4  2002/05/01 00:01:56  michael
// Adjusted the Modify calls to use the Modified on the object instead of the one in the environment
//
// Revision 1.3  2002/04/29 19:39:52  manju
// Added rectangle annotation in mouse move event.
//
// Revision 1.2  2002/04/29 18:34:39  manju
// Added rectangle annotation. Changed names of some methods for ellipse
// annotation.
//
// Revision 1.1  2002/04/29 13:20:54  michael
// Renamed the manipulators for handleset
//
// Revision 1.33  2002/04/26 22:27:19  dmitry
// Added SQUARE Mouse Cursor.
//
// Revision 1.32  2002/04/24 17:35:48  jenny
// vxBlockVolume stuff
//
// Revision 1.30  2002/04/23 20:50:46  dmitry
// Cursors implemented.
//
// Revision 1.29  2002/04/23 20:12:23  michael
// replaced Density by Intensity
//
// Revision 1.28  2002/04/23 15:33:47  jenny
// switched to vxBlockVolume<uint2>
//
// Revision 1.27  2002/04/22 20:05:20  manju
// Modified the radius of the point to take into account the volume size, so that
// it does not appear too big on bigger datasets.
//
// Revision 1.26  2002/04/22 19:18:40  manju
// Removed a bug from the volume annotation part in OnButtonDown event.
//
// Revision 1.25  2002/04/22 18:07:45  manju
// Changed parameter of CalculateROIStatsitics to take in reference of the
// density volume.
//
// Revision 1.24  2002/04/20 18:53:33  frank
// Converted #includes into forward declarations.
//
// Revision 1.23  2002/04/18 14:32:33  manju
// Storing the plane orientation as a member in the lasso.
//
// Revision 1.22  2002/04/17 18:17:34  frank
// Cleaned up header & footer.
//
// Revision 1.21  2002/04/12 19:52:46  manju
// Only one Volume annotation is performed on a selected label volume with a
// selected seed point value.
//
// Revision 1.20  2002/04/10 15:04:14  manju
// Removed a bug in creation of arrow in MouseMove event.
// Modified creation of ellipse to take into account ll 3 orientations.
//
// Revision 1.19  2002/04/08 21:21:02  manju
// Modified setting of length of arrow annotation.
//
// Revision 1.18  2002/04/08 13:57:45  manju
// Added key press for toggling of display of histograms fro annotations.
//
// Revision 1.17  2002/04/02 22:18:18  manju
// Modified OnButtonUp event.
//
// Revision 1.16  2002/04/02 15:29:25  manju
// Slight modifications.
//
// Revision 1.15  2002/04/01 20:36:40  manju
// Modified creation of ellipse annotation in the Move event.
//
// Revision 1.14  2002/04/01 19:04:50  manju
// Not using number as label for annotations. Using "default" as of now.
//
// Revision 1.13  2002/04/01 15:17:47  manju
// Firing event Added in ButtonDown instead of Modified.
//
// Revision 1.12  2002/03/29 22:33:34  jaddonisio
// Moved world coordinate function ouside to base class.
//
// Revision 1.11  2002/03/29 16:35:53  manju
// Annotation can be deleted while creation, if the Esc key is pressed.
//
// Revision 1.10  2002/03/28 19:04:16  manju
// No comment.
//
// Revision 1.9  2002/03/28 19:03:31  manju
// Modified creation of ellipse annotation.
//
// Revision 1.8  2002/03/28 16:05:16  manju
// Added undef FILE_REVISION
//
// Revision 1.7  2002/03/28 14:49:58  manju
// Setting the mode to adjust for now, after creation of annotation.
//
// Revision 1.6  2002/03/27 22:26:51  manju
// Slight modifications.
//
// Revision 1.5  2002/03/27 18:31:08  manju
// Using viewportsize instead of size.
//
// Revision 1.4  2002/03/27 18:17:17  manju
// Using world coordiantes of mouse positions now.
//
// Revision 1.3  2002/03/25 16:29:51  manju
// Changed names of annotation modes and changed GetMeasurementMode
// to GetAnnotationMode.
//
// Revision 1.2  2002/03/24 16:36:06  michael
// adjusted to new dsp files with less dependencies and introduced more forward declarations ... to be continued
//
// Revision 1.1  2002/03/21 16:18:53  manju
// Manipulator to create annotations.
//
// $Header: /CVS/cvsRepository/V3D/src/vxViewer/src/vxManipulatorHandlesObjCreate.C,v 1.1.1.1 2004/03/02 03:58:42 geconomos Exp $
// $Id: vxManipulatorHandlesObjCreate.C,v 1.1.1.1 2004/03/02 03:58:42 geconomos Exp $
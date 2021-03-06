#include "mark_tracker.h"
#include "ros/ros.h"

#include <visp/vpImageIo.h>
#include <visp/vpDisplayX.h>
#include <visp/vpMeLine.h>
#include <visp/vpTemplateTracker.h>
#include <visp/vpTemplateTrackerSSDESM.h>
#include <visp/vpTemplateTrackerSSDForwardAdditional.h>
#include <visp/vpTemplateTrackerSSDForwardCompositional.h>
#include <visp/vpTemplateTrackerSSDInverseCompositional.h>
#include <visp/vpTemplateTrackerZNCCForwardAdditional.h>
#include <visp/vpTemplateTrackerZNCCInverseCompositional.h>
#include <visp/vpTemplateTrackerWarpHomography.h>
#include <visp/vpTrackingException.h>

TrackLandingMark::~TrackLandingMark()
{
  if(tracker != NULL)
	delete(tracker);
  if(warp != NULL);
	delete(warp);
  if(display != NULL)
	delete(display);
}

/**
* Use this constructor to set up this class, with default tracker parameters
*/
TrackLandingMark::TrackLandingMark(int x, int y, TrackerType type)
{
  warp = new vpTemplateTrackerWarpHomography();
  switch(type){
	case SSDESM:
	  tracker = new vpTemplateTrackerSSDESM(warp);
	  break;
	case SSDForwardAdditional:
	  tracker = new vpTemplateTrackerSSDForwardAdditional(warp);
	  break;
	case SSDForwardCompositional:
	  tracker = new vpTemplateTrackerSSDForwardCompositional(warp);
	  break;
	case SSDInverseCompositional:
	  tracker = new vpTemplateTrackerSSDInverseCompositional(warp);
	  break;
	case ZNCCForwardAdditional:
	  tracker = new vpTemplateTrackerZNCCForwardAdditional(warp);
	  break;
	case ZNCCInverseCompositional:
	  tracker = new vpTemplateTrackerZNCCInverseCompositional(warp);
	  break;
	default:
	  ROS_ERROR("Invalid trackerType value %d (this should never happen..", (int)type);
	  throw "BAD TRACKERTYPE";
  } 
  
  setSampling(2, 2);
  setLambda(0.001);
  setIterationMax(200);
  setPyramidal(4, 1);
  
  I.init(y, x);
  
  display = new vpDisplayX(I, 0, 0, "Detector and Tracker output");
  
  detectedState = false;
  trackingState = false;
  displayEnabled = false;
  
  trackerData.minX = 0;
  trackerData.minY = 0;
  trackerData.maxX = 0;
  trackerData.maxY = 0;
  trackerData.confidence = 0.0;
}

bool TrackLandingMark::detectAndTrack(const sensor_msgs::Image::ConstPtr& msg)
{
  if(displayEnabled){
	I = visp_bridge::toVispImage(*msg);
	vpDisplay::display(I);
  }
  
  if(!detectedState)
	detectedState = detector.detect(msg);
  
  if(detectedState && !trackingState)
  {
	// initialize the tracker
	landing_mark mark = detector.get_landing_mark();
	
	ROS_INFO("A marker has been detected, (%f, %f, %f, %f)", mark.x, mark.y, mark.width, mark.height );
	vpTemplateTrackerZone tz;
	tz.add(vpTemplateTrackerTriangle(
	  vpImagePoint(mark.y, mark.x), 
	  vpImagePoint(mark.y + mark.height, mark.x),
	  vpImagePoint(mark.y, mark.x + mark.width)
	));
	tz.add(vpTemplateTrackerTriangle(
	  vpImagePoint(mark.y, mark.x + mark.width),
	  vpImagePoint(mark.y + mark.height, mark.x),
	  vpImagePoint(mark.y + mark.height, mark.x + mark.width)
	));
	tracker->initFromZone(I, tz);
	trackingState = true;
  }
  
  if(trackingState)
  {
	try{
	  tracker->track(I);

	  // get tracker information
	  vpColVector p = tracker->getp();
	  vpHomography H = warp->getHomography(p);
	  std::cout << "Homography: \n" << H << std::endl;
	  vpTemplateTrackerZone zone_ref = tracker->getZoneRef();
	  vpTemplateTrackerZone zone_warped;
	  warp->warpZone(zone_ref, p, zone_warped);
	  
	  // Display the information if needed
	  if(displayEnabled){
		tracker->display(I, vpColor::red);
	  }
	  
	  // create a message with tracker data and publish it
	  trackerData.minX = zone_warped.getMinx();
	  trackerData.minY = zone_warped.getMiny();
	  trackerData.maxX = zone_warped.getMaxx();
	  trackerData.maxY = zone_warped.getMaxy();
	  trackerData.confidence = 1.0f;
	}catch(vpTrackingException e)
	{
	  ROS_INFO("An exception occurred.. cancelling tracking.");
	  tracker->resetTracker();
	  trackingState = false;
	  detectedState = false;
	}
  }else{
	trackerData.confidence /= 2.0f; // half the confidence level every step we don't track
  }
  
  if(displayEnabled)
	vpDisplay::flush(I);
}

// get methods
kuri_mbzirc_challenge_1_marker_tracking::TrackerData TrackLandingMark::getTrackerData(){ return trackerData; }

vpTemplateTrackerWarpHomography TrackLandingMark::getHomography(){ return *warp; }

bool TrackLandingMark::markDetected(){ return detectedState; }

bool TrackLandingMark::isTracking(){ return trackingState; }

  
// set methods
void TrackLandingMark::enableTrackerDisplay(bool enabled){ displayEnabled = enabled; }

void TrackLandingMark::setSampling(int i, int j){ tracker->setSampling(i,j); }
void TrackLandingMark::setLambda(double l){ tracker->setLambda(l); }
void TrackLandingMark::setIterationMax(const unsigned int& n){ tracker->setIterationMax(n); }

void TrackLandingMark::setPyramidal(unsigned int nlevels = 2, unsigned int level_to_stop = 1)
{
  tracker->setPyramidal(nlevels, level_to_stop);
}


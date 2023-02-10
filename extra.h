#include "opencv2/tracking.hpp"

//PARAMETRI

//keyArea kvadrat
#define x1 0 //0
#define x2 480 //480
#define y1 10 //30
#define y2 300 //300

//center obmocje za stetje
#define c1 145 //145
#define c2 155 //155

//parametri za detekcijo
#define HOGpyramid 1.5 //1.5
#define windowStride 4 //8

//na koliko framov sprozimo detekcijo
#define frames 15 //5

namespace cv {

class CV_EXPORTS_W MultiTrackerD
{
public:

	/**
	* \brief Constructor.
	* In the case of trackerType is given, it will be set as the default algorithm for all trackers.
	* @param trackerType the name of the tracker algorithm to be used
	*/
	CV_WRAP MultiTrackerD(const String& trackerType = "");

	/**
	* \brief Destructor
	*/
	~MultiTrackerD();

	/**
	* \brief Add a new object to be tracked.
	* The defaultAlgorithm will be used the newly added tracker.
	* @param image input image
	* @param boundingBox a rectangle represents ROI of the tracked object
	*/
	CV_WRAP bool add(const Mat& image, const Rect2d& boundingBox);

	/**
	* \brief Add a new object to be tracked.
	* @param trackerType the name of the tracker algorithm to be used
	* @param image input image
	* @param boundingBox a rectangle represents ROI of the tracked object
	*/
	CV_WRAP bool add(const String& trackerType, const Mat& image, const Rect2d& boundingBox);

	/**
	* \brief Add a set of objects to be tracked.
	* @param trackerType the name of the tracker algorithm to be used
	* @param image input image
	* @param boundingBox list of the tracked objects
	*/
	CV_WRAP bool add(const String& trackerType, const Mat& image, std::vector<Rect2d> boundingBox);

	/**
	* \brief Add a set of objects to be tracked using the defaultAlgorithm tracker.
	* @param image input image
	* @param boundingBox list of the tracked objects
	*/
	CV_WRAP bool add(const Mat& image, std::vector<Rect2d> boundingBox);

	/**
	* \brief Update the current tracking status.
	* The result will be saved in the internal storage.
	* @param image input image
	*/
	std::vector<int>  update(const Mat& image);

	//!<  storage for the tracked objects, each object corresponds to one tracker algorithm.
	std::vector<Rect2d> objects;

	//!< storage for history of movement points
	std::vector<std::vector<Point>> vektorjiP;

	//!< storage for IDs (ID is x + y value of center of object)
	std::vector<int> ID;

	/**
	* \brief Update the current tracking status.
	* @param image input image
	* @param boundingBox the tracking result, represent a list of ROIs of the tracked objects.
	*/
	CV_WRAP std::vector<int>  update(const Mat& image, CV_OUT std::vector<Rect2d> & boundingBox);

	/**
	* \brief Return vector of vectors of points.
	* Returns history of movement points.
	*/
	CV_WRAP std::vector<std::vector<Point>> returnVektorP();

	/**
	* \brief Return vector of IDS.
	*/
	std::vector<int> returnIDs();

protected:
	//!<  storage for the tracker algorithms.
	std::vector< Ptr<Tracker> > trackerList;

	//!<  default algorithm for the tracking method.
	String defaultAlgorithm;
};
}
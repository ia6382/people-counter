#include "extra.h"

namespace cv {

  /*
    Vzeto iz OpenCV 3.1.0 source kode, dodal IDje, nadgradil brisanje in sledenje pozicijam trackanih objektov
  */

  // constructor
  MultiTrackerD::MultiTrackerD(const String& trackerType):defaultAlgorithm(trackerType){};

  // destructor
  MultiTrackerD::~MultiTrackerD(){};

  // add an object to be tracked, defaultAlgorithm is used
  bool MultiTrackerD::add(const Mat& image, const Rect2d& boundingBox){
    if(defaultAlgorithm==""){
      printf("Default algorithm was not defined!\n");
      return false;
    }

    return add(defaultAlgorithm.c_str(), image, boundingBox);
  };

  // add a new tracked object
  bool MultiTrackerD::add( const String& trackerType, const Mat& image, const Rect2d& boundingBox ){ //////////////GLAVNI//////////////////
    if(boundingBox.tl().y > y1 && boundingBox.br().y < y2+30){ //dodaj samo ce je keyArea
      Ptr<Tracker> newTracker = Tracker::create( trackerType );

      trackerList.push_back(newTracker);
      objects.push_back(boundingBox);

      Point center = (boundingBox.br() + boundingBox.tl())*0.5;

      ID.push_back(center.x + center.y);//dovolj unikatno stevilo

      std::vector<Point> tmp;
      tmp.push_back(center);
      vektorjiP.push_back(tmp); 

      return trackerList.back()->init(image, boundingBox);
    }else{
      return true;
    }
  };

  // add a set of objects to be tracked
  bool MultiTrackerD::add(const String& trackerType, const Mat& image, std::vector<Rect2d> boundingBox){
    bool stat=false;

    for(unsigned i =0;i<boundingBox.size();i++){
      stat=add(trackerType,image,boundingBox[i]);
      if(!stat)break;
    }

    return stat;
  };

  // add a set of object to be tracked, defaultAlgorithm is used.
  bool MultiTrackerD::add(const Mat& image, std::vector<Rect2d> boundingBox){
    if(defaultAlgorithm==""){
      printf("Default algorithm was not defined!\n");
      return false;
    }

    return add(defaultAlgorithm.c_str(), image, boundingBox);
  };

  // update position of the tracked objects, the result is stored in internal storage /////////////////////////////poklice se update iz trackinga
  std::vector<int> MultiTrackerD::update( const Mat& image){ 

    std::vector<int> onCenter;
    for(unsigned i=0;i< trackerList.size(); i++){
      trackerList[i]->update(image, objects[i]);

      //poglej ali je tracker izven meja, ce je ga izbrisi
      if(objects[i].tl().y < y1 || objects[i].br().y > y2+30){
        trackerList[i]->clear();
        trackerList.erase(trackerList.begin() + i);
        objects.erase(objects.begin() + i);
        vektorjiP.erase(vektorjiP.begin() + i);
        ID.erase(ID.begin() + i);
      }
      else{
        //add movement point to the list
        Point center = (objects[i].br() + objects[i].tl())*0.5;
        vektorjiP[i].push_back(center);

        //if target is on counting line add to list
        if(center.y > c1 && center.y < c2){
          onCenter.push_back(i);
        }
      }

    }
    return onCenter;
  };

  // update position of the tracked objects, the result is copied to external variable ////////////////////////////ta gre v zgornjega
  std::vector<int> MultiTrackerD::update( const Mat& image, std::vector<Rect2d> & boundingBox ){
    std::vector<int> onCenter = update(image);
    boundingBox=objects;
    return onCenter;
  };

  std::vector<std::vector<Point>> MultiTrackerD::returnVektorP(){
    return vektorjiP;
  }

  std::vector<int> MultiTrackerD::returnIDs(){
    return ID;
  }

} /* namespace cv */
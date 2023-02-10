#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/tracking.hpp>
#include <opencv2/videoio.hpp>
#include <opencv2/core/utility.hpp>
#include "opencv2/objdetect/objdetect.hpp"
#include <iostream>
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <cstring>
#include <ctime>

#include "extra.h"

using namespace cv;
using namespace std;

vector<Rect> detectHuman(Mat img, HOGDescriptor hog){
    vector<Rect> found, found_filtered;

    hog.detectMultiScale(img, found, 0, Size(windowStride,windowStride), Size(8,8), HOGpyramid, 2);
        
    //preglej zadetke ali je vsebovan v katerem drugem, ce ni dodaj v filtered
    size_t i, j;
    for( i = 0; i < found.size(); i++ )
    {
        Rect r = found[i];

        for( j = 0; j < found.size(); j++ )
            if( j != i && (r & found[j]) == r)
                break;
            if( j == found.size() )
                found_filtered.push_back(r);
    }
    // the HOG detector returns slightly larger rectangles than the real objects.
    // so we slightly shrink the rectangles to get a nicer output. deluje pocasneje potem
    /*for( i = 0; i < found_filtered.size(); i++ )
    {
        found_filtered[i].x += cvRound(found_filtered[i].width*0.1);
        found_filtered[i].width = cvRound(found_filtered[i].width*0.8);
        found_filtered[i].y += cvRound(found_filtered[i].height*0.07);
        found_filtered[i].height = cvRound(found_filtered[i].height*0.8);
    }
    */
    return found_filtered;
}

//preveri katerim detektiranim ze sledimo, uporabi Jaccardov indeks
vector<Rect2d> check(vector<Rect2d> detected, vector<Rect2d> tracked){
	Rect2d unija;
	Rect2d presek;
	vector<Rect2d> dodaniDetected;

	double unijaSize;
	double presekSize;
	double jaccard;
	double max;

	double treshold = 0.05;

	if(tracked.size() == 0){
		return detected;
	}

	size_t i, j;
	for( i = 0; i < detected.size(); i++ ){
		max = 0;
		jaccard = 0;
		for( j = 0; j < tracked.size(); j++ ){
			unija = detected[i] | tracked[j];
			presek = detected[i] & tracked[j];
			unijaSize = unija.width * unija.height; //kako dobiti size pravilno
			presekSize = presek.width * presek.height;
			jaccard = presekSize / unijaSize;
			
			if(jaccard > max){
				max = jaccard;
			}
		}
		if(max < treshold){ //ce ni ze trackan ga dodaj, da se bo
			dodaniDetected.push_back(detected[i]);
		}
	}
	return dodaniDetected;

}
 
int main(int argc, char *argv[]){
	int tipka;
  	Mat img;
  	size_t i, j;
  	int stevecLjudi = 10;
  	int stevecDet = 0;

  	printf("*************************");
	//Reading video
  	VideoCapture video(argv[1]);

  	//inicializiraj HOG detektor ljudi
    HOGDescriptor hog; //deskriptor
    hog.setSVMDetector(HOGDescriptor::getDefaultPeopleDetector()); //natreniran model

    //inicializiraj multitracker
    MultiTrackerD trackers("KCF");
    vector<Rect2d> tracked;


  	while (true) {
	    video.read(img);
	    if (img.empty()){
	      break;
	    }
	    double t = (double)getTickCount();

		//vzami del slike
		Rect keyArea = Rect(x1,y1,x2,y2);
		Mat smallImg = Mat(img, keyArea);

		//spremenljivke za detektiranje 
		vector<Rect> detected;
	    vector<Rect2d> DdetectedTmp;
	    vector<Rect2d> dodaniDetected;
	    

		if(stevecDet == frames){ // detektiraj le vsakih 5 framov
		    //detektiraj ljudi
		    printf("Detecting ");
		    detected = detectHuman(smallImg, hog);
			vector<Rect2d> Ddetected(detected.begin(), detected.end()); //rect v rect2d (int v double)
		    DdetectedTmp = Ddetected;

		    stevecDet = 0;
	    }

	    //poglej ali ze sledimo detektiranemu objektu
	    if(detected.size() > 0){
	    	dodaniDetected = check(DdetectedTmp, tracked);
		}

	    //ce ne dodaj objekt/objekte v tracker
	    if(dodaniDetected.size() > 0){
	    	trackers.add(img,dodaniDetected);
	    }

	    //sledi objektom
		vector<int> onCenter = trackers.update(img, tracked);

		//vektorji premikanja
		vector<vector<Point>> zgodovina = trackers.returnVektorP();

		//vektorji IDjev
		vector<int> IDs = trackers.returnIDs();

		//ce je sel kateri cez center poglej smer
		for(i = 0; i < onCenter.size(); i ++){
			int sum = 0;
			int stPreckanj = 0;
			for(j = 0; j < zgodovina[onCenter[i]].size(); j ++){
				int val = zgodovina[onCenter[i]][j].y - c1; //sredinska crta je izhodisce
				if(val > 0 && val < c2-c1){
					stPreckanj ++;
				}
				sum += val;
			}
			//ce je ta objekt ze sel cez linijo
			if(stPreckanj > 1){
				break;
			}
			//ce je sel gor pristej, ce je sel dol odstej
			else if(sum > 0){
				stevecLjudi ++;
			}
			else{
				stevecLjudi --;
			}
		}

	    //narisi kvadrat okoli detektiranih objektov
		for( i = 0; i < detected.size(); i++ ){
            Rect r = detected[i];
            rectangle(img, r.tl(), r.br(), cv::Scalar(0,255,0), 3);
        }

        if(tracked.size() > 0){
	        //narisi kvadrat okoli trackanih objektov
	        for( i = 0; i < tracked.size(); i++ ){	
	            rectangle(img, tracked[i], Scalar( 255, 0, 0 ), 2, 1);
			    putText(img, "ID: "+to_string(IDs[i]), Point(tracked[i].tl().x, tracked[i].tl().y-5), FONT_HERSHEY_PLAIN, 1, Scalar(255, 0, 0), 2, 8, false ); //ID sledenega objekta
	        }
	    }

	    //narisi vektorje premikanje
		for( i = 0; i < zgodovina.size(); i++ ){
			for( j = 0; j < zgodovina[i].size(); j++ ){
				line(img, zgodovina[i][j], zgodovina[i][j], Scalar( 255, 255, 255 ), 2, 8, 0);
			}
		}

	    //narisi crte za stetje
	    line(img, Point(x1, c1), Point(x2, c1), Scalar( 0, 255, 0 ), 1, 8, 0);
	    line(img, Point(x1, c2), Point(x2, c2), Scalar( 0, 255, 0 ), 1, 8, 0);

	    //narisi okvir okoli preiskovanega podrocja
	    rectangle(img, keyArea, Scalar( 0, 255,  0), 2, 1);

	    //napisi stevilo ljudi
	    putText(img, "#"+to_string(stevecLjudi), Point(x1, c1-5), FONT_HERSHEY_DUPLEX, 1, Scalar(0, 255, 0), 1, 8, false );
		
	    imshow("Video", img);

	    //stevec za detekcijo
	    stevecDet ++;

	    t = (double)getTickCount() - t;
	    t = t*1000./cv::getTickFrequency();
    	printf("time = %gms\n", t);

    	//fps najvec 30
    	if(t > 30){
    		tipka = waitKey(1);
    	}
    	else{
    		tipka = waitKey(30);
    	}

		if (tipka == 27){ //ESC
			return 0;
		}
		
	}

  video.release();
  return 0;
}

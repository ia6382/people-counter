# people-counter

A prototype project made for the Computer Vision course at the University of Ljubljana, Faculty of Computer and Information Science in 2017. 
The idea was to develop an application that would be able to diplay the number of people currently in the dinning hall by monitoring the entrance stairway. 

Using OpenCV the app scans the video frames for humans using a HOG (Histogram of Oriented Gradients) descriptor and object detector. We used an included classifier already trained for people detection. It then tracks the detected humans using KCF (Kernelized Correlation Filter) tracker. By calculating the movement vector it is then able to deduce if the person is leaving or entering the dining room and thus count the number of people.

Repository includes the source code and 2 testing videos.

Made with C++ and OpenCV v3.1 in 2017.

//
// Created by lyp on 24.10.20.
//

#ifndef TESTS_DISPLAYVIDEO3D_H
#define TESTS_DISPLAYVIDEO3D_H

#include <vector>
#include "DeckLinkAPI.h"
#include <opencv4/opencv2/opencv.hpp>
class DisplayVideo3D{
public:
//    DisplayVideo3D();
//    ~DisplayVideo3D();

    bool InitDeckLink();
    bool InitVideo();
    void Display();
    void Display_nonsplit();
    void Display_framepointer();
    void FillFrame(cv::Mat frame, char* frame_data);
    void GetFrame(int index);

private:
    IDeckLinkOutput*        myDLOutput_left;
    IDeckLinkOutput*        myDLOutput_right;
    std::vector<IDeckLinkDisplayMode*>	displayModes;
    int deckLinkModeIndex;
    cv::VideoCapture capture;
    double frame_count_total;
};



#endif //TESTS_DISPLAYVIDEO3D_H

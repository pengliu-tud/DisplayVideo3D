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
    void Display();
    void FillFrame(cv::Mat frame, char* frame_data);

private:
    IDeckLinkOutput*        myDLOutput_left;
    IDeckLinkOutput*        myDLOutput_right;
    std::vector<IDeckLinkDisplayMode*>	displayModes;
    int deckLinkModeIndex;
};



#endif //TESTS_DISPLAYVIDEO3D_H

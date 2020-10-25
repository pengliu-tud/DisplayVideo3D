//
// Created by lyp on 24.10.20.
//

#ifndef TESTS_DISPLAYVIDEO3D_H
#define TESTS_DISPLAYVIDEO3D_H

#include <vector>
#include "DeckLinkAPI.h"

class DisplayVideo3D{
public:
//    DisplayVideo3D();
//    ~DisplayVideo3D();

    bool InitDeckLink();
    void Display();

private:
    IDeckLinkOutput*        myDLOutput_left;
    IDeckLinkOutput*        myDlOutput_right;
    std::vector<IDeckLinkDisplayMode*>	displayModes;
    int deckLinkModeIndex;
};



#endif //TESTS_DISPLAYVIDEO3D_H

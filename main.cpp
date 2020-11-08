#include <iostream>
#include "DeckLinkAPI.h"
#include "DisplayVideo3D.h"
#include "DLOutputCallback.h"
int main() {
//    DisplayVideo3D* displayVideo3D = new DisplayVideo3D();
//    displayVideo3D->InitDeckLink();
//    displayVideo3D->InitVideo();
//    displayVideo3D->Display();
//    displayVideo3D->GetFrame(60);

    DLOutputCallback* dlOutput_delegate = new DLOutputCallback();
    dlOutput_delegate->InitDeckLink();
//    dlOutput_delegate->InitVideo();
    return 0;
}

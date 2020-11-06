#include <iostream>
#include "DeckLinkAPI.h"
#include "DisplayVideo3D.h"
int main() {
    DisplayVideo3D* displayVideo3D = new DisplayVideo3D();
//    displayVideo3D->InitDeckLink();
    displayVideo3D->Display();

    return 0;
}

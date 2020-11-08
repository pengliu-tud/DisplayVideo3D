//
// Created by lyp on 25.10.20.
//


#include <unistd.h>
#include <bitset>
#include "DisplayVideo3D.h"

//#include <opencv2/opencv.hpp>

using namespace cv;
using namespace std;

bool DisplayVideo3D::InitDeckLink() {
    IDeckLinkIterator* deckLinkIterator = NULL;
    IDeckLinkDisplayModeIterator* deckLinkDisplayModeIterator = NULL;
    IDeckLink* deckLink = NULL;
    IDeckLinkDisplayMode* deckLinkDisplayMode = NULL;
    BMDDisplayMode selectedDisplayMode = NULL;
    HRESULT	result;
    deckLinkModeIndex = 1;

    deckLinkIterator = CreateDeckLinkIteratorInstance();
    if (deckLinkIterator == NULL){
        printf("DeckLink driver required!");
        return false;
    }

    while (deckLinkIterator->Next(&deckLink) == S_OK){
        if (! myDLOutput_left){
            if (deckLink->QueryInterface(IID_IDeckLinkOutput, (void**)&myDLOutput_left) != S_OK){
                myDLOutput_left = NULL;
            }
        }else{
            if (!myDLOutput_right){
                if (deckLink->QueryInterface(IID_IDeckLinkOutput, (void**)&myDLOutput_right) != S_OK){
                    myDLOutput_right = NULL;
                }
            }
        }
        if (myDLOutput_right && myDLOutput_left){
            break;
        }
    }

    if (!myDLOutput_left && !myDLOutput_right){
        printf("two DeckLink devices required!");
        return false;
    }

    myDLOutput_left->GetDisplayModeIterator(&deckLinkDisplayModeIterator);
    while (deckLinkDisplayModeIterator->Next(&deckLinkDisplayMode) == S_OK)
    {
        displayModes.push_back(deckLinkDisplayMode);
    }

    for (size_t i = 0; i < displayModes.size(); i++)
    {
        char* displayModeName;
        result = displayModes[i]->GetName((const char **)&displayModeName);
        if (result == S_OK)
        {
            BMDTimeValue frameRateDuration;
            BMDTimeValue frameRateScale;

            displayModes[i]->GetFrameRate(&frameRateDuration, &frameRateScale);
            fprintf(stderr,
                    "        %2d:  %-20s \t %li x %li \t %g FPS\n",
                    i,
                    displayModeName,
                    displayModes[i]->GetWidth(),
                    displayModes[i]->GetHeight(),
                    (double)frameRateScale / (double)frameRateDuration
            );
            free(displayModeName);
        }
    }

    selectedDisplayMode = displayModes[deckLinkModeIndex]->GetDisplayMode();

    bool displayModeSupported;
    result = myDLOutput_left->DoesSupportVideoMode(
            bmdVideoConnectionUnspecified,
            selectedDisplayMode,
//            bmdFormat10BitYUV,
            bmdFormat8BitBGRA,
            bmdNoVideoOutputConversion,
            bmdSupportedVideoModeDefault,
            NULL,
            &displayModeSupported);

    if(result != S_OK){
        printf("Could not determine whether video mode is supported");
        return false;
    }
    if (!displayModeSupported)
    {
        fprintf(stderr, "Video mode is not supported\n");
        return false;
    }
    myDLOutput_left->EnableVideoOutput(selectedDisplayMode, bmdVideoOutputFlagDefault);
    myDLOutput_right->EnableVideoOutput(selectedDisplayMode, bmdVideoOutputFlagDefault);
//m_deckLinkOutput->DoesSupportVideoMode(bmdVideoConnectionUnspecified, displayMode, pixelFormat, bmdNoVideoOutputConversion, supportedVideoModeFlags, nullptr, &displayModeSupported)

}

bool DisplayVideo3D::InitVideo() {
    capture.open("video/drop001.avi");
    if(!capture.isOpened())
    {
        printf("can not open ...\n");
        return false;
    }else{
        frame_count_total = capture.get(CV_CAP_PROP_FRAME_COUNT);
        cout<<"total frames:"<<frame_count_total<<endl;
    }
}

void DisplayVideo3D::FillFrame(cv::Mat frame, char* frame_data) {
    unsigned int RGB10bit_1;
    unsigned int RGB10bit_2;
    unsigned int BGRA;
    for (int x=0; x<frame.rows; x++){
        for (int y=0; y<frame.cols; y++){
            unsigned int b = frame.data[frame.channels()*(frame.cols*y + x) + 0];
            unsigned int g = frame.data[frame.channels()*(frame.cols*y + x) + 1];
            unsigned int r = frame.data[frame.channels()*(frame.cols*y + x) + 2];
            cout<<x<<" "<<y<<": B "<<b<<" G "<<g<<" R "<<r<<endl;

            BGRA =  (b & 0x000000FF)+
                    ((g << 8) & 0x0000FF00)+
                    ((r << 16) & 0x00FF0000);

            RGB10bit_1 = ((b & 0x000000FF) << 24) + ((b & 0x00000300) << 8) +
                         ((g & 0x0000003F) << 18) + ((g & 0x000003C0) << 2) +
                         ((r & 0x000003F0) >> 4)  + ((r & 0x0000000F) << 12);

            RGB10bit_2 = ((r & 0x000003FF) << 20) +
                         ((g & 0x000003FF) << 10) +
                         (b & 0x000003FF);
            cout<<"BGRA:"<<BGRA<<" RGB10bit:"<<RGB10bit_1<<" "<<RGB10bit_2<<endl;

            ((unsigned int*)frame_data)[x * y * 4] = BGRA;
        }
    }

    //        int j = 0;
//    for(int i=0; i< frame.rows * frame.cols; i++){
//        int a = ((int*)frame.data)[i];
//
//        std::stringstream sstream;
//        sstream << std::hex << a;
//        std::string result = sstream.str();
//        cout<<result<<endl;
//
//        bitset<32> a_bin(a);
//        cout << a_bin << endl;

//        int a1 = (a & 0x3FF00000) >> 22;
//        int a2 = (a & 0x000FFC00) >> 12;
//        int a3 = (a & 0x000003FF) >> 2;

//        int a1 = (a & 0x3FF00000) >> 22;
//        int a2 = (a & 0x000FFC00) >> 12;
//        int a3 = (a & 0x000003FF) >> 2;
//        cout<<i/frame.cols<<" "<<i%frame.rows<<": B "<<a1<<" G "<<a2<<" R "<<a3<<endl;
//    }
}


void DisplayVideo3D::GetFrame(int index) {
    cout<<"index:"<<index<<endl;

    if(!capture.isOpened())
    {
        printf("video is not opened!\n");
        return;
    }

    cv::Mat frame;
    capture.set(CV_CAP_PROP_POS_FRAMES,index);
    capture.read(frame);
    cv::namedWindow("frame", WINDOW_AUTOSIZE);
    imshow("output", frame);
    cv::waitKey(3000);
}

void DisplayVideo3D::Display() {
//    CvCapture *input_video = cv::cvCreateFileCapture("guitarplaying.avi");
//    std::cout<<cv::getBuildInformation()<<std::endl;
//    cv::VideoCapture capture("drop001.avi");

    cv::Mat frame;
    cv::Mat up;
    cv::Mat down;
    double fps;
    int width;
    int height;

    //void* videoFrame_up_data;
    //void* videoFrame_down_data;

    char *buffer;
    if((buffer = getcwd(NULL, 0)) == NULL)
    {
        perror("getcwd error");
    }
    else
    {
        printf("%s\n", buffer);
        free(buffer);
    }


    //frame= capture.open("video/drop001.avi");
//    frame= capture.open("/home/liupeng/video/Video.avi");
//    cout<<"frame depth:"<<frame.depth()<<"   frame channels:"<<frame.channels()<<endl;
    if(!capture.isOpened())
    {
        printf("video is not opened!\n");
        return;
    }
    width = capture.get(CAP_PROP_FRAME_WIDTH);
    height = capture.get(CAP_PROP_FRAME_HEIGHT);
    cout<<"size:"<<width<<"x"<<height<<endl;
    cout<<CV_8U<<endl;
    fps = capture.get(CAP_PROP_FPS);
    printf("frame rate: %f", fps);
    int frame_num = 0;

    //cv::namedWindow("output", cv::WINDOW_AUTOSIZE);
    //cv::namedWindow("up", cv::WINDOW_AUTOSIZE);
    //cv::namedWindow("down", cv::WINDOW_AUTOSIZE);
    while (capture.read(frame))
    {

//        FillFrame(frame);
        cout<<"frame number:"<<frame_num<<endl;
        //cout<<sizeof(frame.data)<<endl;
        IDeckLinkMutableVideoFrame* videoFrame_up = nullptr;
        IDeckLinkMutableVideoFrame* videoFrame_down = nullptr;
        cv::Mat up_resized, down_resized;
        cv::Mat up_resized_c4, down_resized_c4;

        cout<<frame.rows<<frame.cols<<endl;
        cout<<"frame depth:"<<frame.depth()<<"   frame channels:"<<frame.channels()<<endl;
        //imshow("output", frame);
        up = frame(cv::Rect(0,0, width, height/2));
        cv::resize(up, up_resized, cv::Size(1920, 1080));
        cv::cvtColor(up_resized, up_resized_c4, CV_BGR2BGRA);
        cout<<"frame channels:"<<up_resized_c4.channels()<<endl;
        //imshow("up", up_resized_c4);
        cout<<up_resized.rows<<" "<<up_resized.cols<<endl;
        down = frame(cv::Rect(0,height/2, width, height/2));
        cv::resize(down, down_resized, cv::Size(1920, 1080));
        cv::cvtColor(down_resized, down_resized_c4, CV_BGR2BGRA);
        //imshow("down", down_resized);
//        cout<<frame.depth()<<frame.channels()<<endl;
        HRESULT result, result1;
        result = myDLOutput_left->CreateVideoFrame(width, height, width*4, bmdFormat8BitBGRA, bmdFrameFlagDefault, &videoFrame_up);
        result1 = myDLOutput_right->CreateVideoFrame(width, height, width*4, bmdFormat8BitBGRA, bmdFrameFlagDefault, &videoFrame_down);
        if (result!= S_OK || result1!=S_OK){
             printf("error when creating frame");
             return;
         }else{
            cout<<"create frame successful"<<endl;
            uint32_t* videoFrame_up_data;
            uint32_t* videoFrame_down_data;
            videoFrame_up->GetBytes((void**)&videoFrame_up_data);
            videoFrame_down->GetBytes((void**)&videoFrame_down_data);
            memcpy(videoFrame_up_data, up_resized_c4.data, width * height * 4);
            videoFrame_down->GetBytes((void**)&videoFrame_down_data);
            memcpy(videoFrame_down_data, down_resized_c4.data, width * height * 4);
//             for (int i = 0; i < (width * height/2) / 10; i ++){
//                 *(videoFrame_up_data + i) = 0;
//                 *(videoFrame_down_data + i) = 0;
//             }

             myDLOutput_left->DisplayVideoFrameSync(videoFrame_up);
             myDLOutput_right->DisplayVideoFrameSync(videoFrame_down);
             videoFrame_up = nullptr;
             videoFrame_up_data = nullptr;
         }


        frame.release();
        up.release();
        down.release();
        up_resized.release();
        up_resized_c4.release();
        //up.release();
        //up.release();
        frame_num++;
        //sleep(20/fps);
        cv::waitKey(1000/fps);
    }
//    cout<<frame.depth()<<frame.channels()<<endl;
    capture.release();
    return;
}

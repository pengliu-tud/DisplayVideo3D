//
// Created by lyp on 06.11.20.
//
#include <unistd.h>
#include "DLOutputCallback.h"

using namespace std;
using namespace cv;


//DLOutputCallback::DLOutputCallback(cv::VideoCapture cap, double count) {
//    capture = cap;
//    frame_count_total = count;
//}

//DLOutputCallback::DLOutputCallback(){
////    InitDeckLink();
////    InitVideo();
//}

bool DLOutputCallback::InitDeckLink() {
    IDeckLinkIterator* deckLinkIterator = NULL;
    IDeckLinkDisplayModeIterator* deckLinkDisplayModeIterator = NULL;
    IDeckLink* deckLink = NULL;
    IDeckLinkDisplayMode* deckLinkDisplayMode = NULL;
    BMDDisplayMode selectedDisplayMode = bmdModeHD1080p50;
    HRESULT result;
    deckLinkModeIndex = 7;

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

    //selectedDisplayMode = displayModes[deckLinkModeIndex]->GetDisplayMode();
    myDLOutput_left->GetDisplayMode(selectedDisplayMode, &deckLinkDisplayMode);
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

    deckLinkDisplayMode->GetFrameRate(&frame_duration, &frame_timescale);
    cout<<"selected mode: frame_duration "<<frame_duration<<" frame_timescale"<<frame_timescale<<endl;
    cout<<"set callback"<<endl;
    if(myDLOutput_left->SetScheduledFrameCompletionCallback(this)!=S_OK){
        cout<<"set callback failed"<<endl;
    }
    //myDLOutput_right->SetScheduledFrameCompletionCallback(this);

    myDLOutput_left->EnableVideoOutput(selectedDisplayMode, bmdVideoOutputFlagDefault);
    //myDLOutput_right->EnableVideoOutput(selectedDisplayMode, bmdVideoOutputFlagDefault);
//m_deckLinkOutput->DoesSupportVideoMode(bmdVideoConnectionUnspecified, displayMode, pixelFormat, bmdNoVideoOutputConversion, supportedVideoModeFlags, nullptr, &displayModeSupported)

}


bool DLOutputCallback::InitVideo(){
    capture.open("/home/liupeng/video/Video.avi");
    if(!capture.isOpened())
    {
        printf("can not open ...\n");
        return false;
    }else{
        frame_count_total = capture.get(CV_CAP_PROP_FRAME_COUNT);
        frame_cols = capture.get(CAP_PROP_FRAME_WIDTH);
        frame_rows = capture.get(CAP_PROP_FRAME_HEIGHT);
        cout<<"total frames:"<<frame_count_total<<endl;
        cout<<"frame resolution:"<<frame_cols<<"x"<<frame_rows<<endl;
    }
    frame_counter = 0;

    return true;
}

void DLOutputCallback::StartPlayback() {

    IDeckLinkMutableVideoFrame* firstFrame;
    uint32_t*	firstFrame_data;
    HRESULT result;
    result = myDLOutput_left->CreateVideoFrame(frame_cols, frame_rows, frame_cols*4, bmdFormat8BitBGRA, bmdFrameFlagDefault, &firstFrame);
//        myDLOutput_left->CreateVideoFrame(frame_cols, frame_rows, frame_cols*4, bmdFormat8BitBGRA, bmdFrameFlagDefault, &firstFrame);
    if (result != S_OK){
        cout<<"create frame failed"<<endl;
    }
    firstFrame->GetBytes((void**)&firstFrame_data);
        //memset(firstFrame_data, 255, firstFrame->GetRowBytes() * frame_cols);

    cv::Mat frame;
    capture.set(CV_CAP_PROP_POS_FRAMES, frame_counter);
    
    cout<<frame_counter<<endl;
    if (!capture.read(frame)){
        cout<<"read frame finished"<<endl;
        //return S_OK;
    }
        //namedWindow("output", cv::WINDOW_AUTOSIZE);
        //imshow("output", frame);
        //waitKey(2000);
        
    frame_counter++;
    memcpy(firstFrame_data, frame.data, frame_rows * frame_cols*4);
    myDLOutput_left->ScheduleVideoFrame(firstFrame, 0, frame_duration, frame_timescale);
    cout<<frame_counter<<endl;
    


    myDLOutput_left->StartScheduledPlayback(0, frame_timescale, 1.0);
    //myDLOutput_right->ScheduleVideoFrame(firstFrame, 0, frame_duration, frame_timescale);
    cout<<"Scheduled frame"<<endl;

    //return true;
}

HRESULT DLOutputCallback::ScheduledFrameCompleted(IDeckLinkVideoFrame* completedFrame, BMDOutputFrameCompletionResult result0){
    cout<<"Scheduled frame completed"<<endl;
    if (completedFrame)
    {
        cout<<"index:"<<frame_counter<<endl;

        if(!capture.isOpened())
        {
            printf("video is not opened!\n");
            return S_FALSE;
        }

        cv::Mat frame;
        IDeckLinkMutableVideoFrame* videoFrame_up = nullptr;
        IDeckLinkMutableVideoFrame* videoFrame_down = nullptr;
        cv::Mat up_resized, down_resized;
        cv::Mat up_resized_c4, down_resized_c4;
        HRESULT result, result1;

        capture.set(CV_CAP_PROP_POS_FRAMES, frame_counter);
        if (!capture.read(frame)){
            cout<<"read frame finished"<<endl;
            return S_OK;
        }
        frame_counter++;

//        cv::namedWindow("frame", WINDOW_AUTOSIZE);
//        imshow("output", frame);
//        cv::waitKey(3000);

        cout<<frame.rows<<frame.cols<<endl;
        cout<<"frame depth:"<<frame.depth()<<"   frame channels:"<<frame.channels()<<endl;
        //imshow("output", frame);
        //up =
        cv::resize(frame(cv::Rect(0, 0, frame_cols, frame_rows/2)), up_resized, cv::Size(1920, 1080));
        cv::cvtColor(up_resized, up_resized_c4, CV_BGR2BGRA);
        cout<<"frame channels:"<<up_resized_c4.channels()<<endl;

        cout<<up_resized.rows<<" "<<up_resized.cols<<endl;
//        down = ;
        cv::resize(frame(cv::Rect(0,frame_rows/2, frame_cols, frame_rows/2)), down_resized, cv::Size(1920, 1080));
        cv::cvtColor(down_resized, down_resized_c4, CV_BGR2BGRA);

        result = myDLOutput_left->CreateVideoFrame(frame_cols, frame_cols, frame_cols*4, bmdFormat8BitBGRA, bmdFrameFlagDefault, &videoFrame_up);
        //result1 = myDLOutput_right->CreateVideoFrame(frame_cols, frame_cols, frame_cols*4, bmdFormat8BitBGRA, bmdFrameFlagDefault, &videoFrame_down);

        if (result == true){
        //if (result && result1 == true){
            myDLOutput_left->ScheduleVideoFrame(videoFrame_up, frame_counter * frame_duration, frame_duration, frame_timescale);
        //    myDLOutput_right->ScheduleVideoFrame(videoFrame_down, frame_counter * frame_duration, frame_duration, frame_timescale);
        }

    }
}



HRESULT DLOutputCallback::ScheduledPlaybackHasStopped(void){
    return S_OK;
}
HRESULT DLOutputCallback::QueryInterface (REFIID iid, LPVOID *ppv){
    return S_OK;
}
ULONG DLOutputCallback::AddRef(){
    return ++m_refCount;
}
ULONG DLOutputCallback::Release(){
    ULONG newRefValue = --m_refCount;
    if (newRefValue == 0){
        delete this;
    }
    return newRefValue;
}



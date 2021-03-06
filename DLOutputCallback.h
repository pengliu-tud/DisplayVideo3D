//
// Created by lyp on 06.11.20.
//

#ifndef TESTS_DLOUTPUTCALLBACK_H
#define TESTS_DLOUTPUTCALLBACK_H
#include "DeckLinkAPI.h"
#include <opencv4/opencv2/opencv.hpp>
class DLOutputCallback: IDeckLinkVideoOutputCallback{

public:
//    DLOutputCallback(cv::VideoCapture cap, double count);
//    DLOutputCallback();
    bool InitDeckLink();
    bool InitVideo();
    void StartPlayback();
//    bool ProcessNextFrame();

    HRESULT STDMETHODCALLTYPE ScheduledFrameCompleted(IDeckLinkVideoFrame* completedFrame, BMDOutputFrameCompletionResult) override;
    HRESULT STDMETHODCALLTYPE ScheduledPlaybackHasStopped() override;
    HRESULT STDMETHODCALLTYPE QueryInterface (REFIID iid, LPVOID *ppv);
    ULONG AddRef();
    ULONG Release();

private:
    IDeckLinkOutput*        myDLOutput_left;
    IDeckLinkOutput*        myDLOutput_right;
    std::vector<IDeckLinkDisplayMode*>	displayModes;
    int deckLinkModeIndex;
    BMDTimeValue frame_duration;
    BMDTimeScale frame_timescale;
    cv::VideoCapture capture;
    double frame_count_total;
    double frame_counter;
    double frame_rows;
    double frame_cols;
    ULONG m_refCount;
};


#endif //TESTS_DLOUTPUTCALLBACK_H

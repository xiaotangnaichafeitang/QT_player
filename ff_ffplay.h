#ifndef FF_FFPLAY_H
#define FF_FFPLAY_H
#include <thread>
#include "ffmsg_queue.h"
#include "ff_ffplay_def.h"

class FFPlayer
{
public:
    FFPlayer();
    int ffp_create();
    void ffp_destroy();
    int ffp_prepare_async_l(char *file_name);

    // 播放控制
    int       ffp_start_l();
    int       ffp_stop_l();
    int stream_open( const char *file_name);
    void stream_close();
    // 打开指定stream对应解码器、创建解码线程、以及初始化对应的输出
    int stream_component_open(int stream_index);
    // 关闭指定stream的解码线程，释放解码器资源
    void stream_component_close(int stream_index);

    MessageQueue msg_queue_;
    char *input_filename_;
    int read_thread();

    std::thread *read_thread_;

    // 帧队列
    FrameQueue	pictq;          // 视频Frame队列
    FrameQueue	sampq;          // 采样Frame队列

    // 包队列
    PacketQueue		audioq;                 // 音频packet队列
    PacketQueue videoq;             // 视频队列
    int abort_request = 0;

    AVStream		*audio_st = NULL;              // 音频流
    AVStream		*video_st = NULL;              // 音频流
    int audio_stream = -1;
    int video_stream = -1;

    int eof = 0;
     AVFormatContext *ic = NULL;
};

inline static void ffp_notify_msg1(FFPlayer *ffp, int what) {
    msg_queue_put_simple3(&ffp->msg_queue_, what, 0, 0);
}

inline static void ffp_notify_msg2(FFPlayer *ffp, int what, int arg1) {
    msg_queue_put_simple3(&ffp->msg_queue_, what, arg1, 0);
}

inline static void ffp_notify_msg3(FFPlayer *ffp, int what, int arg1, int arg2) {
    msg_queue_put_simple3(&ffp->msg_queue_, what, arg1, arg2);
}

inline static void ffp_notify_msg4(FFPlayer *ffp, int what, int arg1, int arg2, void *obj, int obj_len) {
    msg_queue_put_simple4(&ffp->msg_queue_, what, arg1, arg2, obj, obj_len);
}

inline static void ffp_remove_msg(FFPlayer *ffp, int what) {
    msg_queue_remove(&ffp->msg_queue_, what);
}



#endif // FF_FFPLAY_H

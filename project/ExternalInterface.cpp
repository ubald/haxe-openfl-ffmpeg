#ifndef IPHONE
#define IMPLEMENT_API
#endif

#ifndef STATIC_LINK
#define IMPLEMENT_API
#endif

#include <iostream>
#include <hx/CFFI.h>

extern "C" {
	#include <libavutil/avutil.h>
	#include <libavformat/avformat.h>
	#include <libswresample/swresample.h>
	#include <libswscale/swscale.h>
}

void out(const AVCodecContext* codecContext, const AVFrame* frame, value callback) {
	int in_nchannels = codecContext->channels;
	int in_nsamples = frame->nb_samples;
	AVSampleFormat in_sample_format = codecContext->sample_fmt;
	int in_channel_layout = codecContext->channel_layout;

	SwrContext *swr = swr_alloc_set_opts(
		NULL,
		AV_CH_LAYOUT_STEREO, AV_SAMPLE_FMT_FLT, 44100,
		codecContext->channel_layout, codecContext->sample_fmt, codecContext->sample_rate,
		0, NULL
	);

	swr_init(swr);
	//int in_size = av_samples_get_buffer_size(NULL, in_nchannels, in_nsamples, in_sample_format, 1);
	int out_size = av_samples_get_buffer_size(NULL, in_nchannels, in_nsamples, AV_SAMPLE_FMT_FLT, 1);
	buffer out_buffer = alloc_buffer_len(out_size);
	uint8_t *out = (uint8_t *)buffer_data(out_buffer);
	swr_convert(swr, &out, in_nsamples, (const uint8_t **)frame->data, in_nsamples);
	swr_free(&swr);
	val_call1(callback, buffer_val(out_buffer));
}

int __ffmpeg_read_file( const char *name, value callback ) {
	// Initialize FFmpeg
    av_register_all();

    AVFrame* frame = av_frame_alloc();
    if (!frame)
    {
        std::cout << "Error allocating the frame" << std::endl;
        return 1;
    }

    // you can change the file name "01 Push Me to the Floor.wav" to whatever the file is you're reading, like "myFile.ogg" or
    // "someFile.webm" and this should still work
    AVFormatContext* formatContext = NULL;
    if (avformat_open_input(&formatContext, name, NULL, NULL) != 0)
    {
        av_free(frame);
        std::cout << "Error opening the file" << std::endl;
        return 1;
    }

    if (avformat_find_stream_info(formatContext, NULL) < 0)
    {
        av_free(frame);
        avformat_close_input(&formatContext);
        std::cout << "Error finding the stream info" << std::endl;
        return 1;
    }

    // Find the audio stream
    AVCodec* cdc = 0;
    int streamIndex = av_find_best_stream(formatContext, AVMEDIA_TYPE_AUDIO, -1, -1, &cdc, 0);
    if (streamIndex < 0)
    {
        av_free(frame);
        avformat_close_input(&formatContext);
        std::cout << "Could not find any audio stream in the file" << std::endl;
        return 1;
    }

    AVStream* audioStream = formatContext->streams[streamIndex];
    AVCodecContext* codecContext = audioStream->codec;
    codecContext->codec = cdc;

    if (avcodec_open2(codecContext, codecContext->codec, NULL) != 0)
    {
        av_free(frame);
        avformat_close_input(&formatContext);
        std::cout << "Couldn't open the context with the decoder" << std::endl;
        return 1;
    }

    std::cout << "This stream has " << codecContext->channels << " channels and a sample rate of " << codecContext->sample_rate << "Hz" << std::endl;
    std::cout << "The data is in the format " << av_get_sample_fmt_name(codecContext->sample_fmt) << std::endl;

    AVPacket readingPacket;
    av_init_packet(&readingPacket);

    // Read the packets in a loop
    while (av_read_frame(formatContext, &readingPacket) == 0) {
        if (readingPacket.stream_index == audioStream->index) {
            AVPacket decodingPacket = readingPacket;

            // Audio packets can have multiple audio frames in a single packet
            while (decodingPacket.size > 0) {
                // Try to decode the packet into a frame
                // Some frames rely on multiple packets, so we have to make sure the frame is finished before
                // we can use it
                int gotFrame = 0;
                int result = avcodec_decode_audio4(codecContext, frame, &gotFrame, &decodingPacket);

                if (result >= 0 && gotFrame) {
                    decodingPacket.size -= result;
                    decodingPacket.data += result;
					out(codecContext, frame, callback);
                } else {
                    decodingPacket.size = 0;
                    decodingPacket.data = 0;
                }
            }
        }

        // You *must* call av_free_packet() after each call to av_read_frame() or else you'll leak memory
        av_free_packet(&readingPacket);
    }

    // Some codecs will cause frames to be buffered up in the decoding process. If the CODEC_CAP_DELAY flag
    // is set, there can be buffered up frames that need to be flushed, so we'll do that
    if (codecContext->codec->capabilities & CODEC_CAP_DELAY) {
        av_init_packet(&readingPacket);
        // Decode all the remaining frames in the buffer, until the end is reached
        int gotFrame = 0;
        while (avcodec_decode_audio4(codecContext, frame, &gotFrame, &readingPacket) >= 0 && gotFrame) {
            out(codecContext, frame, callback);
        }
    }

    // Clean up!
    av_free(frame);
    avcodec_close(codecContext);
    avformat_close_input(&formatContext);

    return 0;
}

value hx_ffmpeg_read_file( value file_name, value callback ) {
	__ffmpeg_read_file(val_get_string(file_name), callback);
	return alloc_null();
}
DEFINE_PRIM(hx_ffmpeg_read_file,2)
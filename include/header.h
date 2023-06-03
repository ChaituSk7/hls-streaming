#ifndef HEADER_H
#define HEADER_H

#include <gst/gst.h>

typedef struct _HlsStreamData {
    GstElement *pipeline;
    GstElement *video_source;
    GstElement *video_convert;
    GstElement *clock_overlay;
    GstElement *video_scale;
    GstElement *caps_filter;
    GstElement *video_encoder;
    GstElement *video_mux;
    GstElement *hls_sink;
    GMainLoop *loop;
} HlsStreamData;

void hls_pipeline ();

#endif
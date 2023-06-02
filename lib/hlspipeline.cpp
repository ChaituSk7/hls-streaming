#include "header.h"

void hls_pipeline () {

    HlsStreamData hlsData;
    memset(&hlsData, 0, sizeof(hlsData));
    /* Initialize gstreamer library */
    gst_init(NULL, NULL);

}
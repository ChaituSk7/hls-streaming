#include "header.h"

static void 
handle_messages(GstBus *bus, GstMessage *msg, HlsStreamData *data) {
  switch (GST_MESSAGE_TYPE(msg))
  {
  case GST_MESSAGE_ERROR: {
      GError *error = NULL;
      gchar *debug = NULL;
      gst_message_parse_error (msg, &error, &debug);
      g_print ("\n Error received from %s : %s", GST_OBJECT_NAME (msg->src),
          error->message);
      g_print ("\n Debug Info : %s\n", (debug) ? debug : "None");
      g_error_free (error);
      g_free (debug);
      gst_element_set_state (data->pipeline, GST_STATE_NULL);
      g_main_loop_quit (data->loop);
  }
    break;
  default:
    break;
  }
}
void
hls_pipeline ()
{
  GstBus *bus;
  HlsStreamData hlsData;
  GstCaps *video_scale_caps = NULL;
  GstCaps *enc_caps = NULL;

  /* Initialize the structe members to 0 */
  memset (&hlsData, 0, sizeof (hlsData));

  hlsData.loop = g_main_loop_new (NULL, FALSE);

  /* Initialize gstreamer library */
  gst_init (NULL, NULL);

  hlsData.pipeline = gst_pipeline_new ("HLS-streaming");
  hlsData.video_source = gst_element_factory_make ("v4l2src", NULL);
  hlsData.video_convert = gst_element_factory_make ("videoconvert", NULL);
  hlsData.clock_overlay = gst_element_factory_make ("clockoverlay", NULL);
  hlsData.video_scale = gst_element_factory_make ("videoscale", NULL);
  hlsData.caps_filter = gst_element_factory_make ("capsfilter", NULL);
  hlsData.video_encoder = gst_element_factory_make ("x264enc", NULL);
  hlsData.video_mux = gst_element_factory_make ("mpegtsmux", NULL);
  hlsData.hls_sink = gst_element_factory_make ("hlssink", NULL);

  if (!hlsData.pipeline || !hlsData.video_source || !hlsData.video_convert
      || !hlsData.clock_overlay || !hlsData.video_scale || !hlsData.caps_filter
      || !hlsData.video_encoder || !hlsData.video_mux || !hlsData.hls_sink) {
    g_printerr ("Elements could not created\n");
    exit (EXIT_FAILURE);
  }

  gst_bin_add_many (GST_BIN (hlsData.pipeline), hlsData.video_source,
      hlsData.video_convert, hlsData.clock_overlay, hlsData.video_scale,
      hlsData.caps_filter, hlsData.video_encoder, hlsData.video_mux,
      hlsData.hls_sink, NULL);

  g_object_set (G_OBJECT (hlsData.video_source), "device", "/dev/video0", NULL);
  g_object_set (G_OBJECT (hlsData.video_encoder), "bitrate", 256, NULL);
  video_scale_caps = gst_caps_new_simple ("video/x-raw",
      "width", G_TYPE_INT, 640, "height", G_TYPE_INT, 360, NULL);
  g_object_set (G_OBJECT (hlsData.caps_filter), "caps", video_scale_caps, NULL);
  g_object_set (G_OBJECT (hlsData.hls_sink), "playlist-root",
      "http://10.1.137.49:7890", "location", "segment_%05d.ts",
      "target-duration", 5, "max-files", 5, NULL);

  if (!gst_element_link_many (hlsData.video_source, hlsData.video_convert,
          hlsData.clock_overlay, hlsData.video_scale, hlsData.caps_filter,
          hlsData.video_encoder, hlsData.video_mux, hlsData.hls_sink, NULL)) {
    g_printerr ("Failed to link elements\n");
    exit (EXIT_FAILURE);
  }

  GstStateChangeReturn ret =
      gst_element_set_state (hlsData.pipeline, GST_STATE_PLAYING);

  if (ret == GST_STATE_CHANGE_FAILURE) {
    g_printerr ("Failed to set the pipeline to playing state\n");
    gst_object_unref (hlsData.pipeline);
    exit (EXIT_FAILURE);
  }

  bus = gst_element_get_bus(hlsData.pipeline);
  gst_bus_add_signal_watch(bus);

  g_signal_connect(bus, "message", G_CALLBACK(handle_messages), &hlsData);

  g_main_loop_run (hlsData.loop);

  gst_element_set_state (hlsData.pipeline, GST_STATE_NULL);
  gst_object_unref (hlsData.pipeline);
  g_main_loop_quit (hlsData.loop);
  g_main_loop_unref(hlsData.loop);
  return;
}

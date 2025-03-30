# Video Streamer Simple APP

### Flow
```bash 
    python_app ---(SRT using Gstreamer)---> MediaMTX ---(webrtc)--->browser
```

## Installation and usage
1. Install Gstreamer complete [GStreamer](https://gstreamer.freedesktop.org/download)
2. Clone the repo
    ```bash
    $ git clone https://github.com/MadRajib/video_streaming.git
    ```
4. Set up MediaMTX server

   ```bash
   $ cd /tmp/
   $ wget https://github.com/bluenviron/mediamtx/releases/download/v1.11.3/mediamtx_v1.11.3_linux_amd64.tar.gz
   $ tar -xzf mediamtx_v1.11.3_linux_amd64.tar.gz
   $ sudo mv mediamtx /usr/local/bin/
   $ mv ./mediamtx.yml /usr/local/etc/
   ```
   Check if its running or not:
   ```bash
    remote@streamer:~$ mediamtx 
    2025/03/30 05:32:43 INF MediaMTX v1.11.3
    2025/03/30 05:32:43 INF configuration loaded from /usr/local/etc/mediamtx.yml
    2025/03/30 05:32:43 INF [RTSP] listener opened on :8554 (TCP), :8000 (UDP/RTP), :8001 (UDP/RTCP)
    2025/03/30 05:32:43 INF [RTMP] listener opened on :1935
    2025/03/30 05:32:43 INF [HLS] listener opened on :8888
    2025/03/30 05:32:43 INF [WebRTC] listener opened on :8889 (HTTP), :8189 (ICE/UDP)
    2025/03/30 05:32:43 INF [SRT] listener opened on :8890 (UDP)
   ```
6. Run the streamer app, add mediamtx server ip,port and required streamid

    For linux:

    ```bash
    export SRT_SERVER_IP=192.168.1.97
    export SRT_PORT=8890
    export SRT_STREAM_ID=mystream
    export VIDEO_SRC_NAME="HP Wide Vision HD Camera"
    
    python .\streamer\stremer.py
    ```

    For windows:

    ```bash
    $env:SRT_SERVER_IP="192.168.1.97"
    $env:SRT_PORT="8890"
    $env:SRT_STREAM_ID="mystream"
    $env:VIDEO_SRC_NAME="HP Wide Vision HD Camera"
    
    python .\streamer\stremer.py
    ```
    Without env variables:

    ```cmd
    video_streaming> python .\streamer\stremer.py
    Enter SRT Server IP: 192.168.1.97
    Enter SRT Port: 8889
    Enter Stream ID: mystream

    (gst-launch-1.0:18684): GStreamer-WARNING **: 11:04:28.897: External plugin loader failed. This most likely means that the plugin loader helper binary was not found or could not be run. You might need to set the GST_PLUGIN_SCANNER environment variable if your setup is unusual. This should normally not be required though.
    
    (gst-launch-1.0:18684): GStreamer-WARNING **: 11:04:28.914: Failed to load plugin 'C:\Program Files\gstreamer\1.0\msvc_x86_64\lib\gstreamer-1.0\gstpython.dll': The specified module could not be found.
    This usually means Windows was unable to find a DLL dependency of the plugin. Please check that PATH is correct.
    You can run 'dumpbin -dependents' (provided by the Visual Studio developer prompt) to list the DLL deps of any DLL.
    There are also some third-party GUIs to list and debug DLL dependencies recursively.
    Use Windows high-resolution clock, precision: 1 ms
    Setting pipeline to PAUSED ...
    Pipeline is live and does not need PREROLL ...
    Pipeline is PREROLLED ...
    Setting pipeline to PLAYING ...
    New clock: GstSystemClock
    Redistribute latency...
    Redistribute latency...
    WARNING: from element /GstPipeline:pipeline0/GstSRTSink:srtsink0: Pipeline construction is invalid, please add queues.
    Additional debug info:
    ../libs/gst/base/gstbasesink.c(1257): gst_base_sink_query_latency (): /GstPipeline:pipeline0/GstSRTSink:srtsink0:
    Not enough buffering available for  the processing deadline of 0:00:00.020000000, add enough queues to buffer  0:00:00.018908400 additional data. Shortening processing latency to 0:00:00.001091600.
    WARNING: from element /GstPipeline:pipeline0/GstSRTSink:srtsink0: Pipeline construction is invalid, please add queues.
    Additional debug info:
    ../libs/gst/base/gstbasesink.c(1257): gst_base_sink_query_latency (): /GstPipeline:pipeline0/GstSRTSink:srtsink0:
    Not enough buffering available for  the processing deadline of 0:00:00.020000000, add enough queues to buffer  0:00:00.018908400 additional data. Shortening processing latency to 0:00:00.001091600.
    WARNING: from element /GstPipeline:pipeline0/GstSRTSink:srtsink0: Connection does not exist. Trying to reconnect
    Additional debug info:
    ../ext/srt/gstsrtobject.c(1979): gst_srt_object_write_one (): /GstPipeline:pipeline0/GstSRTSink:srtsink0
    WARNING: from element /GstPipeline:pipeline0/GstSRTSink:srtsink0: Error on SRT socket: Connection timeout (16). Trying to reconnect
    Additional debug info:
    ../ext/srt/gstsrtobject.c(1979): gst_srt_object_write_one (): /GstPipeline:pipeline0/GstSRTSink:srtsink0
    0:00:00.0 / 99:99:99.
   ```
8. Open viewer web page

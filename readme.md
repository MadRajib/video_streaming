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
    # For SRT
    export STREAM_PROTOCOL="srt"
    export STREAM_PORT="8890"

    # For RTMP
    export STREAM_PROTOCOL="rtmp"
    export STREAM_PORT="1935"

    # Common variables
    export STREAM_SERVER_IP="192.168.1.97"
    export STREAM_ID="mystream"
    export STREAM_SRC_NAME="HP Wide Vision HD Camera"
    
    python .\streamer\stremer.py
    ```

    For windows:

    ```bash
    # for srt
    $env:STREAM_PROTOCOL="srt"
    $env:STREAM_PORT="8890"

    # for rmtp
    $env:STREAM_PROTOCOL="rmtp"
    $env:STREAM_PORT="1935"

    $env:STREAM_SERVER_IP="192.168.1.97"
    $env:STREAM_ID="mystream"
    $env:STREAM_SRC_NAME="HP Wide Vision HD Camera"
    
    python .\streamer\stremer.py
    ```
    Without env variables:

    1. SRT
    ```cmd
    video_streaming> python .\streamer\stremer.py --protocol srt --server_ip="172.23.242.30" --port="8890" --stream_id="mystream" --src_name="Integrated Camera"
   ```
    2. RMTP
    ```cmd
    python .\streamer\stremer.py --protocol rtmp --server_ip="172.23.242.30" --port="1935" --stream_id="mystream" --src_name="Integrated Camera"
    ```
    Note: By default __*rmtp*__ protocol is set and default streamkey is __*live*__

8. Open viewer web page

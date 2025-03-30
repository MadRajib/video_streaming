import os

# Get values from environment variables or ask the user
server_ip = os.getenv("SRT_SERVER_IP") or input("Enter SRT Server IP: ").strip()
port = os.getenv("SRT_PORT") or input("Enter SRT Port: ").strip()
stream_id = os.getenv("SRT_STREAM_ID") or input("Enter Stream ID: ").strip()
src_name = os.getenv("VIDEO_SRC_NAME") or input("Enter Video src name: ").strip()

# Construct the SRT URL
SRT_URL = f"srt://{server_ip}:{port}?streamid=publish:{stream_id}&pkt_size=1316"

# GStreamer pipeline for Windows
gst_pipeline = f'mfvideosrc device-name="{src_name}" ! videoconvert ! x264enc tune=zerolatency bitrate=2000 speed-preset=superfast ! mpegtsmux ! srtsink uri={SRT_URL}'

# Run GStreamer pipeline
os.system(f'gst-launch-1.0 {gst_pipeline}')
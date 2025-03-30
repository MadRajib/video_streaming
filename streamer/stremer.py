import os

# Get user input for SRT details
server_ip = input("Enter SRT Server IP: ").strip()
port = input("Enter SRT Port: ").strip()
stream_id = input("Enter Stream ID: ").strip()

# Construct the SRT URL
SRT_URL = f"srt://{server_ip}:{port}?streamid=publish:{stream_id}&pkt_size=1316"

# GStreamer pipeline for Windows
gst_pipeline = f'mfvideosrc device-name="HP Wide Vision HD Camera" ! videoconvert ! x264enc tune=zerolatency bitrate=2000 speed-preset=superfast ! mpegtsmux ! srtsink uri={SRT_URL}'

# Run GStreamer pipeline
os.system(f'gst-launch-1.0 {gst_pipeline}')
import os
import argparse

# Load environment variables (if set)
PROTOCOL = os.getenv("STREAM_PROTOCOL", "rtmp")  # Default to RTMP
SERVER_IP = os.getenv("STREAM_SERVER_IP", None)
PORT = os.getenv("STREAM_PORT", None)
STREAM_ID = os.getenv("STREAM_ID", "live")
SRC_NAME = os.getenv("STREAM_SRC_NAME", "Integrated Camera")

# Parse command-line arguments
parser = argparse.ArgumentParser(description="Stream webcam video using GStreamer via SRT or RTMP.")
parser.add_argument("--protocol", choices=["srt", "rtmp"], default=PROTOCOL, help="Choose 'srt' or 'rtmp'.")
parser.add_argument("--server_ip", type=str, default=SERVER_IP, required=SERVER_IP is None, help="Server IP for streaming.")
parser.add_argument("--port", type=int, default=int(PORT) if PORT else None, required=PORT is None, help="Port number.")
parser.add_argument("--stream_id", type=str, default=STREAM_ID, help="Stream ID (SRT) or Stream Key (RTMP).")
parser.add_argument("--src_name", type=str, default=SRC_NAME, help="Webcam source name.")
args = parser.parse_args()

# Construct streaming URL based on protocol
if args.protocol == "srt":
    stream_url = f"srt://{args.server_ip}:{args.port}?streamid=publish:{args.stream_id}&pkt_size=1316"
    gst_pipeline = (
        f'mfvideosrc device-name="{args.src_name}" ! videoconvert ! '
        f'x264enc tune=zerolatency bitrate=2000 speed-preset=superfast ! mpegtsmux ! '
        f'srtsink uri={stream_url}?latency=50 '
    )
elif args.protocol == "rtmp":
    stream_url = f"rtmp://{args.server_ip}:{args.port}/{args.stream_id}"
    gst_pipeline = (
        f'mfvideosrc device-name="{args.src_name}" ! videoconvert ! '
        f'x264enc tune=zerolatency bitrate=2000 speed-preset=superfast ! flvmux ! '
        f'rtmpsink location={stream_url}'
    )
else:
    print("Unsupported protocol. Please choose 'srt' or 'rtmp'.")
    exit(1)

# Run GStreamer pipeline
print(f"Starting {args.protocol.upper()} stream to {stream_url}...")
os.system(f'gst-launch-1.0 {gst_pipeline}')
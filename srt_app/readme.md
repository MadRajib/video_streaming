# SRT server
A imlentation of srt server using [libsrt](https://github.com/Haivision/srt)

# To Build
```bash
$ make
```

# run
```
$ build/server
```

# Streaming client
```bash
$ python .\streamer\stremer.py --server_ip="192.168.1.97" --port=8890 --src_name="HP Wide Vision HD Camera" --protocol="srt"
```

# view stream

Run vlc media player and set "srt://<server_ip>:8891" 
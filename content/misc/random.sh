#!/bin/bash
#based off of https://github.com/keroserene/rickrollrc
trap '' 2
trap '' HUP
trap '' SIGSTP
rick='https://keroserene.net/lol'
audio_gsm="$rick/roll.gsm"
audio_raw="$rick/roll.s16"
video="$rick/astley80.full.bz2"
has() { hash $1 2>/dev/null; }
obtainium() {
if has curl; then curl -s $1
elif has wget; then wget -q -O - $1
else echo "Cannot has internets. :(" && exit
fi
}
#echo -en "\x1b[?25l \x1b[2J \x1b[H"
cat>/tmp/play.py<<EOF
import sys
import time
fps = 25; time_per_frame = 1.0 / fps
buf = ''; frame = 0; next_frame = 0
begin = time.time()
try:
  for i, line in enumerate(sys.stdin):
    if i % 32 == 0:
      frame += 1
      sys.stdout.write(buf); buf = ''
      elapsed = time.time() - begin
      repose = (frame * time_per_frame) - elapsed
      if repose > 0.0:
        time.sleep(repose)
      next_frame = elapsed / time_per_frame
    if frame >= next_frame:
      buf += line
except KeyboardInterrupt:
  pass
EOF
obtainium $video | bunzip2 -q 2> /dev/null | python /tmp/play.py &
while true
do if has afplay; then
[ -f /tmp/roll.s16 ] || obtainium $audio_raw >/tmp/roll.s16
afplay /tmp/roll.s16
elif has aplay; then
obtainium $audio_raw | aplay -Dplug:default -q -f S16_LE -r 8000
elif has play; then
obtainium $audio_gsm >/tmp/roll.gsm.wav
play -q /tmp/roll.gsm.wav
fi
done

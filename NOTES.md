## Squeezebox documentation

http://lithium.pecorous.co.uk:9000/html/docs/help.html

## Getting current playlist information

Querying for information on the current playlist can be done a number of ways.

Using the `playlist album <index> ?` command will return the current album name. Also fro the following:

```
<playerid> playlist genre <index> ? 
<playerid> playlist artist <index> ? 
<playerid> playlist album <index> ? 
<playerid> playlist title <index> ? 
<playerid> playlist path <index> ? 
<playerid> playlist remote <index> ? 
<playerid> playlist duration <index> ?
```

Alternatively to also get the track_id the `status` command can be used, e.g.

```
00:04:20:17:81:d5 status 0 10 tags:
00%3A04%3A20%3A17%3A81%3Ad5 status 0 10 tags%3A player_name%3ALiving%20room player_connected%3A1 player_ip%3A192.168.2.193%3A37075 power%3A1 signalstrength%3A92 mode%3Astop time%3A0 rate%3A1 duration%3A252.866 can_seek%3A1 mixer%20volume%3A0 playlist%20repeat%3A0 playlist%20shuffle%3A0 playlist%20mode%3Aoff seq_no%3A0 playlist_cur_index%3A1 playlist_timestamp%3A1493934066.80794 playlist_tracks%3A2 digital_volume_control%3A1 playlist%20index%3A0 id%3A9165 title%3AIntro playlist%20index%3A1 id%3A9166 title%3AApocalypse%20Please
```

The track_id is a smaller piece of information to store on the esp8266.

## Adding items to the playlist

```
<player id> playlist loadalbum <genre> <artist> <album>
<player id> playlistcontrol cmd:load track_id:<comma separated list>
```

## ESP8266 filesystem

https://github.com/esp8266/Arduino/blob/master/doc/filesystem.md
http://docs.platformio.org/en/latest/platforms/espressif8266.html#uploading-files-to-file-system-spiffs

## ESP8266 TCP Client

http://www.arduino.org/learning/reference/wificlient

# wowpatcher-5.4.8
The patcher applies the following modifications to `Wow-64.exe` (verified against clean 5.4.8.18414 client):
- NOP signature verification call.
- Remove patch number limit.
- Unlock FrameXML & Blizzard interface addons.
- Enable test realm connection mode.
- Enable CDN downloads to occur.

## CDN Setup (for streaming / minimal client)
1. Get a **full clean 5.4.8.18414 client**.
2. Copy files from the game `Data/` folder into the template's `Data/` folder: `contrib/cdn_setup/www/wow-pod-retail/enUS/18414.direct/`.
3. Copy or move the www folder into your webserver to serve the files.
4. Put the patched `Wow-64.exe` in `contrib/cdn_setup/client`
5. Open & modify: `contrib/cdn_setup/client/WoW.mfil` `http://YOUR-IP-OR-DOMAIN-HERE/wow-pod-retail/enUS/18414.direct/` & point `contrib/cdn_setup/client/WTF/Config.wtf` `SET realmlist "127.0.0.1"` to your actual realmlist.
6. Launch `Wow-64.exe` and it will fetch the files from your cdn server and allow you to login to your game while streaming.

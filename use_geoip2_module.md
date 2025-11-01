## 1. Install packages

`sudo apt update`

`sudo apt install -y libnginx-mod-http-geoip2 mmdb-bin curl`

## 2. download and setup files

`sudo mkdir -p /usr/share/GeoIP && cd /usr/share/GeoIP`

`sudo curl -L -o dbip-country-lite-$(date +%Y-%m).mmdb.gz https://download.db-ip.com/free/dbip-country-lite-$(date +%Y-%m).mmdb.gz`
  
`sudo curl -L -o dbip-city-lite-$(date +%Y-%m).mmdb.gz https://download.db-ip.com/free/dbip-city-lite-$(date +%Y-%m).mmdb.gz`

`sudo gunzip -f dbip-country-lite-*.mmdb.gz`

`sudo gunzip -f dbip-city-lite-*.mmdb.gz`

`sudo ln -sf dbip-country-lite-$(date +%Y-%m).mmdb dbip-country-lite.mmdb`

`sudo ln -sf dbip-city-lite-$(date +%Y-%m).mmdb    dbip-city-lite.mmdb`

- ensure files exist

`ls -lh /usr/share/GeoIP/*.mmdb`



## 3. config Nginx
`sudo vim /etc/nginx/modules-enabled/50-mod-http-geoip2.conf`
```nginx
load_module modules/ngx_http_geoip2_module.so;
```

`sudo vim /etc/nginx/conf.d/geoip2.conf`
```nginx
geoip2 /usr/share/GeoIP/dbip-country-lite.mmdb {
    auto_reload 5m;
    $geoip2_country_code default=ZZ source=$remote_addr country iso_code;
    $geoip2_country_name country names en;
}

geoip2 /usr/share/GeoIP/dbip-city-lite.mmdb {
    auto_reload 5m;
    $geoip2_city_name   default=-  source=$remote_addr city names en;
    $geoip2_latitude    location latitude;
    $geoip2_longitude   location longitude;
}

log_format geo '$remote_addr - $remote_user [$time_local] '
               '"$request" $status $body_bytes_sent '
               '"$http_referer" "$http_user_agent" '
               '$geoip2_country_code "$geoip2_country_name" "$geoip2_city_name" '
               '$geoip2_latitude $geoip2_longitude';

access_log /var/log/nginx/access_geo.log geo;
```


## 4. reload Nginx
`sudo nginx -t`

`sudo systemctl reload nginx`

## 5. Test it :)
`tail -f /var/log/nginx/access_geo.log`

## 6. final step:
`sudo vim /usr/local/bin/update-dbip.sh`
```bash
#!/usr/bin/env bash
set -euo pipefail
cd /usr/share/GeoIP
for kind in country city; do
  curl -fsSL -O "https://download.db-ip.com/free/dbip-${kind}-lite-$(date +%Y-%m).mmdb.gz"
  gunzip -f "dbip-${kind}-lite-$(date +%Y-%m).mmdb.gz"
  ln -sf "dbip-${kind}-lite-$(date +%Y-%m).mmdb" "dbip-${kind}-lite.mmdb"
done
nginx -t && systemctl reload nginx
```

`vim /etc/cron.d/update-dbip`
```bash
0 6 2 * * root /usr/local/bin/update-dbip.sh
```

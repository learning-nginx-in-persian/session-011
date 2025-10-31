## 1. Install requirements
`sudo apt install -y nginx build-essential libpcre2-dev zlib1g-dev libssl-dev curl`

## 2. configure ngx_tiny_module
`sudo mkdir -p /usr/local/src/ngx-tiny-echo`

`sudo mkdir -p /usr/local/src/nginx-1.24.0`

`vim /usr/local/src/ngx-tiny-echo/ngx_http_tiny_echo_module.c`

https://github.com/learning-nginx-in-persian/session-011/blob/main/ngx-tiny-echo/ngx_http_tiny_echo_module.c

`vim /usr/local/src/ngx-tiny-echo/config`

https://github.com/learning-nginx-in-persian/session-011/blob/main/ngx-tiny-echo/config

`cd /usr/local/src`

find best version based on `nginx -v`:

`sudo curl -LO http://nginx.org/download/nginx-1.24.0.tar.gz`

`sudo tar xf nginx-1.24.0.tar.gz -C /usr/local/src`

`cd /usr/local/src/nginx-1.24.0`

`sudo ./configure --with-compat --add-dynamic-module=/usr/local/src/ngx-tiny-echo`

`sudo make modules`

- ensure file exists

`ls -l objs/*.so`

`sudo mkdir -p /usr/lib/nginx/modules`

`sudo cp objs/ngx_http_tiny_echo_module.so /usr/lib/nginx/modules/`

## use it
`vim /etc/nginx/modules-available/50-ngx_tiny_echo.conf`
```nginx
load_module /usr/lib/nginx/modules/ngx_http_tiny_echo_module.so;
```

> [!Note]
> its based on Debian/Ubuntu based Nginx:
> 
> `sudo ln -sf /etc/nginx/modules-available/50-ngx_tiny_echo.conf /etc/nginx/modules-enabled/50-ngx_tiny_echo.conf`

`vim /etc/nginx/sites-available/default`
```nginx
server {
    listen 8080 default_server;
    listen [::]:8080 default_server;

    # ...

    location /echo {
        tiny_echo;
    }
}
```

## 4. test it

`sudo nginx -t`

`sudo systemctl reload nginx`

`curl -i http://127.0.0.1:8080/echo`

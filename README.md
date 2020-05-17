## Introduce
This repo will show you an example of how to build an HTTPS server with self signed certificate using openssl. But it is just a homework actually.

## Run
* set the server certificate and key path in `initial.h`
* set the domain name mapping to localhost in `ect/host`
* set the CA certificate on browser
* **CA certificate's CN field need to be same as domain name**
```
terminal:
    make
    ./server
browser:
    <domain name you set>:8787
```

## Expected results
![](https://i.imgur.com/4k3ef5d.png)

## How to use and what can do
* click the link on the webpage can download it

## more example
* This repo is the extension of below two repos
* [simple http server](https://github.com/DannyLeee/HTTPD-from-scratch)
* [simple TLS server/client](https://github.com/DannyLeee/simple-TLS-server-client)
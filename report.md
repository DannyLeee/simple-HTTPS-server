<p style="text-align:right;">
姓名:李韋宗<br>
學號:B10615024<br>
日期:2020/5/22<br>
</p>

<h1 style="text-align:center;"> Netweork Security - HW3

# Part 1 - HTTPS implement

## 建置環境
* WSL(Ubuntu 16.04 LST)
* VS code
* Firefox
* Chrome

## 使⽤說明
* [README](https://github.com/DannyLeee/simple-HTTPS-server/blob/master/README.md)
* step 0:
    * 在系統目錄 `/etc/hosts` 中設定 domain name 對應到本機
        * **domain name 需與 CA 的 CN 欄位相同**
    * 在瀏覽器中設定 CA 路徑
    * 先在 `initial.h` 中設定 server 的憑證及公鑰路徑
* step 1: 在目錄中執行 `make`
* step 2: 在目錄中執行 `./server`
* step 3: 在瀏覽器中進入 <設定的 domain name>:8787，即可進入首頁
* 使用時 terminal 將輸出每筆 request
    
    ---

## 程式碼設計架構與說明
### initial.h 與 initial.c
* 跟作業2的差別，只有刪掉一些沒用到的 code 所以不多加解釋

### server.c
* `create_socket()` 與作業2相同，不重複解釋
* 初始化並建立 socket 與 ssl_CTX 後，進入處理連線的無窮迴圈
* `accept` 建立連線
<br>
<br>
<br>
<br>

```cpp
while(1) 
{
    struct sockaddr_in addr;
    uint len = sizeof(addr);
    SSL *ssl;
    char *reply;
    char receive[1024];
    int count;
    FILE *fp;

    int client = accept(sock, (struct sockaddr*)&addr, &len);
    if (client < 0)
    {
        perror("Unable to accept");
        exit(EXIT_FAILURE);
    }
```
* 連線建立後 `fork`
* 交給子程序處理 ssl 封裝與連線
* 連線不成功的除錯寫在 `acc <= 0` 裡，篇幅關係就拿掉了
```cpp
    if((cpid = fork()) < 0) {
        perror("fork");
        exit(EXIT_FAILURE);
    }
    
    /*child process*/
    if (cpid == 0)
    {
        ssl = SSL_new(ctx);
        SSL_set_fd(ssl, client);    // 配對 SSL 跟新的連線 fd

        // SSL_accept() 處理 TSL handshake
        int acc = SSL_accept(ssl);
        if (acc <= 0)
        {
            printf("acc err: %d\n", acc);
            int err_SSL_get_error = SSL_get_error(ssl, acc);                    
            /* 略 */
        }
```
<br>
<br>
<br>
<br>

* ssl handshake 成功後，分析 http request 做出相應 response
* 用如同作業2，用 `popen` 列出能下載的檔案列表
    * 同時做成連結
```cpp
        else
        {
            printf("get connect!!\n");

            count = SSL_read(ssl, receive, sizeof(receive));
            receive[count] = 0;
            printf("Received from client:\n");
            printf("%s\n\n", receive);

            if (strncmp(receive, "GET / ", 6) == 0 ||
            strncmp(receive, "GET /favicon.ico", 16) == 0)
                {
                    SSL_write(ssl, webPageResponse, strlen(webPageResponse));
                    char * temp = "<!DOCTYPE html>\r\n"
                    "<html><head><title>webServer</title></head>\r\n"
                    "<body><center><h3>Welcome to the 8787 server</h3><br>\r\n";
                    SSL_write(ssl, temp, strlen(temp));
                    if ((fp = popen("ls -p | grep -v / | cat", "r")) == NULL)
                    {
                        perror("open failed!");
                        return -1;
                    }
                    char buf[256];
                    while (fgets(buf, 255, fp) != NULL)
                    {
                        SSL_write(ssl, "<a href='./", strlen("<a href='./"));
                        SSL_write(ssl, buf, strlen(buf));
                        SSL_write(ssl, "'>", strlen("'>"));
                        SSL_write(ssl, buf, strlen(buf));
                        SSL_write(ssl, "</a><br>", strlen("</a><br>"));
                    }
                    if (pclose(fp) == -1)
                    {
                        perror("close failed!");
                        return -2;
                    }
                    SSL_write(ssl, webPageBaseTail, strlen(webPageBaseTail));
                }
```
<br>
<br>
<br>
<br>
<br>
<br>

* 若為其他 request 表點擊首頁的連結或由url輸入
* 開啟相應的檔案，若開啟失敗送出**404**
* 成功則透過 SSL_write 輸出至網頁
    * Firefox 會自動跳出下載
```cpp
                else
                { 
                    char *file_name = strtok(receive, " ");
                    file_name = strtok(NULL, " ");
                    strcpy(file_name, file_name + 1);
                    if ((fp = fopen(file_name, "rb")) == NULL)
                    {
                        SSL_write(ssl, notFound404, strlen(notFound404));
                        SSL_write(ssl, webPageBaseTail, strlen(webPageBaseTail));
                        perror("File opening failed");
                    }
                    else
                    {
                        fseek(fp, 0, SEEK_END);
                        int file_size = ftell(fp);
                        fseek(fp, 0, SEEK_SET);
                        unsigned char *c = malloc(file_size * sizeof(char));
                        fread(c, file_size, 1, fp);

                        SSL_write(ssl, fileResponse, strlen(fileResponse));
                        // write whole file to client
                        SSL_write(ssl, c, file_size);
                        printf("File copy complete\n");
                        fclose(fp);
                        free(c);
                    }
                }
        }
        SSL_shutdown(ssl);
        SSL_free(ssl);
    }
    close(client);
}
```
* 迴圈結束後(程式結束)，關閉 socket 及記憶體處理

    ---

## 成果截圖
* Forefox
    * 首頁
    ![](https://i.imgur.com/Y2gDw3R.png)
    * 點擊連結後
    ![](https://i.imgur.com/qT5V6kI.png)
    * 自動彈出視窗所下載的檔案
    ![](https://i.imgur.com/KUrjDr4.png)
* Chrome
    * 首頁
    ![](https://i.imgur.com/avMlcJr.png)
    * 點擊連結後
    ![](https://i.imgur.com/1aL5NCJ.png)
    * 右鍵儲存檔案所下載的檔案
    ![](https://i.imgur.com/NKXHLVC.png)

    ---

## 困難與⼼得
&emsp;&emsp;整個架構跟作業2類似，解析 request 則與作業1類似，困難的地方反而是設定瀏覽器的部分，因為不知道 CA 的 CN 要跟 domain name 相同，花了不少時間在嘗試解讀從 SSL 錯誤訊息撈到的東西，但都無功而返，所幸忽略錯誤後，奇蹟般地可以繼續連線並顯示，就把安全性驗證放一邊，先做出功能，後來得知 CN 與 domain name 要用相同後，卻只有在 firefox 才可以使用， chrome 還是不行。在 chrome 安全性尚未搞定前，可以正常顯示檔案內容在網頁內，但右鍵的下載檔案也會顯示網路錯誤，同時 server 也會出現 SSL_ERROR_SSL 的錯誤。<br>
&emsp;&emsp;經過找查及同學解釋後發現，chrome 不只要上述的相同，在 chrome 58 後 必須要在生成 csr 與簽屬 crt 時加入 `subjectAltName` 同時定址到相同的 domain name，才可以讓 chrome 認證為安全連線。<br>
&emsp;&emsp;另外有實作作業2被忽略的 fork，意外的發現作業2因為沒 fork 讓子程序處理 ssl accpet，若同時有2個以上的 client 連線，會導致 client 本身 crash，而經過測試這次的作業因為要用瀏覽器模擬作業2的情況較困難，所以就算不 fork 也可以繼續順利的運行。

## 致謝與參考資料
> 生成自簽憑證
> > https://blog.cssuen.tw/create-a-self-signed-certificate-using-openssl-240c7b0579d3

> CA 憑證主體要求
> > https://dotblogs.com.tw/yc421206/2019/05/24/mmc_request_web_certificate_from_ca_server_can_trusted_by_chrome_browser  Chrome 信任憑證的條件

> ssl config 設定
> > https://blog.miniasp.com/post/2019/02/25/Creating-Self-signed-Certificate-using-OpenSSL<br>
> > csr 用 `-config ssl.conf`；crt 用 `-extfile host.ext` 增加 subjectAltName

> 書銘解釋需要 subjectAltName
> > https://github.com/LJP-TW/EZhttpd/blob/master/keys/server.ext<br>
> > https://github.com/LJP-TW/EZhttpd/blob/master/keys/Makefile

---

# Part 2 - A Study of Man-in-the-Middle Attack Based on SSL Certificate Interaction

## Abstract and Introduction
* 分析 SSL 實作的 handshake 流程
* 提出了改進互動式 SSL 憑證
    * 以防止中間人攻擊
    * 提高訊息溝通的品質

## Digest Algorithm
* hash 可以縮短訊息
* 加入機密訊息後可以確保完整性
* 但具有單一私鑰的 hash 卻不能當 MAC，除非具有良好的性能
* 若 MAC 表示為 $MAC_k(m) = \delta$ ，k 表雙方共享的金鑰，m 為訊息， $\delta$ 為認證標記
* 攻擊者發動攻擊時，得到 $(m_1, \delta_1), (m_2, \delta_2) ... (m_q, \delta_q)$
    * 若他得到 $m$ 不存在於 $m_1 ... m_q$ 且同時得到正確的認證標記 $\delta = MAC_k(m)$，則他成功攻擊
    * 攻擊成功的機率為破解 MAC 的機率
* digest algorithim 主要用於計算數位簽章與 MAC

## Principle of SSL Protocal
* 用加密的技術確保資料不會被攔截或竊聽
* 主要提供3種服務
    * 認證使用者與伺服器，確保資料送到對的地方
    * 加密資料以防傳輸時被竊聽
    * 確保資料完整性

### SSL Protocal Architecture
* 位於網路層與應用層之間
* 用 TCP 提供可靠的 end-to-end 安全
* 在進行應用程序通訊之前先諮詢私鑰通訊和服務器身份驗證
* 然後對應用層的資料加密
* 實際上 SSL 由數個協定組成
    * SSL handshake protocol, SSL change password protocol, SSL protocol, SSL alarm protocol and SSL record protocol.
![](https://i.imgur.com/x9e7lr8.png)
* 上層接收應用層的資料，再用可靠的傳輸層向下傳遞資料
* SSL record protocal 是最下層，負責封裝和傳輸上層的資料
    * 上層數句被切成214 bytes 或更小塊
    * 壓縮每一塊並生成 MAC 附加在後
    * 用對稱金鑰加密後由 TCP 傳輸

### SSL Protocol Workflow
![](https://i.imgur.com/qLwihs1.png)

### The Basic Features of SSL Protocal
* handshake 後，用加密方式交換對稱金鑰 (DES)
* 可以透過公鑰 (RSA) 驗證身分
* 封包包含完整性檢驗碼 (SHA)

## SSL Ceritficate Interactive
* 數位憑證包含憑證、簽章演算法及簽章結果
* 憑證包含誰持有此憑證的公鑰、第3方 CA 簽章演算法及 CA 簽章
* 格式遵照  ITU-T x.509

### Man-in-the-Middle Attack
* 攻擊者再中間假扮通訊對方
* 並竊聽或修改通訊雙方的內容

<span id="t"></span>

### Forge SSL Certificate

* 竄改伺服器憑證
* 假設中間人產生了自己的公鑰、私鑰及自簽憑證
* A 要與 B 連線 M 為中間人
    * M 跟 B 用正常 SSL handshake 取得B的憑證資訊
    * 接著在自己偽造的憑證中，在某個 domain (通常是 OU) 加入偽造資訊 (B 的相同 domain 資料但前面加空格)
        * 用戶端的 program window 無法區分
    * 最後 M 在用自己的私要簽章
    * 這樣 M 就有一個偽造 B 的自簽憑證了
    * 對於 SSL 用戶端程式，由於自簽憑證找不到 CA 所以交給使用者決定是繼續連線
    * 對使用者而言，分不出有無空格，通常會接受
    * 最後 M 向 A 發出 server hello 及偽造憑證就可以讓 A 以為在跟 B 溝通

### Improved Certificate Alternate Method
* 通訊雙方共享密鑰
* 伺服器端用 MAC 及密鑰生成 certification mark 一併傳送給使用者
* 客戶端用相同算法確認資料完整性 (不同則斷開連線)
* 攻擊者透過各種方法生成合法 certification mark 稱 hypocrisy

    ---

## 此篇中間⼈攻擊的⽅法
如同 [Forge SSL Certificate](#t)，中間人先跟伺服器正常連線取得憑證後，透過仿製伺服器憑證主體的各個屬性，偽造伺服器憑證，再用自己的私鑰加密，雖然客戶端的應用程式會察覺異狀，但繼續連線決定權還是在是用者，使用者通常無法分別真偽，將會決定繼續連線，只要中間人持續做與伺服器相同的行為，則使用者更難察覺，攻擊成功。

## 對此做法的看法
基本上就是個釣魚的手法，類似釣魚網站把 domain name 用 0 取代 O，因為憑證主體幾乎一樣，就算客戶端應用程式(瀏覽器)提醒，使用者也不一定察覺，尤其是一些盜版軟體或色情網站，通常使用者會繼續連線，那麼攻擊就會成功。老實說，這樣的攻擊聽起來超弱的，如果遇到資安觀念較好的使用者根本無效，或是應用程式強迫不允續繼續連線，那攻擊就失敗了，不過資安攻擊就是針對最弱的一環攻擊，使用者的戒心與粗心就是這個攻擊的目標吧。


<script type="text/javascript" src="http://cdn.mathjax.org/mathjax/latest/MathJax.js?config=TeX-AMS-MML_HTMLorMML"></script>
<script type="text/x-mathjax-config">
    MathJax.Hub.Config({ tex2jax: {inlineMath: [['$', '$']]}, messageStyle: "none" });
</script>
<p style="text-align:right;">
姓名:李韋宗<br>
學號:B10615024<br>
日期:2020/5/22<br>
</p>

<h1 style="text-align:center;"> A Study of Man-in-the-Middle Attack Based on SSL Certificate Interaction

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
* 伺服器端用 MAC 及密鑰生成 certification mark 一並傳送給使用者
* 客戶端用相同算法確認資料完整性 (不同則斷開連線)
* 攻擊者透過各種方法生成合法 certification mark 稱 hypocrisy


---

<script type="text/javascript" src="http://cdn.mathjax.org/mathjax/latest/MathJax.js?config=TeX-AMS-MML_HTMLorMML"></script>
<script type="text/x-mathjax-config">
    MathJax.Hub.Config({ tex2jax: {inlineMath: [['$', '$']]}, messageStyle: "none" });
</script>
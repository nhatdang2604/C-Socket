# C-Socket
## 1.)	Một số thông tin chung
-	Thư viện sử dụng: Window Socket 2 của Microsoft
-	Giao thức sử dụng: HTTP
-	Format khi tải tài nguyên: Content-length
-	Format khi download: Chunked transfer encoding

## 2.)	Ý tưởng
-	Về phần socket base method, ta sẽ tuân theo mô hình server-client TCP
  + Cách setup các socket base method trên internet đều có hướng dẫn rất cụ thể, tạm thời chúng ta sẽ không quan tâm đến nó
-	Câu hỏi trọng tâm đặt ra lúc này là, nếu theo mô hình trong sơ đồ, process request của chúng ta sẽ được xử lí như thế nào?
  + Một số điều chú ý ta có thể rút ra khi đọc đề tài ở đây là:
	  *Giao thức sử dụng: HTTP
	  *Method sử dụng: chỉ có GET và POST
	  *Khi load các file xxx.html, ta sẽ sử dụng format Content-Length
	  *Khi download từ /download/, ta sẽ sử dụng format Chunked Transfer Encoding
    
## 3.)	Cách test
-	B1: Mở chương trình Socket.exe
-	B2: Bật browser lên (chương trình này test thành công trên Chrome)
-	B3: Tiến hành test các chức năng

### a.)	Test chức năng truy cập vào index.html:
-	Nhập vào URL: localhost:8080/ hoặc localhost:8080/index.html
-	Ta sẽ được truy cập vào index.html

### b.)	Test chức năng thông báo error 404 khi đăng nhập sai:
-	 Ta nhập thử account không chính xác, ví dụ:
+ Username: hello
+ Password: 123
-	Ta sẽ thấy xuất hiện thông báo error404

### c.)	Test chức năng redirect vào info.html khi đăng nhập đúng:
-	Ta back lại: localhost:8080/index.html
-	Lần này, ta nhập mật khẩu đúng
+ Username: admin
+ Password: admin
-	Ta sẽ được truy cập vào info.html

### d.)	Test chức năng download (Ví dụ cụ thể: ta sẽ download video.mp4)
-	Ở info.html, ta click vào đường dẫn: Click to open the download folder
-	Ta sẽ được truy cập vào files.html
-	Click vào icon phía trên video.mp4
-	Ta sẽ được tiến hành download
-	Vị trí file download thành công là vị trí đã được đặt sẵn trên browser

### 4.) Demo
- Link youtube: 
https://www.youtube.com/watch?v=pfsmjyOAGPI&feature=youtu.be

# HTML_ls의 결과를 다른 장치의 웹 브라우저에서 확인할 수 있도록 지원하는 서버 프로그램 작성
---

## Steps
1. (web_server) Socket 생성
   - 각자 할당 받은 포트 번호 사용
2. (사용자) 웹 브라우저로 사용자가 접속
   - ex) http://223.194.46.163:port_number
3. (web_server)웹 브라우저가 보낸 HTTP request message를 처리
4. (web_server)HTML_ls의 결과를 HTTP response message에 담아 웹 브라우저에 전송
5. (사용자)웹 브라우저에서 실행 결과를 확인



## Code Requirements
1. -al 옵션 및 -l 옵션을 적용한 결과 출력
2. ./web_server의 실행위치를 root path(default directory)로 하고, root path인 경우 -l 옵션 적용
3. root path 밑의 하위 디렉토리인 경우 -al 옵션 적용
4. 존재하지 않는 디렉토리의 URL 입력 시 404 ERROR
5. 포트번호는 40000으로 고정 필


## 결과 화면
<img width="341" alt="image" src="https://github.com/user-attachments/assets/1bf26ad9-03e0-4159-b44a-c481b802afcf" />

./web_server가 실행되는 root path 에서는 -l 옵션의 결과가 출력된다. file name 클릭 시 해당하는 위치로 이동한다.


<img width="396" alt="image" src="https://github.com/user-attachments/assets/0f8f2edd-8693-4e67-9164-ca425676f2e3" />

하위 디렉토리 폴더로 이동하면 -al 옵션의 결과를 출력하고 상단에는 System Programming Http가 출력된다.


<img width="707" alt="image" src="https://github.com/user-attachments/assets/1f681056-b4ca-4e2d-9e86-5d20e44bfbdb" />
실행파일을 클릭하면 download가 정상적으로 되며, open하면 알맞은 창이 나온다.


<img width="393" alt="image" src="https://github.com/user-attachments/assets/d5380327-1c18-45a5-bb16-d193b75b36d8" />
text 파일 및 소스코드를 클릭하면 파일의 내용을 display한다.


<img width="586" alt="image" src="https://github.com/user-attachments/assets/d5a9a826-549d-4399-8775-e2058f264e64" />
존재하지 않는 디렉토리의 url을 입력하면 not found 창이 뜬다.



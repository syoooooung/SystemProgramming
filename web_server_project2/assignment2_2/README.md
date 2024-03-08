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

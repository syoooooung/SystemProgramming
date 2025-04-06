# 자식 프로세스에 연결된 client의 접속 기록 출력 프로그램 작성
---

## Requirements
- Connection history 출력 (assignment2_3의 연장)
    1. 10초마다 자식 프로세스에 연결된 client의 접속 기록(connection history)를 출력해야 함
    2. 각 자식 프로세스 마다 최신 history 기록을 저장 및 출력 (자식 프로레스 당 최대 10개)
    3. 즉, 5개의 자식 프로세스의 전체 history 개수는 최대 50개
    4. History 번호("No.")는 자식 프로세스 마다 개별적으로 유지
    5. History 제목은 부모 프로세스가 추렭, 하위에 있는 내용은 자식 프로세스가 출력
    6. 즉 "..Connection history...", "No.", "IP" 등의 제목 부분은 부모 프로세스에서 출력
    7. 부모 프로세스가 history 제목을 출력한 뒤, SIGUSR1 signal을 자식 프로세스에게 전달하여 history 내용 출력을 지시
    8. 본 과제에서는 history 내용 출력 관련한 동기화 문제를 고려하지 않음


- 터미널에 간단한 로그 기록을 출력
    1. "...Server is started."
    2. "...Server is terminated"


<br><br>

## 결과 화면
![image](https://github.com/user-attachments/assets/2110963c-9921-451f-9a8b-1bf25e3cb8bb)

child 프로세스 생성 시에 fork 되었다는 출력문을 내보내면 client 접속이 되면 new client 화 함께 연결된 시간과 클라이언트의 정보를 출력한다. 클라이언트 종료시에도 출력한다.


![image](https://github.com/user-attachments/assets/120b3d4f-2db8-4c2b-9189-b0df99ba739b)

10초마다 자식 프로세스에 연결됐었던 클라이언트의 접속 기록을 출력한다. history 번호는 프로세스 마다 개별적으로 유지하며, 프로세스 별로 묶어서 출력하게끔 해주었다. 그리고 SIGINT signal을 발생시키면 자식 프로세스가 종료된다는 메시지와 서버가 종료된다는 메시지를 출력한다.


![image](https://github.com/user-attachments/assets/f3a776af-2202-4f51-8faa-ec55e8ffab21)

서버가 종료되고나서 좀비 프로세스가 없는 것을 확인할 수 있다.

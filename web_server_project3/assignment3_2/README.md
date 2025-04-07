# Idle processes의 수를 공유 메모리로 관리하는 프로그램 작성
---

## Requirements
- httpd.conf에 따라 프로세스, history 개수 등을 조절
- 10초 마다, 공유메모리에 저장된 history 기록을 최신 순서대로 출력
    1. 자식 프로세스는 client의 접속이 들어올 때 마다, History정보를 공유메모리에 저장
    2. 공유메모리에 저장된 history의 수가 MaxHistory수와 같으면 가장 오래된 history를 제거 후 저장
- Process에서 공유메모리에 접근할 때 thread를 생성한 뒤 이를 활용
    1. fork를 사용하여 (StartProcess)개의 자식 프로세스 생성 (Assignment 3-1의 방식 유지)
    2. Shared memory key value는 40000으로 고정
    3. 공유메모리 동기화 문제를 pthread_mutex_lock, pthread_mutex_unlock로 해결
 <img width="205" alt="image" src="https://github.com/user-attachments/assets/a300b67f-23f0-4048-9330-e616e24c6510" />

  
- Idle process 관리
    1. Idle process: client와 연결이 되지 않은 자식 프로세스
    2. Idle process 수를 공유메모리로 관리
    3. Idle process의 수가 변경되는 경우 부모 프로세스에서 출력
    4. Idle process의 수가 (MinIdleNum)개 미만이 되거나 (MaxIdleNum)개 초과가 되면 process를 생성 또는 종료하여 5개를 유지
    5. Client와 연결된 자식 프로세스의 수와 idle process의 수의 합이 ‘Maxchilds’를 넘지않
도록 조절
- 프로그램 종료 전 shared memory 제거 확인(SIGINT signal에서도 처리)
- Client가 연결을 종료하면 자식 프로세스에서 종료 전 5초 sleep (by sleep() function)

<br><br>

## 결과 화면

![image](https://github.com/user-attachments/assets/5a6bc886-5969-472f-a1c0-b40e10b00006)


start process 수 만큼 프로세스를 생성하며 시작된다. 새로운 client 와 연결을 하면 idle process의 수가 줄어드는 것을 출력한다. MinidleNum 보다 idle process 의 수가 작아질 경우, start process의 수에 맞도록 fork 해주어 idle process 수를 맞춰준다. 10초에 한 번씩 connection history를 출력해주며, No는 누적 클라이언트 수와 상관없이 1부터 시작하고 순서는 최신 연결된 클라이언트 부터이다. 클라이언트와 disconnect가 되면 idle count 수는 늘어나고 이를 출력한다.


![image](https://github.com/user-attachments/assets/cf322a84-7d3e-4224-ae9a-9b06912c89e2)


maxidlenum 보다 idle process count 가 커질 경우엔 idle process를 종료하여 start process 수에 맞춰준다. ctrl+c 를 누르면 모든 프로세스를 종료 시킨다.



## Code Flow Chart

-main
![image](https://github.com/user-attachments/assets/da6a8565-eab0-4409-afbb-d7859d817936)

main 함수에선 먼저 httpd.conf 에서 정보들을 읽어오고 shmget으로 공유 메모리를 생성해준다. 필요한 변수 및 구조체를 초기화 한 후 소켓을 생성한다. 소켓 옵션을 설정하고 서버 주소를 성정 후 소켓에 주소 할당한 다음 클라이언트 연결 요청을 대기한다. 그 후에 자식 프로세스 start process 만큼을 만들어 주며, 모든 자식 프로세스가 종료될 때까지 기다렸다가 모든 자식 프로세스가 종료되면 공유 메모리 제거 후 자신도 종료된다는 출력문과 함께 종료된다.


-ls sort

![image](https://github.com/user-attachments/assets/ecfe764b-c4b1-469b-af11-f61982fc1bf2)

디렉토리 안의 요소들을 정렬해주는 함수이다. 루트 path일 땐 히든파일이 있다면 .을 제외한 문자열을 비교하여 정렬해주고, f_info를 호출해준다.


-f_info

![image](https://github.com/user-attachments/assets/6a22db7c-3ace-4417-9fda-4c45dc0752ee)

디렉토리인지, 링크 파일인지 other 인지 구분해 놓은 후 . , .. , 루트 path, other 에 따라 하이퍼링크를 다르게 설정해준다. 파일 유형, 허가권, 링크 수 등 각 요소를 출력해준다.


-sig_history

![image](https://github.com/user-attachments/assets/b33a1cb1-7916-4066-8cab-6254dca9e87a)

부모 프로세스 전용 sighandler 함수이다. signal로 SIGINT 가 들어온다면 자식 프로세스들에게 SIGTERM을 전달해주고, SIGCHLD가 온다면 waitpid를 해준다. 그리고 SIGARLM 이 들어온다면 커넥션 히드토리 제목을 출력하고 자식프로세스에게 SIGUSR1 을 보내준다. SIGUSR1 이 들어오면 idle 수를 조정해줘야한다. 현재 idle 수가 최소idle 수보다 크다면 fork 를 해주고 최대 idle 수 보다 현재 idle 수가 크다면 자식 프로세스를 종료시켜주어 idle 수를 줄여준다.


-child_main

![image](https://github.com/user-attachments/assets/36098562-f295-4e45-b528-9a919e7ae712)

SIGINT 가 발생하면 무시해준다. SIGUSR1, SIGTERM 이 발생하면 child_histroy 함수를 호출한다. 클라이언트의 요청을 받아드리고 요청된 HTTP 메시에서 URL과 ip를 추출한다. url 가 존재하는 지, 디렉토리인지, 파일인지 state를 0,1,2로 나눈다. 현재 연결을 요청한 클라이언트의 ip를 기준으로 허용한 ip를 가진 사용자가 서버에 접속을 요청했는지 판단하고 accessible.usr에 해당하는 ip가 없다면 오류 창을 출력해주는 함수를 호출한다. ip가 허용된 것이라면 new client 로 ip와 port 정보를 출력해준다. 그리고 idle 프로세스 수를 1 줄여주며 저장할 공유메모리의 client list index를 가져온다. 존재하지 않는 경우엔 404 에러 응답을 출력하며 디렉토리인 경우 목록 출력을 위한 경로 생성 후 ls sort 함수를 호출한다. 파일 인 경우 파일 타입에 따라 헤더를 write해준다. 클라이언트와 연결이 해제될 때는 disconnect client 를 출력해주고 idle 프로세스 수를 늘려준다.


-child_history

![image](https://github.com/user-attachments/assets/a9c68444-2936-4bed-9c36-e04eea1cea83)

자식프로세스에서 signal 을 선언할 때 두번째 인자로 넣어주는 함수이다. SIGTERM 이 들어오면 종료한다는 문장출력과 idle process 수룰 줄여주며 종료를 하고 SIGUSR1 이 들어오면 연결됐던 클라이언트들의 정보를 buf에 담았다가 한번에 출력해준다.




## 고찰
스레드에 대한 이해를 도울 수 있는 과제였다. idle process 수가 변경이 될 때마다 그 수를 출력해야 했는데, 부모 프로세스에게 signal을 보내주어 idle process 수를 출력하도록 하는 SIGUSR을 따로 만들어주었다. 하지만 이렇게 하면 처음에 start process 수만큼 fork를 할 때라던가, ctrl+c를 발생시켜 모두 종료를 시킬 때 많은 작업이 한번에 일어나 출력이 꼬이는 일이 발생했다. 그래서 fork 혹은 terminate 됐다는 출력 후에 signal 을보내는 대신 부모 프로세스해서 fork 를 해주거나 자식 프로세스를 종료 시킬 때 같이 idle 프로세스 수를 출력해주도록 수정했다. 또한 fork 를 해주고 자식 프로세스들의 pid 를 저장하는 pids 라는 전역 변수를 선언해주었는데, 처음 시작할 때에 모두 0으로 초기화 해줬음에도 쓰레기값이 저장되는 일이 발생했다. 그래서 fork를 해줄 때마다 자신의 위치 뒷부분을 한 번 더 0으로 초기화 해주는 작업을 추가해줬다











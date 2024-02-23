# 파일 이름만 출력하는 simple ls 구현
---

### Code Requiredments
1. 하나의 디렉토리의 안의 파일 이름들을 출력
2. 파일 경로를 인자로 주지 않을 경우 default로 current directory 결과 출력
3. 하든 파일(starting with . )은 출력하지 않음
4. 파일 이름은 정렬되어 출력
  * 알파벳 순으로 정렬 (aaa, Abc, ada)
  * 정렬 함수 라이브러리 사용 금지
5. 아래 설명의 "예외 처리"를 필히 반영

### 예외 처리
1. 디렉토리가 아닌 파일을 입력하는 경우
2. 존재하지 않는 디렉토리를 입력하는 경우
3. 두 개 이상의 파일 경로를 입력하는 경우

  
### 결과 화면
<img width="533" alt="image" src="https://github.com/syoooooung/SystemProgramming/assets/110389611/fc01cb6b-72d1-4100-a95c-f5267032c1b2">

파일 경로를 인자로 주지 않을 경우 현재 디렉토리의 결과를 출력하며,
파일 이름이 정렬된 순서로 나오는 것을 확인할 수 있다.


<img width="544" alt="image" src="https://github.com/syoooooung/SystemProgramming/assets/110389611/2095c1f3-f07f-4fda-98e5-6f8b6644a193">

디렉토리가 아닌 파일을 입력하거나, 존재하지 않는 디렉토리를 입력하는 경우에는
no such directory라는 오류문을 출력하며,
두 개 이상의 파일 경로를 입력하는 경우에는 하나의 경로만 가능하다는 출력문을 확인할 수 있다.


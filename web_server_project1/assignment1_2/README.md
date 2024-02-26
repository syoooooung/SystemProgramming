# Advanced ls 구현
---

Assignement1_1에서 -a, -l, -la 옵션을 추가 구현한다.

## Requirements
1. Assignment 1-1 과 동일한 조건으로 정렬되어야 함.
   단, 숨김 파일은 처음 dot을 제외한 문자열이 동일한 조건으로 정렬되어 있어야 함.
2. 옵션이 입력된 순서에 무관하게 정상 동작해야 함.
3. 파일 혹은 디렉토리 입력 개수에 제한 없음
4. 각 요소(파일 유형, 허가권, 링크 수 등) 간에는 탭을 삽입
5. 존재하지 않는 파일이나 디렉토리가 입력으로 들어온 경우 에러 메시지 출력
6. 존재하지 않는 입력과 올바른 입력이 같이 들어온 경우, 존재하지 않는 파일 및 디렉토리에 관한 에러 메시지를 상단에 일괄 출력. 올바른 파일 및 디렉토리에 대한 결과를 하단에 순서대로 출력



## 결과 화면
<img width="365" alt="image" src="https://github.com/syoooooung/SystemProgramming/assets/110389611/29766d29-6cda-4fa3-8110-4eafdd8b3b65">


옵션 -a를 입력으로 주었을 때, 히든 파일을 포함하여 정렬(히든파일은 .을 생략하고 정렬)된 후 출력된다.


<img width="503" alt="image" src="https://github.com/syoooooung/SystemProgramming/assets/110389611/24b97ed4-7675-4341-a649-50952eb71a38">


옵션 -l을 입력으로 주었을 때, directory path, total st_block과 각 요소들을 출력한다.


<img width="518" alt="image" src="https://github.com/syoooooung/SystemProgramming/assets/110389611/4a773fa4-2a88-4d59-a6d7-0548339430d1">


-al 은 각 요소를 히든파일을 포함해서 출력한다.


<img width="515" alt="image" src="https://github.com/syoooooung/SystemProgramming/assets/110389611/23304251-1b18-4b47-9309-09a7393e8055">



상대경로를 입력했을 때도 정상적으로 출력이 된다.


<img width="508" alt="image" src="https://github.com/syoooooung/SystemProgramming/assets/110389611/3208f8d2-50c9-4d3d-a064-55ff6fff6547">


여러 개의 경로를 입력하였고 두가지는 없는 경로, 하나는 존재하는 경로인데, 존재하지 않는다는 문구를 먼저 내보낸 후에 존재하는 경로에 대한 정보를 출력한다.


<img width="473" alt="image" src="https://github.com/syoooooung/SystemProgramming/assets/110389611/011416c7-ed7f-4e04-bc0b-6f0bbe642faf">


파일 입력시에도 출력이 된다.


<img width="520" alt="image" src="https://github.com/syoooooung/SystemProgramming/assets/110389611/b2a29504-5bf5-4076-9217-505a68cf77a8">


절대 경로 입력 시에도 정상 출력된다.


<img width="511" alt="image" src="https://github.com/syoooooung/SystemProgramming/assets/110389611/c5d4ad5d-d80c-4378-a94b-44cabb2ce95e">


경로를 여러 개 입력했을 땐, 순서대로 출력한다.




# Final ls 구현
---
Assignment 1-2 에서 추가 기능들을 구현한다.

## Code Requirements
1. Wildcard matching ('*'. '?', '[seq]')
2. Passing arguments with quotes (작은 따음표)
3. Implements -h, -r, -S option
4. Wildcard를 이용해 디렉토리를 찾을 경우 디렉토리 내용 출력
5. Wildcard는 인자의 끝에만 사용한다고 가정


## 결과 화면
<img width="548" alt="image" src="https://github.com/syoooooung/SystemProgramming/assets/110389611/905dd37c-b9d9-4fc3-978d-4cfba91b55d2">

-h 옵션이 주어지면 human readable format으로 filesize가 출력된다.


<img width="543" alt="image" src="https://github.com/syoooooung/SystemProgramming/assets/110389611/d3c20009-7b41-49b8-8739-2a8bba317030">

-S 옵션이 들어가면 file size 순서대로 정렬이 된다.

<img width="542" alt="image" src="https://github.com/syoooooung/SystemProgramming/assets/110389611/0e0078d6-b87f-438d-a525-4c8200ff0e47">

-r 옵션이 들어가면 파일 이름이 역순으로 정렬된다.

<img width="531" alt="image" src="https://github.com/syoooooung/SystemProgramming/assets/110389611/7d5474d9-bfe3-48f7-822f-ac91ab1dce2c">

-r 옵션과 -S 옵션이 동시에 있다면, file size 순으로 정렬한 후, 같은 filesize인 요소들끼리 이름을 역순으로 정렬한다.


<img width="357" alt="image" src="https://github.com/syoooooung/SystemProgramming/assets/110389611/e37d4e4e-f373-4a77-802c-a3dd8c75c07b">

wildcard가 포함이 된다면 출력 형식이 바뀐다. 파일만 모아서 먼저 출력한 후, 디렉토리 경로를 출력하고 그 디렉토리 안의 요소들을 출력한다. text[1-4].txt 를 입력하면 1부터 4까지에서 겹치는 파일을 찾고 출력한다 

*하나만 입력으로 주면 현재 경로에서의 파일들을 출력한다.

<img width="349" alt="image" src="https://github.com/syoooooung/SystemProgramming/assets/110389611/75dd2c30-984b-4e5a-b758-aeff4d01131a">

경로를 여러 개 입력했을 때이다. 와이들카드가 포함된 인자가 있으므로 그냥 경로가 주어진 것 도 출력 형식이 동일하게 나온다.


## 고찰
strncpy 사용을 했을 때에 뒤에 이상한 문자열이 하나가 더 붙는 일이 발생했다. 그래서 함수 인 자에 수를 하나 더 넣어준건가하여 수를 하나 줄이면 문자가 두개가 줄었다. 왜 이런 오류가 일 어났는지 찾아보았는데, 뒤에 이상한 문자가 붙은 그 자리가 null이 어야하는데 null이 아니어서 일어난 것이었다. 그래서 해당하는 자리에 null값을 넣어주어야 이상한 문자가 출력되지 않을 수 있었다. 또한 와일드 카드 사용시에 인자를 여러 개 받을 때 앞의 인자가 두번째 argv 에 복사되 어서 main 함수에서 문제가 있는 줄 알고 그곳에서만 잘못된 부분을 찾아보았었는데, 알고보니 main 함수에서 다른 함수로 argv 배열을 넘겨주고 그 함수에서 argv에 값을 넣어사용하여 이러한 변수가 생긴 것이었다. 건드리지 말아야할 데이터와 바꿔주어야할 데이터를 명확히 한 후에 코드 를 작성해야겠다고 생각했다. -h 함수 사용할 때에, 수를 적절하게 나눠주고 round 함수를 사용하 고 싶었는데 리눅스에서는 round 함수를 사용하려면 컴파일 시에 -lm을 넣어주어야한다해서 makefile에 추가해준 후 round를 사용했다. 하지만 round를 진행하여도 ls를 직접 해본 거랑 filesize 크기의 오차가 조금 있었다.

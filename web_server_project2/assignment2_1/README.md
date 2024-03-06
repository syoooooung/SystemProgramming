# HTML-ls 구현
---
web_server_project 1 에서 구현한 ls를 사용해 HTML document를 만든다.

## Requirements
1. Output HTML document contains
  - current directory -> title
  - command -> heading
  - results of ls command -> use TABLE
  - file names -> hyperlink to the file
  - output file name : html_ls.html
2. 파일 타입에 따라 다른 컬러로 표시한다.
  - Directory : Blue
  - Link file : Green
  - Other : Red
3. list에 output file 'html_ls.html'은 출력하지 않는다.


## 결과 화면


<img width="360" alt="image" src="https://github.com/syoooooung/SystemProgramming/assets/110389611/9a2c3e68-92c6-4cf0-888b-2ad8d0b523dc">

위와 같이 command를 주었을 때,


<img width="350" alt="image" src="https://github.com/syoooooung/SystemProgramming/assets/110389611/3955880e-cb47-4ec8-9511-68558c2b6f31">


와일드 카드가 존재하여 와일드카드 존재 시 출력형식을 맞추도록 했다. 파일들만 먼저 출력되고, 그 다음 디렉토리들 마다 나뉘어서 출력된다. 디렉토리인 경우엔 파란색, 링크파일인 경우엔 초록색, 나머지는 빨간색으로 표시된다.


<img width="323" alt="image" src="https://github.com/syoooooung/SystemProgramming/assets/110389611/fad997aa-046d-4f06-bc0e-b2c3f05bfc0a">

와일드 카드가 없을 때 -al 옵션 사용


<img width="501" alt="image" src="https://github.com/syoooooung/SystemProgramming/assets/110389611/0f281b85-9609-4ed1-b1b8-43e78c1decb4">

-al 옵션에 맞게 표를 만들어준다.

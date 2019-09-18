## 프로젝트명 : ChattyTalker
>소켓 프로그래밍의 다양한 방식을 사용해 채팅 서버와 클라이언트를 구현한다.
## 방식 목록
     1. 블록소켓 UDP
     2. 블록소켓 TCP
     3. 논블록소켓 UDP
     4. 논블록소켓 TCP
     5. Overllapped I/O
     6. IOCP
## 주요 순서도
![flowchart.png](./image/flowchart.PNG)

## 스크린샷
![ChattyTalker.png](./Common/ChattyTalker.png)

## 클래스 구조
![class_structure_1.PNG](./image/class_structure_1.PNG)
![class_structure_2.PNG](./image/class_structure_2.PNG)


## 개발 환경
* 언어 : C++
* 편집기 : Visual Studio Commnity 2019
* 링커 추가종속성 : ws2_32.lib (Winsock)
* 프로젝트
  * 콘솔 앱 : [Client](./Client/), [Server](./Server/)
  * 참조(공용) 프로젝트: [Common](./Common/)


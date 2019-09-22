# ������Ʈ�� : ChattyTalker
## ����
>���� ���α׷����� �پ��� ����� ����� ä�� ������ Ŭ���̾�Ʈ�� �����Ѵ�.
## ä�ù� ��� ���
     1. ��ϼ��� UDP
     2. ��ϼ��� TCP
     3. ���ϼ��� UDP
     4. ���ϼ��� TCP
     5. Overllapped I/O
     6. IOCP
## �ֿ� ������
�κ� ������ ä�ù��� �����ϰ� ä�ÿ� �����Ѵ�.
![flowchart.png](./image/flowchart.PNG)


## ��ũ����
![ChattyTalker.png](./Common/ChattyTalker.png)

## Ŭ���� ����
 **Client** , **Server** : TCP/IP ������ ����ϴ� ä�� Ŭ���̾�Ʈ/����
![class_structure_1.PNG](./image/class_structure_1.PNG)
 **ChatPacket** : �޼���, ȣ��Ʈ ���� ���� ���� ���� ��Ŷ�� ����ȭ
![class_structure_2.PNG](./image/class_structure_2.PNG)

## ���� ����
* **����** : ���ſϷ� �̺�Ʈ���� �ϷḦ ��ٸ��� �߿� ���� �߰��� ���� �̺�Ʈ�� ��� �̺�Ʈ���� �߰��ϰ� �ʹ�.
* **�ذ�** : **Alertable Wait** ���¸� �ٲٱ� ���Ͽ� �̺�Ʈ���� ù��° �̺�Ʈ�� ���Ƿ� ��ȭ�� ��Ÿ���� �̺�Ʈ�� �����Ͽ� �� ������ ����� ���� ��� �̺�Ʈ�� ��ȣ�� �� �̺�Ʈ�� �߰����� �˷ȴ�.

**OverlappedServ::Run()** �Լ� �Ϻ�
``` c++
accp_sock = accept(serv_sock_, (SOCKADDR*)& clnt_addr, &addrlen);

...

WSASetEvent(num_changed_event_);
```

**OverlappedServ::Chat()** �Լ� �Ϻ�
``` c++
DWORD wait_result = WSAWaitForMultipleEvents(sock_num_, events_, FALSE, WSA_INFINITE, true);
		
int index = wait_result - WSA_WAIT_EVENT_0;

if (index == 0)
{
    WSAResetEvent(num_changed_event_);
    continue;
}
```

## ���� ȯ��
* ��� : C++
* ������ : Visual Studio Commnity 2019
* ��Ŀ �߰����Ӽ� : ws2_32.lib (Winsock)
* ������Ʈ
  * �ܼ� �� : [Client](./Client/), [Server](./Server/)
  * ����(����) ������Ʈ: [Common](./Common/)


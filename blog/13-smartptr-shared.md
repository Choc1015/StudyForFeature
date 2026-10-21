# 주인이 여럿이면 누가 delete할까? MyShared 만들기

## 궁금했던 것

12편의 MyUniquePtr는 주인이 하나뿐이라 복사를 막았다. 그런데 게임에서는 보스 하나를 플레이어 셋이 동시에 타깃으로 잡는 것처럼, 여러 곳이 같은 객체를 가리켜야 할 때가 있다. 그럼 누가 delete해야 할까?

처음엔 상황 자체가 잘 안 들어왔다.

> 자자 이해를 못했어. 스마트 포인터에 3개의 객체를 넣는다?

> 잠만 잠만, 이해를 하나도 못했어.

그래서 진짜 `std::shared_ptr`로 짧은 예시를 돌려 봤다.

```cpp
std::shared_ptr<Boss> playerA(new Boss());
std::shared_ptr<Boss> playerB = playerA;
std::shared_ptr<Boss> playerC = playerA;
playerA.reset();
playerB.reset();
playerC.reset();
```

```
  [Boss created]
A targets the boss.      count = 1
B targets the same boss. count = 2
C targets the same boss. count = 3
same boss? A=...2FA0 B=...2FA0 C=...2FA0
A lets go.               count = 2
B lets go.               count = 1
C lets go...
  [Boss deleted]
```

> 이제 이해가 가네. 오히려 나는 코드를 보니까 이해가 가는 거 같아.

보스는 하나, 가리키는 쪽이 셋. 복사할 때마다 숫자가 올라가고, 놓을 때마다 내려가고, 0이 되는 순간에만 delete된다.

## 실험 설계

### 숫자를 어디에 둘까?

각 스마트 포인터 안에 숫자를 따로 두면?

> 바뀌는 거 아니야?

안 바뀐다. 각자 따로 있는 int라서, A가 자기 숫자를 줄여도 B와 C의 숫자는 그대로다. 아무도 0을 보지 못해서 보스가 영영 지워지지 않는다.

> 숫자를 보스가 가지고 있으면 되잖아?

> 그래서 내가 이야기한 게 보스가 가지고 있으면 된다고 한 건데.

맞는 방법이다. 이걸 침투형(intrusive) 참조 카운팅이라고 한다. 그래서 내 방식대로 먼저 만들어 봤다. Character에 `int count = 0;`을 넣고, MyShared가 그 숫자를 올리고 내리게 했다.

그런데 만들고 보니 걸리는 게 있었다.

> 만약 다른 클래스에 count가 없는 걸 넣으면 에러 아닌가?

```
MyShared.h(17): error C2039: 'count': is not a member of 'std::basic_string<...>'
```

`int`나 `std::string`처럼 내가 고칠 수 없는 타입에는 숫자를 넣을 곳이 없다. 그래서 `std::shared_ptr`는 숫자를 보스 밖에 따로 만든다. 보스를 `new`로 하나 만들어 주소를 같이 들고 있는 것처럼, 숫자도 `new int`로 하나 만들어 주소를 같이 든다.

```cpp
template <typename T>
class MyShared
{
public:
	T* Data;
	int* NumPointer;

	MyShared(T* Other)
	{
		Data = Other;
		NumPointer = new int();
		*NumPointer += 1;
	}

	virtual ~MyShared()
	{
		*NumPointer -= 1;
		if (*NumPointer == 0)
		{
			delete NumPointer;
			delete Data;
		}
	}

	MyShared(const MyShared& Other)
	{
		Data = Other.Data;
		NumPointer = Other.NumPointer;
		*NumPointer += 1;
	}
	// 복사 대입은 아래에서
};
```

이제 `MyShared<int>`, `MyShared<std::string>`도 컴파일된다.

## 결과

### 복사 생성

```
A: 1
B: 2
B 놓음: 1
Player
Character
끝
```

MyVector에서는 버그였던 얕은 복사를, 여기서는 일부러 한다. 대신 숫자로 마지막 주인을 찾아서 이중 해제를 막는다.

### 복사 대입: 네 번 고쳤다

c가 P2를 보다가 `c = a`로 P1을 보게 되면, c는 P2에서 손을 놓아야 한다.

1. 놓는 처리 없이 바로 받음 → P2 누수
2. `Data = nullptr; NumPointer = nullptr;` → 주소를 잊었을 뿐, P2는 숫자 1인 채로 남아 누수. **잊기와 놓기는 다르다.**
3. `if (*NumPointer == 0)`으로 delete → 숫자를 줄이지 않아서 1이 0이 될 일이 없음
4. `*NumPointer -= 1;`을 먼저 → 성공

```cpp
MyShared& operator=(const MyShared& Other)
{
	if (this == &Other)
	{
		return *this;
	}

	*NumPointer -= 1;
	if (*NumPointer == 0)
	{
		delete NumPointer;
		delete Data;
	}

	Data = Other.Data;
	NumPointer = Other.NumPointer;
	*NumPointer += 1;

	return *this;
}
```

```
[c = a]  대입하는 순간 Player / Character (P2 해제) → count 2 → 끝에서 P1 해제
[a = a]  count 1, 끝에서 한 번 해제
```

## 왜 그런가

- 여러 주인이 같은 객체를 볼 때는 "몇 명이 보고 있나"를 모두가 같이 봐야 한다. 그래서 숫자도 하나만 만들고 주소를 공유한다.
- 숫자를 객체 안에 두는 방식(침투형)도 되지만, 객체 타입을 고쳐야 해서 아무 타입에나 쓸 수 없다.
- 놓기 = 숫자 -1, 0이면 delete. 잊기 = 주소를 nullptr로. 이동에서 넘겨준 쪽은 잊고, 소멸자와 대입에서는 놓는다.

## 정리

> ✏️ 작성 필요: 이 글의 결론을 세 줄로. (처음에 무엇이 이해가 안 됐고 무엇으로 이해했는지, 숫자를 어디에 두는지, 잊기와 놓기의 차이를 넣으면 좋아요)

## 더 궁금해진 것

> ✏️ 작성 필요: 새로 궁금해진 점 한 가지. 없으면 "없음"이라고 보내 주세요.

# 서로를 붙잡으면 아무도 안 지워진다: weak_ptr 직접 만들기

## 궁금했던 것

13편의 shared_ptr은 마지막 주인이 놓을 때 객체를 지운다. 그런데 플레이어는 자기 길드를, 길드는 길드장(플레이어)을 서로 shared_ptr로 들고 있으면 어떻게 될까?

```cpp
struct Player { std::shared_ptr<Guild>  MyGuild; };
struct Guild  { std::shared_ptr<Player> Leader;  };

std::shared_ptr<Player> p(new Player());
std::shared_ptr<Guild>  g(new Guild());
p->MyGuild = g;
g->Leader  = p;
```

```
player count = 2, guild count = 2
end                      ← 아무것도 해제되지 않음
```

직접 따라가 봤다.

> 처음에 p에 플레이어, g에는 길드, myguild와 Leader는 비어 있는 상태. 다음에는 g에서 보는 길드를 myguild가 보고 있고, 플레이어 카운트 1, 길드 2. 다음에는 p와 리더가 플레이어를 보고 있고 카운트도 각각 2. 소멸할 때는 나중에 만든 g부터 시작, 카운트가 0이 아니기에 카운트만 줄이고 사라짐, 플레이어 2 길드 1. p 또한 하나 줄여도 1이어서 결국 둘 다 해제가 안 되는 건가?

맞다. 남은 1은 서로의 멤버가 들고 있다. 밖에서는 아무도 둘을 가리키지 않는데, 서로가 서로를 붙잡고 있어서 영영 0이 되지 않는다. 이걸 **순환 참조**라고 한다.

## 실험 설계

### 한쪽이 "보기만" 하면

길드는 길드장이 누군지 알기만 하면 되고, 살려 둘 책임은 없다. `Leader`를 숫자를 올리지 않는 `std::weak_ptr`로 바꿨다.

> 보기만 한다는 게 이해가 안 가는데, 플레이어 숫자가 1이면 처음에는 1 2, g가 사라질 때 1 1, p가 사라질 때 0 1. 그래도 길드는 남아 있는 거 아닌가?

```
player count = 1, guild count = 2
guild can still see its leader? yes
[Player deleted]
[Guild deleted]
```

빠진 단계는, Player가 지워질 때 Player 안의 멤버 `MyGuild`도 같이 사라지면서 Guild 숫자가 1에서 0이 되는 것이었다.

> 그러니까 weak 포인터는 공유 포인터에서 count만 없는 개념인 건가?

거의 맞다. 그리고 일반 포인터와 다른 점이 하나 더 있다.

```
raw  : still has address (dangling!)
weak : expired (knows it's gone)
lock : nothing, player is gone
```

weak_ptr는 대상이 지워졌는지 알 수 있다.

### 직접 만들기: 숫자가 두 개 필요한 이유

> 직접 만들어 보자. 강한, 약한 숫자 이해가 안 돼.

진짜 shared_ptr가 숫자 덩어리를 언제 만들고 지우는지 찍어 봤다.

```
1. make shared_ptr s     -> [count block created]   strong 1, weak 0
2. make weak_ptr w        strong 1, weak 1
3. s lets go             -> [Boss deleted]           strong 0, weak 1
   w asks: is boss alive? no, expired
4. w lets go             -> [count block deleted]
```

보스는 strong이 0일 때 지운다. 하지만 숫자 덩어리는 weak_ptr가 "죽었네"를 확인할 수 있게 weak까지 0이 될 때까지 남겨야 한다. 그래서 MyShared의 숫자를 `ControlBlock { Strong, Weak }`으로 바꿨다.

```cpp
struct ControlBlock
{
	int Strong = 0;
	int Weak = 0;
};
```

> 구조체를 new로 생성하는 부분이 틀린 거 같은데 내가 뭘 잘못 알고 있지?

`new`는 맞았다. 문제는 복사 생성자에서 덩어리 주소가 아니라 숫자 값을 복사한 것이었다.

> 처음에 복사 생성자에다가 new로 새로운 객체를 받았다가, 출력 결과가 1 1 1이 떠서 디버깅해 보니 a의 ControlBlock과 b의 ControlBlock이 복사된 게 아니라 따로 놀고 있어서 뭐지 했는데, b의 ControlBlock이 a랑 얕은 복사되어야 하니까 같은 객체를 받아야 한다는 걸 깨닫고 고침.

### MyWeak

```cpp
template <typename T>
class MyWeak
{
public:
	T* Data;
	ControlBlock* ControlNum;

	MyWeak(const MyShared<T>& Shared)
	{
		Data = Shared.Data;
		ControlNum = Shared.ControlNum;
		Shared.ControlNum->Weak++;
	}
	virtual ~MyWeak()
	{
		ControlNum->Weak--;
		if (ControlNum->Strong == 0 && ControlNum->Weak == 0)
		{
			delete ControlNum;
		}
	}

	bool Expired()
	{
		if (ControlNum->Strong == 0)
		{
			return true;
		}
		return false;
	}
	// Lock()은 아래에서
};
```

처음 테스트는 Player가 한 번 해제되는 것만 봐서, 숫자가 하나든 둘이든 출력이 같았다.

> 아니 그러니까 예시 코드가 좀 별로였다 이거지.

그래서 "s가 사라진 순간 보스가 지워졌는가"를 보는 `Expired()`로 확인했다.

```
strong 1, weak 1
Player
Character
s 놓음
Expired: 1
```

덩어리를 일찍 지우는 버전과 비교하면, s가 사라진 뒤 w가 본 숫자가 `strong = 0`이 아니라 `-572662307`(0xDDDDDDDD, 해제된 메모리)였다.

### Lock()

> Lock이 뭔데?

> Lock이라는 건 살아 있으면 확인하는 사이 죽지 않게 Strong을 하나 추가해서 보고, 다 보면 다시 빼는 기능인가?

맞다. 빼는 건 Lock이 돌려준 MyShared의 소멸자가 자동으로 한다.

Lock이 "죽었으면 빈 것"을 돌려주려면 먼저 빈 MyShared가 필요했다. 기본 생성자, 소멸자, 복사 생성자, 복사 대입에 nullptr 처리를 하나씩 넣었다. 복사 생성자 대신 복사 대입에 넣기도 하고, 숫자를 줄이는 한 줄만 감싸서 바로 다음 줄에서 터지기도 했다.

```cpp
MyShared<T> Lock()
{
	if (this->Expired())
	{
		MyShared<T> emptyShared;
		return emptyShared;
	}
	else
	{
		MyShared<T> emptyShared;
		ControlNum->Strong++;
		emptyShared.Data = Data;
		emptyShared.ControlNum = ControlNum;
		return emptyShared;
	}
}
```

처음엔 `MyShared<T> emptyShared = new T();`로 시작해서, 쓰지도 않을 Character와 덩어리를 하나씩 만들었다가 덮어써 누수가 났다.

```
8 bytes, Data: 01 00 00 00 00 00 00 00   ← ControlBlock (Strong 1, Weak 0)
8 bytes, Data: 90 45 2C 33 F7 7F 00 00   ← Character (vptr 8바이트)
```

## 결과

```
빌리는 중 strong: 2
다 씀 strong: 1
Player
Character
죽은 뒤 Lock: 1
끝
--- leak check ---
no leaks
```

## 왜 그런가

- 서로를 shared_ptr로 붙잡으면 숫자가 0이 될 수 없어 누수가 난다 (순환 참조). 한쪽을 weak_ptr로 바꾸면 고리가 끊긴다.
- weak_ptr는 숫자를 올리지 않고 보기만 한다. 대신 대상이 지워졌는지 알 수 있다.
- 그러려면 숫자 덩어리가 보스보다 오래 살아야 해서 숫자가 두 개 필요하다. strong 0이면 보스, strong과 weak 둘 다 0이면 덩어리를 지운다.
- Lock()은 살아 있으면 strong을 올린 MyShared를 돌려줘서, 쓰는 동안 안 지워지게 한다. 다 쓰면 소멸자가 알아서 내린다.

## 정리

- 유니크 포인터는 복사가 안 되는 단 하나의 객체 포인터.
- 공유 포인터는 얕은 복사의 원리로 하나의 객체를 여러 포인터에서 가질 수 있게 해 주는 기능.
- 약한 포인터는 공유 포인터의 약한 버전 느낌. 가지지는 않고 해당 객체가 뭔지 보기만 할게. 혼자 쓰이기보단 공유 포인터와 같이 쓰인다.

## 더 궁금해진 것

이런 포인터들을 실제 개발 때 언제 쓰는지 궁금하긴 한데, 어차피 하다 보면 나올 테니 넘어가는 걸로. 그때 다시 보면 기억에 도움도 될 거 같으니.

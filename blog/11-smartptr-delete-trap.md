# delete를 깜빡하면? 직접 만든 스마트 포인터로 막아 보기

## 궁금했던 것

`new`로 만든 객체는 `delete`를 직접 해야 한다. 그런데 함수 중간에 `return`으로 일찍 나가면 어떻게 될까?

## 실험 설계

실험 1에서 만든 `Character`와 `Player`를 다시 썼다. 둘 다 소멸자에서만 이름을 출력한다.

```cpp
void Spawn(bool bCancel)
{
	Character* c = new Player();
	if (bCancel)
	{
		return;      // 스폰 취소!
	}
	delete c;
}
```

```cpp
Spawn(false);
cout << "---" << endl;
Spawn(true);
```

처음 내 예측:

> Spawn(false): 소멸자에서 delete할 객체가 사라져서 댕글링 포인터가 발생?
> Spawn(true): 생성자 소멸자 텍스트가 실행되겠지

코드를 한 줄씩 따라가 본 뒤 다시 한 예측:

> 음.. 순간 자동으로 해제해 줄 줄 알았는데 포인터 타입 선언이었네. 그치, 그러면 false만 해제하고 true는 메모리에 계속 남아 있겠지.

## 결과

### 직접 delete

```
Player
Character
---
```

`---` 아래에 아무것도 없다. `Spawn(true)`의 Player는 소멸자가 불리지 않고 메모리에 남았다.

### 자동으로 delete해 주는 클래스 만들기

MyVector는 객체가 사라질 때 소멸자에서 `delete[] data;`를 했다. 같은 원리로, 포인터를 들고 있다가 자기가 사라질 때 `delete`해 주는 클래스를 만들면 될까?

> 뭐 그렇겠지?

직접 만들어 확인했다.

> new로 만든다는 건 무슨 이야기일까?

MyUniquePtr가 직접 `new`를 하는 게 아니라, 밖에서 `new Player()`로 만든 주소를 받아서 들고 있는 것이다.

첫 시도는 이렇게 나왔다.

> 지금 Character만 나와.

```
Character
---
Character
```

멤버를 `T SavePointer;`로 만들어서, 주소가 아니라 Player를 Character 모양으로 복사한 실물을 저장했다. 사라진 건 복사본이고 진짜 Player는 여전히 남아 있었다.

> 주소를 담는 타입은 T&가 아니었나?

`T*`는 주소를 담는 변수라 `delete`할 수 있고, `T&`는 이미 있는 객체의 별명이라 `delete`할 수 없다. `new`가 돌려주는 것도 포인터다.

```cpp
template <typename T>
class MyUniquePtr
{
public:
	T* SavePointer;

	MyUniquePtr(T* Tname)
	{
		SavePointer = Tname;
	}
	virtual ~MyUniquePtr()
	{
		delete SavePointer;
	}
};
```

(중간에 `delete[]`로 썼다가 `delete`로 고쳤다. `new` 하나에는 `delete`, `new[]`에는 `delete[]`.)

```cpp
void Spawn(bool bCancel)
{
	MyUniquePtr<Character> c(new Player());
	if (bCancel)
	{
		return;
	}
}
```

```
Player
Character
---
Player
Character
```

`delete`를 한 줄도 안 썼는데, 일찍 `return`해도 Player가 해제됐다.

## 왜 그런가

- 포인터 변수 `c`는 함수가 끝나면 사라지지만, `c`가 가리키던 객체는 그대로 남는다. 그래서 `delete`를 놓치면 누수가 된다.
- 스택에 만든 객체는 함수가 어떻게 끝나든 소멸자가 자동으로 불린다.
- 그래서 포인터를 객체 안에 넣고, 그 객체의 소멸자에서 `delete`하게 하면 자동으로 해제된다. 이게 스마트 포인터의 기본 원리이고, `std::unique_ptr`, 언리얼의 `TUniquePtr`가 이 모양이다.

## 정리

아직도 "주소를 담는다" 또는 "생성자에 new를 담는다"고 하면 이해가 안 갔던 것 같다. 다음 예제 또는 다음에도 헷갈릴 수도 있지만, 반복하다 보면 늘지 않을까 싶다.

그리고 스마트 포인터 원리는 결국 포인터를 객체화시켜서 자동으로 해제되도록 하는 원리인 것 같다.

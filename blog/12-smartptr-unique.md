# 주인은 하나뿐: MyUniquePtr에 복사 금지와 이동 넣기

## 궁금했던 것

11편에서 만든 MyUniquePtr는 소멸자에서 `delete`를 해 줘서, 함수에서 일찍 `return`해도 객체가 해제됐다. 그런데 이걸 복사하면 어떻게 될까?

```cpp
{
	MyUniquePtr<Character> a(new Player());
	MyUniquePtr<Character> b = a;
}
cout << "끝" << endl;
```

실행 전 내 예측:

> 흠, 주소가 복사되지 않을까?

> 이중 해제로 댕글링 포인터 발생

## 실험 설계

1. 복사 생성자 없이 복사해서 무슨 일이 생기는지 본다.
2. 복사를 막는다.
3. 주인을 넘기는 이동 생성과 이동 대입을 만든다.

## 결과

### 1. 복사하면 이중 해제

> 실행했더니 소멸자 두 번 나왔으.

```
Player
Character      ← b의 소멸자가 delete (정상)
(크래시)       ← a의 소멸자가 같은 주소를 또 delete
```

"끝"은 출력되지 않았다. b가 먼저 Player를 해제하고, a는 이미 해제된 주소(댕글링 포인터)를 들고 있다가 다시 `delete`하면서 이중 해제로 죽었다.

### 2. 복사 금지

MyVector는 깊은 복사로 풀었다. 그런데 MyUniquePtr는 이름 그대로 주인이 하나뿐이어야 한다.

> 복사를 못 하게 해야 하는 거 아닌가?

```cpp
MyUniquePtr(const MyUniquePtr& Other) = delete;
void operator=(const MyUniquePtr& Other) = delete;
```

이제 복사하려고 하면 빌드할 때 에러가 난다.

```
error C2280: 'MyUniquePtr<Character>::MyUniquePtr(const MyUniquePtr<Character> &)': attempting to reference a deleted function
```

실행 중에 터지던 이중 해제가, 빌드할 때 미리 잡히게 됐다.

### 3. 주인 넘기기: 이동

복사는 막았지만, 함수 밖으로 돌려주거나 다른 곳에 넘겨주는 건 필요하다.

> 이동 생성자랑 이동 대입을 쓰면 되나?

```cpp
MyUniquePtr(MyUniquePtr&& Other)
{
	SavePointer = Other.SavePointer;
	Other.SavePointer = nullptr;
}

MyUniquePtr& operator=(MyUniquePtr&& Other)
{
	if (Other.SavePointer == SavePointer)
	{
		return *this;
	}

	delete SavePointer;
	SavePointer = Other.SavePointer;
	Other.SavePointer = nullptr;

	return *this;
}
```

```
[이동 생성]  1 (a는 nullptr) / Player / Character (한 번만) / 끝
[이동 대입]  Player / Character / 대입 끝 / Player / Character / 끝
```

처음엔 이동 대입에 자기 대입 확인이 없어서, `a = std::move(a);`를 하면 자기 Player를 지우고 빈손이 됐다.

```
before: a holds a Player
Player
Character
after a = std::move(a): a holds nothing
```

확인을 넣은 뒤에는 `a holds a Player`로 남았다.

## 왜 그런가

- 포인터를 그대로 복사하면 주인이 둘이 되고, 둘 다 `delete`하다 이중 해제가 난다.
- unique_ptr은 주인이 하나뿐이라는 규칙을 복사 금지(`= delete`)로 지킨다. 컴파일러가 규칙을 대신 지켜 준다.
- 주인을 바꿀 때는 이동을 쓴다. 넘겨준 쪽은 nullptr이 되고, nullptr을 `delete`하는 건 아무 일도 하지 않으니 안전하다.
- 소멸자에서 delete, 복사 금지, 이동 생성, 이동 대입. 이게 `std::unique_ptr`의 핵심 구조이고, 언리얼의 `TUniquePtr`도 같다.

## 정리

일단 개념은 이제 다 알겠는데, 아직도 어디를 건드려야 하는지 조금 어려운 것 같다. 원래 만들었던 걸 참고하면서 만들었는데, 다음에는 그냥 한 번에 나오면 좋겠다.

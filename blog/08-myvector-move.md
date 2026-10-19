# 100만 개를 복사하지 않고 넘기는 법: 이동

## 궁금했던 것

`MyVector<string> a`에 값이 100만 개 들어 있고, a를 이 뒤로 다시 안 쓴다면? `MyVector<string> b = a;`는 100만 개를 하나하나 복사하고 원본은 그냥 버리는 셈이다. 판을 복사하지 않고 b에게 넘길 수는 없을까?

처음 내 생각:

> 판을 복사하지 않고 넘기는 거면 참조로 주면 되는 거 아니야? 대신 a를 바꾸면 b도 바뀌겠지? 비유하자면 계란판을 둘이서 잡고 있는 느낌?

둘이서 잡으면 2편의 얕은 복사와 같다. 끝날 때 두 소멸자가 같은 판을 해제한다. 이동은 같이 잡는 게 아니라 **건네주고 손을 놓는 것**이다.

```
복사:  a [판1]  →  a [판1],   b [판2 (새로 만들어 하나하나 옮김)]
공유:  a [판1]  →  a [판1],   b [판1]   ← 이중 해제
이동:  a [판1]  →  a [ 없음 ], b [판1]   ← 판은 하나, 주인도 하나
```

그럼 a는 어떻게 손을 놓을까?

> 그냥 delete를 넘겨줬을 때 하면 되는 거 아닌가?

delete는 판 자체를 없앤다. 판은 이제 b의 것이라 없애면 안 된다. a는 판의 주소만 잊으면 된다.

> nullptr?

## 실험 설계

### 이동 생성자

```cpp
MyVector(MyVector&& InVector) noexcept
{
	data = InVector.data;
	sizeValue = InVector.sizeValue;
	capacityValue = InVector.capacityValue;

	InVector.data = nullptr;
	InVector.sizeValue = 0;
	InVector.capacityValue = 0;
}
```

`&&`는 곧 사라지거나 더 이상 안 쓸 객체를 받는다는 표시다. 그래서 상대를 바꿔야 하니 `const`가 없다. `std::move(a)`는 "a는 이제 안 쓸 테니 이동해도 된다"는 표시다.

### 이동 대입

이미 자기 판이 있는 객체에 이동할 때.

> 복사 대입할 때 이중 해제랑 메모리 누수가 있었지. 여기도 마찬가지로 옮기고 이번에 delete 해 주면 될 거 같은데.

처음엔 받은 판을 바로 delete해 버렸고, 그다음엔 판만 받고 size와 capacity를 안 받았고, 그다음엔 쓰지도 않을 판을 `new`로 만들었다가 덮어썼다.

> 만들기 전에 해제하고 null 초기화한 다음에 넣는다?

이동에서는 판을 만들지 않는다. nullptr로 만드는 건 넘겨준 쪽이다.

```cpp
MyVector& operator=(MyVector&& InVector) noexcept
{
	if (this == &InVector)
	{
		return *this;
	}

	delete[] data;

	sizeValue = InVector.sizeValue;
	capacityValue = InVector.capacityValue;
	data = InVector.data;

	InVector.data = nullptr;
	InVector.sizeValue = 0;
	InVector.capacityValue = 0;

	return *this;
}
```

## 결과

```
[move]       same tray handed over: yes
             m1: size 0, capacity 0, []
             m2: size 3, capacity 4, [melon, kiwi, plum]
             이동한 뒤의 m1에 다시 push_back → [reuse]
             no leaks
[moveassign] m3{grape} = std::move(m2) → m3: [melon, kiwi, plum], m2: 빈 벡터
             자기 이동, 빈 벡터 이동 → 정상
             no leaks
```

`same tray handed over: yes`: 판을 새로 만들지 않고 주소만 넘겼다. 100만 개든 1억 개든 포인터 하나만 옮기면 끝난다.

## 왜 그런가

- 복사는 판을 새로 만들어 계란을 하나하나 옮긴다. 이동은 판의 주소만 넘긴다.
- 넘겨준 쪽은 판을 지우는 게 아니라 주소를 잊는다 (nullptr, 0, 0). 그래서 나중에 소멸자가 불려도 안전하고, 다시 써도 된다.
- 이동 대입은 이동 생성자 + "내 원래 판 버리기" + 자기 대입 확인이다.
- 소멸자, 복사 생성자, 복사 대입, 이동 생성자, 이동 대입. 이 다섯 개를 묶어 **5의 법칙**이라고 한다.

## 정리

> ✏️ 작성 필요: 이 글의 결론을 세 줄로. (복사와 이동의 차이, 넘겨준 쪽이 해야 할 일, 이동 대입에서 헷갈렸던 것을 넣으면 좋아요)

## 더 궁금해진 것

> ✏️ 작성 필요: 새로 궁금해진 점 한 가지. 없으면 "없음"이라고 보내 주세요.

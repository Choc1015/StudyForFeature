# MyVector를 복사했더니 프로그램이 터졌다: 얕은 복사, 깊은 복사, 그리고 자기 대입

## 궁금했던 것

1편에서 만든 MyVector를 복사하면 어떻게 될까? 복사 생성자를 만들지 않은 채로 해 봤다.

```cpp
MyVector a;
a.push_back(1);
MyVector b = a;
```

실행 전 내 예측:

> 잘 모르겠다. 그냥 값이 옮겨지는 거 아닌가?

## 실험 1: 복사 생성자 없이 복사하기

### 결과

> 복사는 잘 되는데, 해제될 때 `_free_dbg(block, _UNKNOWN_BLOCK);`이 실행되면서 에러를 띄우네. 이게 뭔지는 모르겠어.

### 왜 그런가

디버거로 `a.data`와 `b.data`를 비교해 보고 찾은 원인:

> a.data와 b.data는 같은 주소를 내포하고 있어서, delete를 두 번째 할 때는 없는 주소를 해제하려고 해서 에러가 나는 건가?

자동으로 만들어지는 복사 생성자는 멤버를 그대로 복사한다. data는 포인터라서 판이 아니라 주소만 복사된다 (얕은 복사). 그래서 소멸자 두 개가 같은 판을 두 번 해제한다 (이중 해제).

### 그럼 두 번째 해제만 건너뛰면 되지 않나?

> 이중 해제를 그냥 하면 되는 거 아니야? 없으면 그냥 없구나 하고 넘기면 되잖아.

a는 판이 이미 해제됐는지 알 방법이 없다. 그리고 진짜 문제는 판 하나를 같이 쓰는 것이라서, 복사본을 바꾸면 원본도 바뀐다. "마지막 사람만 해제"하려면 쓰는 사람 수를 세야 하고, 그게 `std::shared_ptr`의 원리다.

## 실험 2: 복사 생성자 만들기 (깊은 복사)

처음 만든 복사 생성자는 원본 값을 하나씩 `push_back`했다. main에서는 됐지만, 검사해 보니 빈 벡터를 복사하면 이렇게 됐다.

```
b: data CDCDCDCDCDCDCDCD, size -842150451, capacity -842150451
delete b → exit code -1073741819
```

복사 생성자도 생성자라서, 시작할 때 멤버 변수가 쓰레기 값이었다. 초기화를 넣고, 판을 한 번에 만드는 방식으로 바꿨다.

```cpp
MyVector::MyVector(const MyVector& InVector)
{
	if (InVector.capacityValue == 0)
	{
		data = nullptr;
		sizeValue = 0;
		capacityValue = 0;
		return;
	}

	data = new int[InVector.capacityValue];
	sizeValue = InVector.sizeValue;
	capacityValue = InVector.capacityValue;

	for (int i = 0; i < InVector.size(); i++)
	{
		data[i] = InVector.data[i];
	}
}
```

> const를 꼭 써야 하는지 모르겠어.

없어도 컴파일은 되지만, 원본을 실수로 바꾸는 걸 막아 주고, `const MyVector&`로 받은 벡터도 복사할 수 있게 해 준다.

## 실험 3: 복사 대입 (`d = a;`)

이미 살아 있는 객체에 대입하면 어떻게 될까?

```cpp
MyVector d;
d.push_back(9);
d = a;
```

실행 전 내 예측:

> 일단 d.data는 a.data 판을 가리키게 되고, 원래 판은 가리키는 값이 없어지면서 메모리 누수가 생기는 거 같아.

결과:

> 아까와 똑같은 에러가 났어.

누수와 이중 해제, 두 문제가 한꺼번에 생겼다.

### 대입 연산자를 만들면서 만난 함정들

1. 복사 생성자에도 `delete[] data;`를 넣었다가 모든 복사가 크래시했다. 생성자의 data는 쓰레기 값이라 해제할 판이 없다.

   > 복사 생성자 쪽에서 delete 해 줘야 되나?

   복사 생성자는 갓 태어난 객체, 대입은 이미 살아 있는 객체. 원래 판을 해제하는 건 대입에서만 한다.

2. 매개변수를 값으로 받고, 사라질 매개변수를 `return`했다 (댕글링 참조).
3. 빈 벡터를 대입받으면 `new int[0]`을 해서, 다음 `push_back`에서 0바이트 누수가 생겼다.
4. 해제 조건을 "원본이 비었을 때만"으로 바꿨다가, 보통의 대입에서 원래 판 `[9]`가 샜다.
5. `delete[]`를 자기 대입 확인보다 앞에 둬서, `a = a`에서 자기 판을 해제했다. 값이 전부 `0xDDDDDDDD`(해제된 메모리)로 바뀌고 크래시.

### 최종 코드

```cpp
MyVector& MyVector::operator=(const MyVector& InVector)
{
	if (this == &InVector)
	{
		return *this;
	}
	delete[] data;
	if (InVector.capacityValue == 0)
	{
		data = nullptr;
		sizeValue = 0;
		capacityValue = 0;
	}
	else
	{
		data = new int[InVector.capacityValue];
		sizeValue = InVector.sizeValue;
		capacityValue = InVector.capacityValue;
	}

	for (int i = 0; i < InVector.size(); i++)
	{
		data[i] = InVector.data[i];
	}

	return *this;
}
```

## 결과

```
[copy]      b = a로 생성          → 주소 다름, [1, 2, 3], no leaks
[assign]    d{9} = a{1,2,3}       → 주소 다름, d만 [100, 2, 3], no leaks
[self]      a = a                 → [1, 2, 3] 그대로, no leaks
[chain]     x = y = z             → 셋 다 주소 다름, no leaks
[empty]     d{9} = 빈 a           → size 0, capacity 0, no leaks
[emptypush] 빈 a 대입 후 push_back → [7], no leaks
```

## 정리

일단 처음에 복사 생성자를 만들지 않고 선언과 동시에 원래 있던 객체로 초기화를 하니까, 해당 객체의 포인터를 참조하게 되면서 얕은 복사가 일어난다. 데이터를 볼 때는 문제없는 줄 알았지만, 원래 객체의 데이터를 바꾸면 후에 만든 객체의 값도 바뀌게 되는 의도와 다른 행동이 발생하였고, 이중 해제가 일어나면서 에러가 났다.

그래서 복사 생성자를 만들게 되었는데, 복사 생성자를 어떻게 만드는지 이번에 제대로 이해한 것 같다. 매개변수를 만들 때는 const로 해당 매개변수를 수정하지 않게 규약을 주었고, 복사 생성자의 정의 부분에서 어떻게 값들을 옮겨야 하는지 이해했다. 여기서 가장 중요했던 건 data를 어떻게 만들고 관리하느냐.

복사 대입이라는 것도 해 봤는데, 오퍼레이터에 관해서는 저번에만 찾아보고, 이번 대입 오퍼레이터를 사용할 때는 저번에 찾아봤던 걸 기준으로 내가 알아서 구현하게 되었다. 복사 대입을 할 때는 원래 있던 판에 대해서 어떻게 할 것인지 관리했어야 했던 게 좋았다.

마지막으로 this를 이번에 쓰면서, 아 나 자신을 이럴 때 내보내야 하니까 쓰는구나라고 알았다.

## 더 궁금해진 것

> ✏️ 작성 필요: 새로 궁금해진 점 한 가지. 없으면 "없음"이라고 보내 주세요.

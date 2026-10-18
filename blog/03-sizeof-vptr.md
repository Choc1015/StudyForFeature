# virtual 함수가 있으면 객체 크기가 달라질까?

## 궁금했던 것

2편에서 virtual 함수가 있는 객체 안에는 vtable을 찾기 위한 숨은 포인터(vptr)가 있다는 걸 알았다. 정말 객체 안에 들어 있다면 `sizeof`로 크기를 재서 확인할 수 있지 않을까?

실행 전 내 예측:

> 몇 바이트일지는 모르겠지만 Character가 큰 거는 오케이

힌트를 받고 다시 한 예측:

> noCharacter는 1바이트 (객체의 주소에 대한 크기?), Character는 가상 테이블이 존재하기에 8바이트?

## 실험 설계

멤버 변수가 하나도 없는 두 클래스를 비교했다.

- `Character`: virtual 함수(`Attack`, 소멸자)가 있음
- `noCharacter`: virtual 함수가 없음

## 결과

### 첫 번째 측정: 잘못 쟀다

```cpp
int cbit = sizeof(c);
int ncbit = sizeof(nc);
```

디버거로 확인하니 둘 다 8이 나왔다.

> Character 8바이트는 이해하겠는데 nc는 왜인지 모르겠네

원인은 `c`와 `nc`가 객체가 아니라 **포인터 변수**라는 것이었다. 객체 크기가 아니라 포인터 크기(x64에서 8바이트)를 잰 것이다. Character의 8이 예측과 맞은 것도 우연이었다.

### 두 번째 측정

```cpp
int cbit = sizeof(Character);
int ncbit = sizeof(noCharacter);
```

```
Character   : 8
noCharacter : 1
```

> 오, 클래스 자체를 넣으니까 Character는 8, noCharacter는 1이 나왔네

## 왜 그런가

처음 해석:

> 결국 빈 클래스도 주소 저장을 위한 메모리로 1바이트를 받은 거고, Character는 가상 테이블을 위해 포인터 공간이 필요해서 8바이트인 건가?

### Character는 왜 1 + 8 = 9바이트가 아닐까?

> 주소를 찾을 메모리가 필요했던 건데, Character는 이미 메모리가 있기에 따로 추가되지 않고 해당 가상 포인터에 주소를 가져오는 거지.

## 정리

1. 빈 클래스가 1바이트를 차지하는 이유는 그 차지하는 1바이트의 주소가 클래스의 주소값이 되기 때문이다.
2. 객체 안에는 vptr이 있고, 그게 vtable을 알려 준다.
3. 클래스에 vptr이 있으므로 8바이트가 되는 거고, vptr이 알려 주는 값은 vtable이다.

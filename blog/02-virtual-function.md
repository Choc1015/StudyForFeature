# 부모 포인터로 자식 객체의 함수를 부르면 누구의 함수가 불릴까?

## 궁금했던 것

`Character* c = new Player();`에서 `c->Attack()`을 호출하면 왜 Player의 함수가 불리는지 질문을 받았다.

처음 내 답:

> 캐릭터 클래스로 만들었지만 할당하는 클래스는 플레이어라서 자식 클래스의 함수 복사되기 때문이다.

## 실험 설계

1편의 두 쌍(Character/Player, noCharacter/noPlayer)에 `Attack()`을 추가했다.

```cpp
// Character.h
virtual void Attack();

// Player.h
virtual void Attack() override;

// noCharacter.h, noPlayer.h
void Attack();
```

```cpp
Character* c = new Player();
noCharacter* nc = new noPlayer();

c->Attack();
nc->Attack();
```

## 결과

```
Player Attack
noCharacter Attack
```

## 왜 그런가

처음 해석:

> 가상함수로 오버라이딩 했을 때는 character의 attack 함수에 player attack 함수가 덮어졌지만, 하지 않았을 때는 noPlayer attack을 읽어오지 못하고 noCharacter의 attack 함수를 실행했다.

다시 생각한 해석:

> 가상함수로 오버라이딩 했을 때는 가상 포인터가 Player의 가상 테이블을 읽고 Player의 Attack 함수를 대신 실행시킨다. 하지 않았을 때는 다른 객체를 보지 않고 noCharacter의 Attack 함수를 실행했다.

## 확인: 정말 덮어써지지 않았을까?

- `c->Character::Attack();`의 출력 예측: Character Attack
- `noPlayer* np = new noPlayer(); np->Attack();`의 출력 예측: noPlayer Attack

## 정리

> ✏️ 작성 필요: 이 글의 결론을 세 줄로. (처음 답 "복사된다"가 왜 틀렸는지 한 줄 넣으면 좋아요)

## 더 궁금해진 것

> ✏️ 작성 필요: 새로 궁금해진 점 한 가지.

# override 키워드는 없어도 동작하는데, 왜 붙일까?

## 궁금했던 것

처음 진단에서 "override는 왜 붙이나?"라는 질문을 받았다.

처음 내 답:

> 명확성을 위해서이다.

정말 그게 전부일까? const 하나만 다르게 써서 확인해 봤다.

## 실험 설계

부모와 자식의 `Attack()`을 const 하나만 다르게 선언했다.

```cpp
// Character.h
virtual void Attack() const;

// Player.h
virtual void Attack();   // const 없음, override 없음
```

```cpp
Character* c = new Player();
c->Attack();
```

그다음 `Player::Attack()`에 `override`를 붙여서 다시 빌드했다.

실행 전 내 예측:

> Player의 어택이 아닌 캐릭터의 어택 함수가 호출된다.

## 결과

override 없을 때:

```
Character Attack
```

override 붙였을 때: 컴파일 에러

## 왜 그런가

내 해석:

> const를 붙인 함수는 완전히 다른 함수가 되어 버림. 설령 함수명이 같아도. 아마 내 생각엔 함수가 오버로딩된 게 아닌가 싶음. 그리고 플레이어 어택에 override를 다시 붙이면 에러가 뜬다.

## 정리

> ✏️ 작성 필요: 이 글의 결론을 세 줄로. (처음 답 "명확성을 위해서"와 비교해서 override의 진짜 역할을 한 줄 넣으면 좋아요)

## 더 궁금해진 것

> ✏️ 작성 필요: 새로 궁금해진 점 한 가지. 없으면 "없음"이라고 보내 주세요.

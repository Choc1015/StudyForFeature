# STUDY_LOG

목표: 언리얼 C++ 클라이언트 프로그래머로 취업 (렌더링은 2차 무기)

## 훈련 규칙

- 코드는 AI 없이 직접 쓴다. 참고는 cppreference와 책만.
- AI(Claude)는 코드를 읽고 리뷰와 힌트만 준다. 정답 코드를 받지 않는다.
- 실험이나 단계 하나가 끝나면 커밋한다.
- 해석은 해설을 보지 않고 내 말로 쓴다.

## 진도

| 과제 | 상태 |
|---|---|
| 보충 실험 1: virtual 소멸자 | 완료 (2026-10-18) |
| 보충 실험 2: virtual 함수 호출 | 완료 (2026-10-18), delete 복구 필요 |
| 보충 실험 3: sizeof와 vptr | |
| 보충 실험 4: override | |
| 다시 답하기 Q1~Q3 | |
| MyVector 1단계: int 전용 | |
| MyVector 2단계: 복사 | |
| MyVector 3단계: 템플릿, 이동 | |
| MyVector 4단계: 테스트, 성능 비교 | |

---

## 실험 기록

### 실험 1: virtual 소멸자 (2026-10-18)

- 코드: `Character`/`Player`(virtual 소멸자), `noCharacter`/`noPlayer`(virtual 없음)를 대조군으로 비교
- 결과: virtual이 없으면 `noPlayer` 소멸자가 불리지 않음. 있으면 `Player`, `Character` 순서로 둘 다 불림
- 내 해석: 가상 함수로 만들지 않으니 부모 클래스를 변수 타입으로, 자식 클래스를 담을 때 해당 소멸자를 읽지 못하고 player가 해제되지 못한다.
- 리뷰에서 다듬은 점:
  - virtual이 없으면 컴파일러가 포인터 타입만 보고 **컴파일 시점에** 부를 소멸자를 정한다.
  - 문제는 객체 해제 자체가 아니라 **자식 소멸자가 실행되지 않아** 그 안의 정리 작업이 빠진다는 것.
- 남은 질문: 왜 `Player`가 `Character`보다 먼저 출력될까? 생성자는 어떤 순서일까?
  - 내 답:

### 실험 2: virtual 함수 호출 (2026-10-18)

- 코드: 실험 1의 두 쌍에 `Attack()` 추가. `Character::Attack`만 virtual, `Player::Attack`에 override
- 결과 (virtual 있을 때 / 없을 때): `Player Attack` / `noCharacter Attack`
- 내 해석 (처음): 가상함수로 오버라이딩 했을 때는 character의 attack 함수에 player attack 함수가 덮어졌지만, 하지 않았을 때는 noPlayer attack을 읽어오지 못하고 noCharacter의 attack 함수를 실행했다.
- 내 해석 (리뷰 후): 가상함수로 오버라이딩 했을 때는 가상 포인터가 Player의 가상 테이블을 읽고 Player의 Attack 함수를 대신 실행시킨다. 하지 않았을 때는 다른 객체를 보지 않고 noCharacter의 Attack 함수를 실행했다.
- 리뷰에서 다듬은 점:
  - "덮어졌다"가 아니다. `Character::Attack`은 그대로 남아 있다. 객체 안의 vptr이 Player의 vtable을 가리키고, 실행 중에 그 테이블에서 `Player::Attack`을 찾아 부른다 (동적 바인딩).
  - virtual이 없으면 "읽어오지 못한" 게 아니라, 컴파일러가 포인터 타입(`noCharacter*`)을 보고 컴파일 시점에 정한다 (정적 바인딩). 실험 1과 같은 원리.
  - 버그: `main`에서 `delete c; delete nc;`를 지워서 메모리 누수가 생겼다. → 고침
- 남은 질문 (실행 전에 예측 먼저):
  - `c->Character::Attack();`의 출력은?
  - `noPlayer* np = new noPlayer(); np->Attack();`의 출력은?
  - 내 예측: 1. Character Attack / 2. noPlayer Attack
  - 실행 결과:

### 실험 3: sizeof와 vptr

- sizeof 결과 (virtual 없을 때 / 있을 때):
- 왜 달라졌는지 내 해석:

### 실험 4: override

- override 없을 때 결과:
- override 있을 때 결과:
- 내 해석:

### 다시 답하기 (해설 보지 않고 내 말로)

- Q1. 부모 소멸자를 virtual로 안 만들면?
- Q2. `Character* c = new Player();`에서 `c->Attack()`이 Player 함수를 부르는 이유는?
- Q3. override를 왜 붙이나?

---

## 학습 일지

복사해서 맨 아래에 날짜별로 추가한다.

```
### 날짜:

- 오전 (기초, AI 없이):
- 오후 (만들기):
- 백준:
  - 문제:      결과(혼자 풂 / 힌트 봄 / 못 풂):
  - 문제:      결과(혼자 풂 / 힌트 봄 / 못 풂):
- 오늘 새로 이해한 것 (내 말로 한 줄):
- 틀렸거나 헷갈린 것:
- AI를 쓴 곳과 이유:
- 내일 할 것:
- 컨디션 (1~5):
```

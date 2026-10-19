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
| 보충 실험 2: virtual 함수 호출 | 완료 (2026-10-18) |
| 보충 실험 3: sizeof와 vptr | 완료 (2026-10-18) |
| 보충 실험 4: override | 완료 (2026-10-18) |
| 다시 답하기 Q1~Q3 | 완료 (2026-10-18) |
| MyVector 0단계: 멤버 변수, 생성자, 소멸자 | 완료 (2026-10-18) |
| MyVector 1단계: int 전용 | 완료 (2026-10-19) |
| MyVector 2단계: 복사 | 완료 (2026-10-19) |
| MyVector 3단계: 템플릿, 이동 | 완료 (2026-10-19) |
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
  - 내 예측: 1. Character Attack / 2. noPlayer Attack (둘 다 맞음, 실행 확인은 생략)

### 실험 3: sizeof와 vptr

- 내 예측: noCharacter 1바이트 (객체의 주소에 대한 크기?), Character는 가상 테이블이 존재하기에 8바이트?
- 첫 측정: `sizeof(c)`, `sizeof(nc)` → 둘 다 8
  - 리뷰: `c`, `nc`는 객체가 아니라 **포인터 변수**라서 포인터 크기(x64에서 8)를 잰 것. 측정 대상이 틀렸다. Character의 8이 예측과 맞은 것도 우연.
- 다시 측정: `sizeof(Character)`, `sizeof(noCharacter)`
- sizeof 결과 (virtual 없을 때 / 있을 때): 1 / 8
- 왜 달라졌는지 내 해석: 결국 빈 클래스도 주소 저장을 위한 메모리로 1바이트를 받은 거고, Character는 가상 테이블을 위해 포인터 공간이 필요해서 8바이트인 건가?
- 리뷰에서 다듬은 점:
  - 1바이트는 주소를 "저장"하는 공간이 아니다. 객체마다 서로 다른 주소를 갖게 하려고 자리만 차지하는 1바이트다.
  - 객체 안에 있는 건 vtable 자체가 아니라 vtable을 가리키는 포인터(vptr) 8바이트. vtable은 클래스마다 하나만 있고 모든 객체가 같이 쓴다.
- 남은 질문: Character도 빈 클래스였는데 왜 1 + 8 = 9바이트가 아니라 8바이트일까?
  - 내 답: 주소를 찾을 메모리가 필요했던 건데, Character는 이미 메모리가 있기에 따로 추가되지 않고 해당 가상 포인터에 주소를 가져오는 거지.
  - 리뷰: 앞부분은 정확하다 (vptr로 이미 크기가 0이 아니니 1바이트를 더할 필요가 없다). 뒷부분은 두 주소를 섞었다. 객체의 주소는 vptr 8바이트가 시작하는 위치이고, vptr 안에 든 값은 vtable의 주소다. vptr이 객체의 주소를 가져오는 게 아니다.

### 실험 4: override (2026-10-18)

- 코드: `Character`는 `virtual void Attack() const;`, `Player`는 `virtual void Attack();` (const만 다름)
- 내 예측: Player의 어택이 아닌 캐릭터의 어택 함수가 호출된다.
- override 없을 때 결과: `Character Attack` (예측 맞음)
- override 있을 때 결과: 컴파일 에러
- 내 해석: const를 붙인 함수는 완전히 다른 함수가 되어 버림. 설령 함수명이 같아도. 아마 내 생각엔 함수가 오버로딩된 게 아닌가 싶음. 그리고 플레이어 어택에 override를 다시 붙이면 에러가 뜬다.
- 리뷰에서 다듬은 점:
  - "완전히 다른 함수"는 정확하다. const는 함수 시그니처의 일부라서, 이름이 같아도 const가 다르면 재정의가 아니다.
  - 오버로딩이 아니라 **이름 숨기기(name hiding)**. 오버로딩은 같은 클래스 안에서 이름이 같고 매개변수가 다른 함수들. 자식 클래스에서 같은 이름을 새로 선언하면 부모의 같은 이름 함수가 가려진다.
  - `Player::Attack()`에 `virtual`을 붙였지만 소용없었다. Player의 vtable에 새 칸이 하나 더 생겼을 뿐, `Attack() const` 칸에는 여전히 `Character::Attack`이 들어 있다. 이 실수를 잡아 주는 건 `virtual`이 아니라 `override`뿐이다.

### 다시 답하기 (해설 보지 않고 내 말로, 2026-10-18)

**Q1. 부모 소멸자를 virtual로 안 만들면?**
- 처음 답 (70점): 소멸할 때 자식 클래스의 소멸자가 발생하지 않는 문제가 생긴다.
- 다시 답 (95점): 부모 클래스 타입으로 자식 클래스 객체를 생성할 때 생성되고 나서 해당 변수를 delete하면 자식 클래스의 소멸자를 호출하지 못하는 상황이 발생하여, 자식 생성자에서 할당한 것을 해제하지 못하고 메모리 누수가 생긴다.
- 리뷰: 조건(부모 포인터로 delete)과 결과(누수)를 모두 담았다. 엄밀히는 표준상 정의되지 않은 동작이라는 것까지 알면 완벽.

**Q2. `Character* c = new Player();`에서 `c->Attack()`이 Player 함수를 부르는 이유는?**
- 처음 답 (30점): 자식 클래스의 함수가 복사되기 때문이다.
- 다시 답 (75점): 플레이어 함수가 불린다는 것은 캐릭터의 어택 함수가 가상 함수로 선언되었다는 뜻인데, 가상 함수로 부모 클래스에서 선언하여 자식 클래스에서 재정의하면 이렇게 c에서 불러올 때 가상 함수의 실제 객체를 찾아와서 플레이어의 어택 함수가 실행된다.
- 리뷰: "virtual일 때만"이라는 조건을 먼저 짚은 게 큰 발전. 다만 "어떻게" 실제 객체를 찾는지가 빠졌다. 호출할 때 객체의 vptr을 따라가 vtable에서 함수를 찾는다(동적 바인딩)는 내용이 들어가야 한다. 실험 2, 3에서 직접 확인한 내용.

**Q3. override를 왜 붙이나?**
- 처음 답 (50점): 명확성을 위해서이다.
- 다시 답 (95점): 오버라이딩해야 하는 함수에 오버라이드를 붙이지 않으면 자식 함수를 호출하지 못하게 될 수도 있다. 예를 들어 함수명은 같지만 부모 클래스에는 const가 붙어 있고 자식 클래스에 const가 붙어 있지 않게 되면, 2개의 함수는 오버라이드되지 않고 부모의 함수가 호출이 된다. 만약 override를 붙였더라면 컴파일에서 해당 함수에 대해 에러를 발생시켜 검증시킬 수 있게 된다.
- 리뷰: 실험에서 얻은 구체적인 예까지 들었다. 면접에서 그대로 쓸 수 있는 답. 한 가지만: 문제를 만드는 건 override가 없어서가 아니라 시그니처가 달라서이고, override는 그걸 잡아 주는 장치다. 뒷 문장에서 이미 그렇게 말하고 있다.

### MyVector 1단계 D: operator[] (2026-10-19)

- 문법만 구글링하고 직접 구현. 없는 칸을 요청하면 -1을 돌려주게 함.
- 내 질문: -1 말고 다른 좋은 방법이 있을까?
- 검증 (검사 프로그램):
  ```
  size 4, capacity 4
  v[0..3] = 1, 2, 4, 4
  v[4] (size 4, capacity 4) = -33686019     ← 0xFDFDFDFD, 배열 끝 너머를 읽음
  v[-1] = -1
  size 5, capacity 8
  v[6] (size 5, capacity 8) = -842150451    ← 0xCDCDCDCD, 한 번도 안 쓴 칸
  w.push_back(-1); w[0] = -1, w[5] = -1     ← 진짜 -1과 에러 -1을 구분 못 함
  ```
- 리뷰:
  1. 범위 검사 버그 두 개: capacity가 아니라 size와 비교해야 하고, `>`가 아니라 `>=`여야 한다.
  2. 반환 타입이 `int`라서 `v[1] = 10;`이 안 된다 (복사본을 돌려주기 때문).
  3. -1은 정상적인 값일 수도 있어서 에러 표시로 쓸 수 없다.
- 수정 1차: 조건을 `>= sizeValue`로 고침, 반환 타입을 `int&`로 바꿈, `v[2] = 3;` 동작. assert를 넣었더니 "다 걸려서" 주석 처리.
- 내 질문: assert를 끄고 보니 넘어가는 값이 다 0으로 처리된다. 왜?
  - 확인: `main`이 `v[5]`를 출력하는데 size는 4다. 출력은 `0, 4`.
  - 리뷰: 0으로 "처리"된 게 아니라 배열 밖 메모리를 읽었는데 거기 우연히 0이 있었던 것. `v[4]`는 0xFDFDFDFD였듯, 배열 밖은 무슨 값이든 나올 수 있다 (정의되지 않은 동작). assert가 `v[5]`에서 멈췄다면 그건 assert가 제 일을 한 것.
  - assert 규칙: `assert(조건)`의 조건은 "반드시 참이어야 하는 것"이다. 에러 조건을 넣으면 정상 접근에서 멈춘다.
- 수정 2차: `assert(Invalue < sizeValue && Invalue >= 0);`로 바꾸고 빈 if문 삭제.
  - 내 말: 완벽히 이해했어. assert 조건을 if랑 똑같이 썼었거든.
- 최종 검증 (검사 프로그램):
  ```
  after v[2] = 3: size 5, capacity 8, [1, 2, 3, 4, 5]
  no leaks
  v[3] (size 4) = 4
  v[4]  → Assertion failed: Invalue < sizeValue && Invalue >=0, file ...MyVector.cpp, line 54
  v[-1] → Assertion failed: (같음)
  ```
- **1단계 완료.**

### MyVector 2단계: 복사 (2026-10-19)

- 코드: 복사 생성자 없이 `MyVector a; a.push_back(1); MyVector b = a;`
- 내 예측: 잘 모르겠다. 그냥 값이 옮겨지는 거 아닌가?
- 결과: 복사는 잘 되는데, 해제될 때 `_free_dbg(block, _UNKNOWN_BLOCK);`이 실행되면서 에러가 뜬다.
- 리뷰: `_free_dbg`는 디버그 모드에서 `delete[]`가 실제로 메모리를 돌려줄 때 부르는 함수. 여기서 에러가 났다는 건 "돌려주면 안 되는 메모리를 돌려주려 했다"는 뜻.
- 남은 할 일: 디버거로 `a.data`와 `b.data`의 주소 비교하기, 소멸자가 몇 번 불리는지 세기
  - 내가 찾은 원인: a.data와 b.data는 같은 주소를 내포하고 있어서, delete를 두 번째 할 때는 없는 주소를 해제하려고 해서 에러가 나는 건가?
  - 리뷰: 정확하다. 자동 복사는 포인터 값(주소)만 복사해서 화살표 두 개가 판 하나를 가리킨다 (**얕은 복사**). 소멸자가 두 번 불리면서 같은 판을 두 번 해제한다 (**이중 해제, double free**). 다듬을 점: "없는 주소"가 아니라 "이미 해제된 메모리". 주소 자체는 그대로 있지만 그 메모리는 이미 돌려준 상태다.
- 다음: 복사 생성자를 직접 만들어 판까지 복사하기 (**깊은 복사**)
- 내 질문: 이중 해제를 그냥 하면 되는 거 아니야? 없으면 없구나 하고 넘기면 되잖아.
  - 리뷰: a는 판이 이미 해제됐는지 알 방법이 없고, 진짜 문제는 판을 같이 쓰는 것(복사본을 바꾸면 원본도 바뀜, 댕글링 포인터)이다. "마지막 사람만 해제"하려면 쓰는 사람 수를 세야 하고, 그게 `std::shared_ptr`의 참조 카운팅.
- 복사 생성자 1차: `MyVector(const MyVector& InVector)`, 원본이 비어 있으면 return, 아니면 원본 값을 하나씩 `push_back`
  - main 결과: `1, 100` (원본은 그대로, 복사본만 바뀜)
  - 리뷰: push_back을 재사용한 아이디어는 좋다. 하지만 **멤버 변수를 초기화하지 않았다.** 생성자는 새 객체를 만드는 곳이라 data, sizeValue, capacityValue가 쓰레기 값으로 시작한다. main에서 된 건 우연.
  - 검사 프로그램:
    ```
    [1] stack (a{1,2,3} 복사) → exit code 3 (비정상 종료)
    [2] heap  (new MyVector(a)) → exception: bad array new length
    [3] empty (빈 벡터 복사) → b: data CDCDCDCDCDCDCDCD, size -842150451, capacity -842150451
                              delete b → exit code -1073741819 (접근 위반)
    ```
- 내 질문: const를 꼭 써야 하나?
  - 리뷰: 컴파일은 되지만 붙인다. (1) 원본을 실수로 바꾸면 컴파일러가 막아 준다. (2) const가 없으면 `const MyVector&`로 받은 벡터를 복사할 수 없다. 복사 생성자 매개변수는 사실상 `const MyVector&`로 정해져 있다.
- 복사 생성자 2차: 원본이 비었으면 세 멤버를 nullptr/0/0으로, 아니면 원본 capacity만큼 판을 한 번에 만들고 size, capacity를 맞춘 뒤 for문으로 값 복사 (push_back 재사용 대신 한 번에 할당하는 방식으로 바꿈)
  - 검사 프로그램:
    ```
    [1] a: data ...D550, size 3, capacity 4, [1, 2, 3]
        b: data ...CAB0, size 3, capacity 4, [100, 2, 3]   ← 주소가 다르고, b만 바뀜
        no leaks
    [2] new MyVector(a) → b: size 3, capacity 4, [1, 2, 3]
    [3] 빈 벡터 복사 → b: data 0000000000000000, size 0, capacity 0 → delete b ok
    ```
  - **복사 생성자 완료.** (검사 중 한 번 보인 16바이트 누수는 검사 프로그램 자신의 std::string 때문이었고, 고친 뒤 사라짐)
- 다음: 복사 대입 연산자 (`b = a;`)
- 복사 대입 실험 (대입 연산자 없이): `MyVector d; d.push_back(9); d = a;`
  - 처음 막힌 곳: 예시 그대로 `MyVector b;`를 써서 변수 이름 `b`가 겹쳐 재정의 에러 → 이름을 `d`로 바꿈
  - 내 예측: 일단 d.data는 a.data 판을 가리키게 되고, 원래 판은 가리키는 값이 없어지면서 메모리 누수가 생기는 거 같아.
  - 결과: 아까와 똑같은 에러가 났다 (이중 해제).
  - 리뷰: 문제 두 개를 모두 맞혔다. (1) d의 원래 판 `[9]`는 누수 (1단계 판 늘리기 누수와 같은 원리) (2) d와 a가 판 하나를 같이 써서 이중 해제 (복사 생성자 없을 때와 같은 원리).
- 복사 대입 연산자 1차: `MyVector& operator=(MyVector InVector)`
  - 내 설명: 메모리 누수는 원래 data가 있으면 delete로 없애고 다시 new로 받는 걸로 했고, 이중 해제 문제는 = 오퍼레이터를 만들어서 MyVector끼리 데이터를 주고받을 때 안에서 일어나는 걸 로직으로 표현했어.
  - 같이 바뀐 것: 복사 생성자에도 `if (data != nullptr) delete[] data;`를 추가함
  - 검사 프로그램:
    ```
    [copy]   b = a로 생성          → exit code -1073741819 (접근 위반)
    [assign] d{9} = a{1,2,3}       → exit code -1073741819
    [self]   a = a                 → exit code -1073741819
    [chain]  x = y = z             → exit code -1073741819
    [empty]  d{9} = 빈 a           → d: size 0, capacity 0, no leaks
    ```
  - 리뷰:
    1. 복사 생성자에 넣은 `delete[] data`가 모든 크래시의 원인. 생성자 안에서 data는 아직 쓰레기 값이라 nullptr 검사를 통과하고, 쓰레기 주소를 해제한다. 대입 연산자의 매개변수가 값으로 받기라서 대입할 때도 복사 생성자가 불려 같이 터진다.
    2. `return InVector;` → 함수가 끝나면 사라지는 매개변수의 참조를 돌려준다 (댕글링 참조). 돌려줘야 하는 건 나 자신.
    3. 매개변수를 `MyVector`(값)로 받아서 대입할 때마다 복사가 한 번 더 일어난다. 보통은 `const MyVector&`.
    4. 자기 대입 확인이 없다 (값으로 받으면 우연히 안전해지지만, `const MyVector&`로 바꾸면 필요).
- 복사 대입 연산자 2차: 복사 생성자의 `delete[]` 삭제, 매개변수 `const MyVector&`, 맨 앞에 `if (this == &InVector) return *this;`, 끝에 `return *this;`
  - 검사 프로그램:
    ```
    [copy]   b: 주소 다름, [1, 2, 3], no leaks
    [assign] d: 주소 다름, [100, 2, 3] (a는 그대로), no leaks
    [self]   a = a 후에도 [1, 2, 3], no leaks
    [chain]  x = y = z → x [100, 4], y [3, 4], z [3, 4], 셋 다 주소 다름, no leaks
    [empty]  d = 빈 a → size 0, capacity 0, no leaks
    [emptypush] d = 빈 a 후 d.push_back(7) → Detected memory leaks! 0 bytes long
    ```
  - 리뷰: 네 가지 모두 정확히 고쳤다. 남은 건 경계 상황 하나. 빈 벡터를 대입받으면 `new int[0]`으로 0칸짜리 판을 만들어 data가 nullptr이 아니게 되고, 다음 push_back은 capacity가 0이라 그 판을 해제하지 않은 채 새 판을 만든다.
- 내 질문: 복사 생성자에 capacity가 0이라면으로 조건을 고치면 되지 않나? / 복사 생성자 쪽에서 delete 해 줘야 되나?
  - 리뷰: 누수는 대입 연산자에서 생긴다. 복사 생성자는 갓 태어난 객체라 해제할 판이 없으므로 delete하면 안 된다. 다만 `capacityValue == 0` 조건은 더 튼튼해서 써도 좋다.
- 복사 대입 연산자 3차: 복사 생성자 조건을 `InVector.capacityValue == 0`으로 바꿈. 대입 연산자의 해제 조건을 `if (data != nullptr)`에서 `if (InVector.capacityValue == 0)`으로 바꿈
  - 검사 프로그램:
    ```
    [assign]    Detected memory leaks! 4 bytes, Data: 09 00 00 00          ← d의 원래 판 [9]
    [chain]     Detected memory leaks! 4 bytes (02), 4 bytes (01)         ← y의 [2], x의 [1]
    [emptypush] Detected memory leaks! 0 bytes                             ← 그대로
    (copy, self, empty는 통과)
    ```
  - 리뷰: 해제 조건이 "원본이 비었을 때만"이 되어서, 원본에 값이 있는 보통의 대입에서 원래 판을 해제하지 않는다. 원래 판 해제는 원본이 비었든 아니든 항상 해야 한다. 원본이 비었을 때 할 일은 해제가 아니라 `new int[0]` 대신 nullptr/0/0으로 만드는 것.
- 복사 대입 연산자 4차: `delete[] data;`를 조건 없이 함수 맨 앞에 둠. 원본이 비었으면 nullptr/0/0, 아니면 새 판을 만들어 복사
  - 검사 프로그램:
    ```
    [copy] [assign] [chain] [empty] [emptypush] → 모두 no leaks
    [self] a = a → a: [-572662307, -572662307, -572662307]
                   Assertion failed: _CrtIsValidHeapPointer(block) → exit code -1073741819
    ```
  - 리뷰: 누수는 전부 사라졌다. 남은 건 순서 하나. `delete[]`가 자기 대입 확인보다 앞에 있어서, `a = a`일 때 자기 판을 해제한 뒤 그대로 return한다. -572662307 = 0xDDDDDDDD는 디버그 힙이 "해제된 메모리"에 채우는 값. 그 뒤 소멸자가 같은 판을 또 해제해서 크래시.
- 복사 대입 연산자 5차: `delete[] data;`를 자기 대입 확인 아래로 옮김
  - 검사 프로그램: copy, assign, self, chain, empty, emptypush 여섯 개 모두 정상 동작, no leaks, exit code 0. main도 정상 종료.
  - **2단계 완료.** 복사 생성자, 복사 대입 연산자, 소멸자를 모두 직접 만든 것 = **3의 법칙(Rule of Three)**: 셋 중 하나를 직접 만들어야 하는 클래스는 보통 셋 다 필요하다.

### MyVector 3단계: 템플릿 (2026-10-19, 진행 중)

- 1차: `template <typename T>`를 붙이고 int를 T로 바꿈. main은 `MyVector<int>`로 바꿈.
  - 내 질문: 이렇게 쓰는 게 아닌가?
  - 빌드 결과: `error LNK2019: MyVector<int>::MyVector<int>(void) ... main 함수에서 참조되는 확인할 수 없는 외부 기호` (5개)
  - 리뷰:
    1. 링크 에러: 템플릿은 설계도라서, 실제 `MyVector<int>` 코드는 쓰는 곳(main.cpp)에서 함수 정의를 보고 만들어진다. 정의가 MyVector.cpp에만 있으면 main.cpp는 볼 수 없다.
    2. 너무 많이 바꿨다: `sizeValue`, `capacityValue`, `size()`, `capacity()`, `operator[]`의 번호 매개변수까지 T가 됨. 개수와 번호는 담는 타입과 상관없이 정수다.
    3. 덜 바꿨다: push_back과 대입 연산자 안의 `new int[...]` 세 곳이 그대로.
- 내 질문: 헤더에 다 넣으면 분리하는 이유가 사라지는데?
  - 리뷰: 맞다. 템플릿은 쓰는 곳에서 본문을 봐야 해서 분리의 장점(한 번만 컴파일, 구현 숨기기)이 사라진다. 선택지는 헤더에 전부 / .inl 파일을 헤더 끝에서 include / .cpp에서 쓸 타입을 미리 지정. 보통은 헤더에 전부 (std::vector, TArray).
- 2차: 헤더에 전부 옮김 (MyVector.cpp는 비움). `size()`, `capacity()` 반환 타입은 int로 돌려놓음. `MyVector<string>`에 "lemon"을 넣어 보려 함.
  - 빌드 결과:
    ```
    MyVector.h(87): error C2676: 이항 '>=': 'T'이(가) 이 연산자를 정의하지 않거나 ... with [ T=std::string ]
    MyVector.h(90): error C2677: 이항 '[': 'T' 형식을 사용하는 전역 연산자가 없거나 ... with [ T=std::string ]
    ```
  - 리뷰: 1차 리뷰의 2번, 3번이 아직 남아 있다. operator[]의 번호 매개변수가 T라서 "lemon" 벡터는 번호도 string이 된다. 이걸 고치면 sizeValue, capacityValue(T)와 new int 세 곳에서 다음 에러가 나올 것.
- (여기서부터 한 번에 하나씩 고치기로 함)
- 3차: operator[] 번호를 int로 → 이어서 return을 `*T`로 바꿨다가 되돌림 (T는 타입 이름이라 변수처럼 쓸 수 없음) → assert 오타 `0 = <`, `Invale` 수정 → push_back의 `new int`, `int* temp`를 T로, sizeValue/capacityValue를 int로
  - 결과: `MyVector<string>`에 "lemon"을 넣고 `ca[0]` 출력 → `lemon`. 빌드 경고 없음, 정상 종료.
  - 남은 것: 대입 연산자 안의 `new int[...]` 한 곳. string 벡터끼리 대입을 아직 안 써서 에러가 안 날 뿐이다 (템플릿 함수는 실제로 쓰일 때만 만들어진다).
- 4차: 대입 연산자의 `new int`를 `new T`로. main에 `cb = ca;` 추가 → `lemon` 출력.
  - 검사 프로그램 (int, float, string 각각 push, 복사, 대입, 자기 대입):
    ```
    [int]    a [1, 2, 4, 4, 5], b 복사본 같음, d만 [100, ...], e = 빈 벡터 후 push → [7], no leaks
    [float]  f [1.5, 2.5, 3.5], no leaks
    [string] s 5개 (긴 문자열 포함, 1→2→4→8로 늘어남), t 복사본 같음, u만 [changed, ...], no leaks
    ```
  - **템플릿 완료.** 남은 3단계 과제: 이동.

### MyVector 3단계: 이동 (2026-10-19)

- 질문: 100만 개짜리 a를 다시 안 쓸 거라면, 판을 복사하지 않고 b에게 넘기려면?
  - 내 답 1: 참조로 주면 되는 거 아니야? 대신 a를 바꾸면 b도 바뀌겠지? 비유하자면 계란판을 둘이서 잡고 있는 느낌?
    - 리뷰: 둘이 같이 잡으면 얕은 복사와 같아서 이중 해제. 이동은 "건네주고 손 놓기".
  - 내 답 2: 그냥 delete를 넘겨줬을 때 하면 되는 거 아닌가?
    - 리뷰: delete는 판 자체를 없앤다. 판은 이제 b의 것이라 없애면 안 되고, a는 판의 주소만 잊으면 된다.
  - 내 답 3: nullptr? → 정확. sizeValue, capacityValue는 0. 빈 벡터 상태.
- 이동 생성자 `MyVector(MyVector&& InVector)`: InVector의 세 멤버를 그대로 받고, InVector를 nullptr/0/0으로. (생성자 이름에서 `<T>`도 정리함)
  - main: `MyVector<string> m2 = std::move(m1);` → `melon, 0`
  - 검사 프로그램:
    ```
    [move] same tray handed over: yes        ← 판을 새로 만들지 않고 그대로 넘김
           m1: size 0, capacity 0, []
           m2: size 3, capacity 4, [melon, kiwi, plum]
           빈 벡터 이동 → size 0, capacity 0
           이동한 뒤의 m1에 다시 push_back → [reuse]
           no leaks
    ```
  - **이동 생성자 완료.**
- 이동 대입 연산자 `MyVector& operator=(MyVector&& InVector) noexcept`
  - 내 생각: 복사 대입 때 이중 해제랑 메모리 누수가 있었지. 여기도 마찬가지로 옮기고 이번에 delete 해 주면 될 거 같은데.
  - 1차: `data = InVector.data;` 다음에 `delete[] InVector.data;` → 받은 판을 바로 지움. 지울 건 내 원래 판.
  - 2차: delete 줄을 지우고 상대 비우기 3줄 추가. main은 `melon, 0`으로 맞게 나옴.
    - 내 이해: 주소를 data에게 넘겨주고, InVector.data가 기억하고 있는 주소 공간은 비우는 거지?
    - 검사 프로그램:
      ```
      m3{grape} = move(m2{melon, kiwi, plum}) → m3: size 1, capacity 1, [melon]   ← kiwi, plum이 안 보임
      Detected memory leaks! (grape 판, 빈 벡터를 받을 때 버린 판)
      ```
    - 리뷰: (1) "주소 공간을 비운다"가 아니라 "주소를 잊는다". 메모리는 그대로 m3가 쓴다. (2) data만 받고 sizeValue, capacityValue를 안 받았다. main은 melon 하나라 우연히 맞아 보였다. (3) 내 원래 판 delete[]가 아직 없다.
  - 3차: sizeValue, capacityValue를 받게 함. 그런데 if/else에 `new T[...]`를 넣어 바로 덮어씀 (1단계 temp 누수와 같은 모양) → 값은 맞지만 누수.
    - 내 이해: 만들기 전에 해제하고 null 초기화한 다음에 넣는다? → "만들기"는 없고, nullptr로 만드는 건 내 쪽이 아니라 넘겨준 쪽.
  - 4차: if/else를 지우고 `delete[] data;` 한 줄 → 받기 3줄 → 상대 비우기 3줄 → `return *this;`
    - 검사 프로그램: int, float, string, move, moveassign 다섯 개 모두 정상, no leaks. main `melon1, melon2, melon3, 0`.
  - **이동 대입 완료. 3단계 완료.** 소멸자, 복사 생성자, 복사 대입, 이동 생성자, 이동 대입을 모두 직접 만들었다 = **5의 법칙(Rule of Five)**.

### MyVector 4단계: 성능 비교 (2026-10-19, 진행 중)

- 질문: `std::vector<int>`와 `MyVector<int>`에 각각 push_back을 100만 번 하면 어느 쪽이 빠를까?
  - 내 예측: 똑같지 않을까? 일부러 std의 원리대로 만들었는데, 그래도 빠르다면 STL의 vector가 빠르겠지. 잘 설계된 거니.
  - 실행 결과 (내 측정): 내가 만든 vector가 30ms 빠르다. 왜지?
  - Claude 재측정 (각 3회, MyVector vs std::vector):
    ```
    Debug   : 4 ms vs 34 ms
    Release : 1 ms vs 2 ms
    ```
  - 리뷰: Debug 빌드의 std::vector는 범위 검사, 반복자 검사 같은 디버그용 검사가 잔뜩 켜져 있고 함수도 펼쳐지지(인라인) 않는다. MyVector는 그런 검사가 거의 없다. 성능은 Release로 재야 의미가 있다. Release에서도 ms 단위는 너무 거칠어서 마이크로초로 다시 재 볼 것.

### MyVector 설계 결정

**처음 capacity: 0으로 시작할까, 미리 몇 칸 잡을까? (2026-10-18)**
- 내 결정: 미리 몇 칸을 잡아 둔다.
- 내 이유: 어차피 데이터를 넣는데 처음부터 이사할 비용을 만들 필요가 없잖아? 그렇다고 너무 크게 메모리를 잡지 말고 적당히 잡아 놓고 그게 찰 때만 이사하는 거지.
- 리뷰: 이유가 있는 좋은 결정. 다만 "어차피 넣는다"는 가정이다. 실제 `std::vector`(MSVC, GCC)는 0으로 시작한다. 끝까지 비어 있는 벡터가 많고, `new` 자체가 비싸서 안 쓸 메모리를 미리 잡는 비용이 더 클 수 있기 때문. 4단계 성능 비교에서 직접 확인해 볼 것.
- 변경 (구현하면서): vector가 0으로 시작한다고 해서 0으로 맞췄다.
  - 리뷰: "vector가 그래서"는 내 이유가 아니다. 0을 고른다면 이유는 "안 쓸 메모리를 미리 잡지 않으려고"여야 한다. 그런데 `new int[0]`은 0칸이어도 할당을 한다. 그 이유를 지키려면 `new`를 하지 말고 `data = nullptr;`로 둬야 한다.

### MyVector 0단계: 멤버 변수, 생성자, 소멸자 (2026-10-18)

- 처음에 막힌 곳: `data`의 타입을 `MyVector*`로 씀 → 판에 담기는 건 int이므로 `int*`
- 내 이해: int의 포인터를 만들어서 int 배열의 주소값을, 배열의 공간이 커질 때마다 새로운 주소를 받아서 다른 메모리 장소로 이동하는 거지.
  - 리뷰: 정확하다. 이게 vector의 핵심 구조.
- 버그: 소멸자에서 `delete data;` → `new[]`로 만든 건 `delete[]`로 해제해야 한다. 짝이 안 맞으면 정의되지 않은 동작. → 고침
- 최종 결정: 불필요한 메모리를 잡지 않기 위해, 쓰지 않는 건 미리 할당하지 않겠다. → `data = nullptr;`
  - 리뷰: 이번엔 내 이유로 고른 결정이고, 코드도 그 이유와 맞다. `nullptr`에 `delete[]`를 해도 아무 일도 일어나지 않아서 안전하다.
- 실행 확인: `main`에 `MyVector* v;`를 씀 → 빌드는 되지만 경고 C4101("참조되지 않은 지역 변수"). 포인터 변수만 만들었을 뿐 MyVector 객체는 만들어지지 않았다. 생성자도 소멸자도 실행되지 않음.
  - 고침: `MyVector v;`로 바꿈. 경고 없이 빌드되고 정상 종료.
- 확인 질문: `MyVector* v = new MyVector();`로 만들면 직접 무엇을 해 줘야 할까?
  - 내 답: 이 경우 직접 포인터를 해제해 줘야 하지. 그러니까 그냥 클래스로 선언할게.
  - 리뷰: 맞다 (`delete v;`). 정확히는 포인터가 아니라 포인터가 가리키는 **객체**를 해제하는 것. 그리고 "클래스로 선언"이 아니라 "객체를 직접(스택에) 만든다". 이렇게 만든 객체는 함수가 끝날 때 소멸자가 자동으로 불린다.

### MyVector 1단계 A: size(), capacity() (2026-10-18)

- 코드: 멤버 변수를 `sizeValue`, `capacityValue`로 바꾸고, `inline int size() const`, `inline int capacity() const`를 헤더에 정의
- 결과: `0, 0` 출력, 경고 없이 빌드
- 내 설명 (inline): 한 줄밖에 안 되는 함수는 이렇게 쓰면 그 함수를 가져와서 쓰는 게 아니라 본문에서 생성해서 써서 효율이 좋다고 해서 썼어.
- 리뷰:
  - 좋은 점: 읽기만 하는 함수에 `const`를 붙였다 (실험 4 적용). 이름 충돌을 멤버 변수 이름을 바꿔서 해결했다.
  - inline 설명은 반만 맞다. 함수 호출을 본문 코드로 바꿔 넣는 것(인라이닝)은 맞다. 하지만
    1. 클래스 안에서 바로 정의한 함수는 이미 자동으로 inline이다. 키워드를 붙여도 달라지는 게 없다.
    2. `inline` 키워드는 인라이닝을 강제하지 못한다. 실제로 펼칠지는 컴파일러가 스스로 정한다 (Debug 빌드에서는 보통 안 펼친다).
    3. 지금 `inline`의 주된 의미는 "헤더에 함수 정의를 둬서 여러 .cpp에서 include해도 링크 에러가 안 나게 한다"이다.
  - "효율이 좋다고 해서"는 들은 이야기다. 실험처럼 출처를 확인하는 습관을 들일 것.
  - 사소한 것: `{ return sizeValue; };`의 마지막 `;`는 필요 없다.

### MyVector 1단계 B, C: push_back과 판 늘리기 (2026-10-19)

- B만 하라고 했는데 C(2배로 늘리기)까지 직접 구현. capacity가 0일 때(0의 2배는 0) 문제를 따로 분기해서 해결했다.
- 첫 버그: 값이 안 들어감
  - 처음 진단: "temp에서 첫 번째 값만 기억하고 있다" → 디버거가 `int*`를 첫 칸만 보여 줘서 생긴 착각. 조사식에 `temp,2`처럼 쓰면 배열로 펼쳐 볼 수 있다.
  - 다시 찾은 패턴: 1, (2), 4, (4) 중 괄호 친 값만 빠진다.
  - 원인: 빠진 호출은 모두 "판 늘리기" 분기로 들어갔고, 그 분기 끝의 `return;` 때문에 새 값을 넣는 줄까지 못 내려감. → `return;` 지우고 `delete[] temp;` 추가
- 검증 (별도 검사 프로그램, 디버그 힙 누수 검사):
  ```
  push_back(1) -> size 1, capacity 1, data [1]
  push_back(2) -> size 2, capacity 2, data [1, 2]
  push_back(4) -> size 3, capacity 4, data [1, 2, 4]
  push_back(4) -> size 4, capacity 4, data [1, 2, 4, 4]
  push_back(5) -> size 5, capacity 8, data [1, 2, 4, 4, 5]
  Detected memory leaks! 4 bytes, 8 bytes, 16 bytes (값은 전부 CD = 한 번도 안 쓴 메모리)
  ```
- 남은 버그: 판을 늘릴 때마다 누수. 4, 8, 16바이트는 정확히 int 1칸, 2칸, 4칸 = 판을 늘리던 순간의 옛날 capacity 크기. 원인은 판 늘리기 분기 안에 있다. (직접 찾기)
  - 고침: `int* temp = new int[capacityValue]; temp = data;` → `int* temp = data;`
  - 재검증: 값은 그대로 `[1, 2, 4, 4, 5]`, 누수 검사 `no leaks`
  - 내가 찾은 원인 (내 말로): 포인터에 다른 주소를 넣으니 원래 주소는 그 주소를 찾을 수 있는 포인터는 사라지고, delete[] temp를 해도 data를 받은 새로운 주소가 사라지는 거고, 원래 빈 new temp는 계속 남아 있는 거지.
  - 이해 확인: 새 판은 data에서 큰 판으로 만들 때 생기는 거고, temp에 저장되는 건 원래 옛날 판이 맞다. → 정확
  - 리뷰: 핵심은 정확하다. 화살표(포인터)를 다른 곳으로 돌리면 원래 메모리는 해제되지 않고 아무도 못 찾게 된다. 한 곳만 다듬자면, `delete[] temp`가 지우는 건 "새로운 주소"가 아니라 판을 늘리기 전의 **옛날 판**이다. `temp = data;` 시점의 data는 아직 옛날 판을 가리키고 있었기 때문. 그래서 그 delete[]는 원래 의도대로 맞게 동작했고, 문제는 오직 첫 줄에서 만든 빈 배열이었다.

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

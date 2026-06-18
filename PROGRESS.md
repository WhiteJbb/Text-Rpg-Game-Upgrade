# 리팩토링 진행 문서 (PROGRESS)

> 이 문서는 **다른 컴퓨터에서도 작업을 이어가기 위한 인수인계용 기록**입니다.
> 작업이 끝날 때마다 갱신합니다. 게임 자체 설명은 [README.md](README.md), 작업 지침은 [CLAUDE.md](CLAUDE.md)를 참고하세요.

- **마지막 업데이트**: 2026-06-18
- **작업 브랜치**: `refactor` (기준: `main`)
- **현재 단계**: 1단계 완료 → 다음은 2단계(저장/불러오기 일원화 + `auto_save` 버그 수정)

---

## 1. 프로젝트 한눈에 보기

- 고등학교 때 C로 만든 콘솔 Text RPG를 점진적으로 리팩토링하는 프로젝트.
- 핵심 소스: [game/code.c](game/code.c) — **단일 파일**(현재 약 1360줄).
- 데이터/저장 파일: [game/test/](game/test/)
  - `data.txt`, `stat.txt` — 저장 파일(플레이 중 갱신, 없으면 자동 생성)
  - `monsters.txt`, `magicspell.txt` — 게임 콘텐츠(읽기 전용)
- Windows 전용(`windows.h`, `system("cls"/"pause")`, `Sleep`).

## 2. 빌드 & 실행 (중요)

데이터 파일 경로가 `C:\test\` 절대경로 → **`test/` 상대경로**로 바뀌었습니다.
따라서 **반드시 `game/` 폴더에서** 빌드·실행해야 `test/` 하위 파일을 찾습니다.

```bash
cd game
# GCC(MinGW)
gcc -std=c11 -Wall -Wextra -o game.exe code.c
./game.exe
```

Visual Studio를 쓰면 `game/code.c`를 빌드하되, **작업 디렉터리를 `game/`로** 맞춰야 합니다.

> ⚠️ 현재 작업 PC에는 컴파일러(gcc/clang/MSVC)가 없어, 코드 변경 후 **빌드 검증은 사용자 환경에서** 해야 합니다.
> 0~1단계 코드는 아직 컴파일 검증 전입니다(사용자가 나중에 한 번에 검증 예정).

## 3. 리팩토링 로드맵 & 상태

| 단계 | 주제 | 상태 |
|---|---|---|
| **0** | 빌드·실행 확보 (경로 상대화 + `fopen` NULL 체크) | ✅ 완료 |
| **1** | 제어 흐름 정상화 (재귀 `main()` 호출 제거) | ✅ 완료 |
| **2** | 저장/불러오기 일원화 + `auto_save` 버그 수정 + `feof` 루프 수정 | ⬜ 예정 (다음) |
| **3** | 입력 처리 공통화 (`read_int()` 등으로 `scanf` 방어) | ⬜ |
| **4** | 중복 제거(포션 로직)·매직넘버 상수화·미사용 자산 정리 | ⬜ |
| **5** | 파일 분리 (main/player/monster/battle/item/shop/save/utils + Makefile) | ⬜ |
| **6** | 게임성·사용성 (crit/speed/magic 전투 반영, 던전·마법 확장, UI) | ⬜ |
| **7** | 포트폴리오화 (README Before/After, 리팩토링 기록) | ⬜ |

## 4. 0단계 — 완료 내용

**주제: 파일 경로 이식성 + 파일 입출력 안전성**

- `C:\test\...` 하드코딩 12곳 → 상대경로 매크로(`DATA_PATH`/`STAT_PATH`/`MONSTERS_PATH`/`MAGIC_PATH`)
- 파일 열기 헬퍼 `open_or_warn()` 추가(실패 시 경고 + `NULL`)
- 모든 `fopen`에 NULL 처리(저장본 없음→첫 실행, 콘텐츠 없음→`exit(1)`, 저장 실패→`return`)
- **변경 파일**: [game/code.c](game/code.c), [README.md](README.md)

## 5. 1단계 — 완료 내용 (이번 작업)

**주제: 제어 흐름 정상화 — 재귀 `main()`/`adventure()`/`fight()` 호출 제거**

기존에는 "메뉴로 돌아갈 때" 함수가 `main()` 등을 다시 호출해 콜스택이 무한히 쌓이는 구조였음.
이를 **반환값(신호) 전파** 방식으로 전환하여, 단일 `main` while 루프가 흐름을 제어하도록 함.

- 신호 상수 추가: `COMBAT_CONTINUE`(탐험 계속) / `COMBAT_TO_MENU`(메뉴 복귀) / `COMBAT_ONGOING`(전투 중)
- 전투 체인 시그니처 변경(불필요해진 `reg`/`region` 제거):
  - `int monster(int num)`, `int fight(int num)`, `int attack(int num)`, `int hp_out(int num)`, `int damage(int dam,int num)`, `void fight_pouse(int num)`
- 재귀 호출 → 반환/복귀로 교체한 지점:
  - `damage` 사망 처리: `main()` → `return 1`(사망 신호) ; 사망 페널티·메시지는 그대로 유지
  - `hp_out` 처치 처리: `adventure(reg)` → `return 1`(처치 신호) ; 보상·리셋 그대로
  - `attack`: 처치 시 반격 생략하고 `COMBAT_CONTINUE`, 사망 시 `COMBAT_TO_MENU`, 그 외 `COMBAT_ONGOING`
  - `fight`: `attack` 결과가 처치/사망이면 그 신호를 반환, 아니면 루프 지속
  - `monster`: 싸운다→`fight` 결과 반환, 도망 성공→`COMBAT_TO_MENU`, 실패→`fight`
  - `adventure`: `monster`가 `COMBAT_TO_MENU`면 `return`, 처치면 다음 몬스터 루프
  - 메뉴 계열의 `main()` 재귀 → `return`: `shop_choose`/`potion_shop`/`slot_machine`(3곳)/`item_show`(`menu()` 제거)
  - 포션 메뉴 돌아가기: `potion_use`(→`adventure_control` 대신 `return`), `fight_pouse`(→`fight` 대신 `return`)
- 부수 정리: `damage`의 미사용 `hurt` 변수 제거, 미구현 지역(2~4) 선택 시 무한 출력 대신 안내 후 메뉴 복귀

### 동작 변화(전투 수치·메시지는 동일, 흐름만 변경)
- 물약상점 "돌아가기" → 메인 메뉴가 아니라 **상점 선택 화면**(한 단계 위)으로 복귀.
- 조우 중 "아이템사용"으로 회복 후 → 지역 선택이 아니라 **같은 몬스터 조우 프롬프트**로 복귀.
- 늑대의 굴/엘프의 숲/뱀파이어의 성(미구현) 선택 시 → 멈춤 없이 "아직 갈 수 없는 지역" 안내 후 메뉴 복귀.

### 검증
- 재귀 `main()` 호출 0개(정의만 존재), 옛 시그니처 호출 0개, 중괄호/괄호 균형(137/137, 838/838) 확인.
- ⚠️ 컴파일 검증은 미실시(컴파일러 부재). 사용자 환경 빌드 시 확인 필요.

### 1단계 확인 방법(사용자 환경)
1. 탐험 → 전투 → **처치** 시 다음 몬스터가 계속 나오는지(탐험 지속).
2. 전투 중 **사망** 시 페널티 적용 후 메인 메뉴로 정상 복귀하는지.
3. 도망 성공 시 메뉴 복귀, 상점/슬롯에서 돌아가기 시 정상 복귀하는지.
4. 오래 플레이해도 메모리/스택이 계속 늘지 않는지(기존 재귀 누적 해소).

## 6. 남은 알려진 문제 (다음 단계 후보, 우선순위순)

1. **🔴 `auto_save()` 저장 포맷 불일치** — `mp`/`maxmp` 누락(7필드)인데 로더는 9필드+포션 6 기대 → 사냥 후 저장본을 다음 실행에서 잘못 로드. **2단계 핵심.**
2. **🟠 저장 로직 4중 중복** (`save`/`inside_save`/`auto_save`/`clear`) → 단일 함수로 통합(중복이 위 버그의 원인).
3. **🟠 `while(!feof(fp))` 루프** — `mob_load`/`magic_load`가 EOF 뒤 쓰레기 1개 더 읽음 + `free` 없음(누수).
4. **🟡 입력 검증 부재** — `menu()` 외 다수 `scanf` 미검증 → 문자 입력 시 무한루프.
5. **🟡 포션 로직 중복** — `potion_use` ≈ `fight_pouse` (~120줄 거의 동일).
6. **🟢 미사용 자산** — `speed`/`magic`/`crit`/`mp`가 전투 미반영, `user.item`·`cheat()`·`magic()`/`magic_shop()` 죽은 코드.
7. **🟢 매직넘버 산재** — 회복량 20/100/500, 가격 100/500/1000, 스폰확률 등.
8. **🟢 인코딩 불일치** — `code.c`/`magicspell.txt`=UTF-8 인데 `monsters.txt`=CP949 → 몬스터 이름 깨질 위험.
9. **🟢 Windows 종속** — 크로스플랫폼 시 추상화 필요(우선순위 낮음, C 유지).

## 7. 다음 단계 제안

**2단계: 저장/불러오기 일원화 + 버그 수정.** `save`/`inside_save`/`auto_save`/`clear`의 중복 파일쓰기를 단일 `save_game()`으로 통합하고, 그 과정에서 `auto_save`의 필드 누락 버그를 제거한다. 이어서 `feof` 루프와 메모리 누수도 정리한다.

---

### 참고: 작업 규칙 (CLAUDE.md 요약)
- 한 번에 한 주제만, 작은 단위로. 기존 감성/흐름 보존. C 언어 유지.
- 변경 전 현재 구조 파악 → 문제 설명 → 작은 수정 → 변경 파일 안내 → 실행/테스트 방법 안내.
- **이 PROGRESS.md는 각 작업 종료 시 갱신한다.**

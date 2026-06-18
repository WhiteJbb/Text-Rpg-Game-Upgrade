# 리팩토링 진행 문서 (PROGRESS)

> 이 문서는 **다른 컴퓨터에서도 작업을 이어가기 위한 인수인계용 기록**입니다.
> 작업이 끝날 때마다 갱신합니다. 게임 자체 설명은 [README.md](README.md), 작업 지침은 [CLAUDE.md](CLAUDE.md)를 참고하세요.

- **마지막 업데이트**: 2026-06-18
- **작업 브랜치**: `refactor` (기준: `main`)
- **현재 단계**: 0단계 완료 → 다음은 1단계(제어 흐름 정상화)

---

## 1. 프로젝트 한눈에 보기

- 고등학교 때 C로 만든 콘솔 Text RPG를 점진적으로 리팩토링하는 프로젝트.
- 핵심 소스: [game/code.c](game/code.c) — **단일 파일**(현재 약 1350줄).
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

> ⚠️ 현재 작업 PC에는 컴파일러(gcc/clang/MSVC)가 설치되어 있지 않아, 코드 변경 후 **빌드 검증은 사용자 환경에서** 해야 합니다.

## 3. 리팩토링 로드맵 & 상태

| 단계 | 주제 | 상태 |
|---|---|---|
| **0** | 빌드·실행 확보 (경로 상대화 + `fopen` NULL 체크) | ✅ 완료 |
| **1** | 제어 흐름 정상화 (재귀 `main()` 호출 제거) | ⬜ 예정 (다음) |
| **2** | 저장/불러오기 일원화 + `auto_save` 버그 수정 + `feof` 루프 수정 | ⬜ |
| **3** | 입력 처리 공통화 (`read_int()` 등으로 `scanf` 방어) | ⬜ |
| **4** | 중복 제거(포션 로직)·매직넘버 상수화·미사용 자산 정리 | ⬜ |
| **5** | 파일 분리 (main/player/monster/battle/item/shop/save/utils + Makefile) | ⬜ |
| **6** | 게임성·사용성 (crit/speed/magic 전투 반영, 던전·마법 확장, UI) | ⬜ |
| **7** | 포트폴리오화 (README Before/After, 리팩토링 기록) | ⬜ |

## 4. 0단계 — 완료 내용 (이번 작업)

**주제: 파일 경로 이식성 + 파일 입출력 안전성** (게임 로직 동작은 그대로 유지)

- `C:\test\...` 하드코딩 12곳 → 상대경로 매크로로 치환
  - `DATA_PATH` `STAT_PATH` `MONSTERS_PATH` `MAGIC_PATH` ([game/code.c](game/code.c) 상단 `#define`)
- 파일 열기 헬퍼 `open_or_warn()` 추가 — 실패 시 표준 경고 출력 후 `NULL` 반환
- 모든 `fopen`(읽기 4 + 쓰기 8 = 12곳)에 NULL 처리 추가
  - `user_load`: 저장 파일 없으면 → 첫 실행으로 보고 `new_user()` 후 종료 (이전엔 크래시)
  - `stat_load`: 없으면 기본값 유지
  - `mob_load` / `magic_load`: 콘텐츠 파일 없으면 메시지 출력 후 `exit(1)`
  - 저장 함수(`save`/`inside_save`/`auto_save`/`clear`): 열기 실패 시 크래시 대신 `return`
- **변경 파일**: [game/code.c](game/code.c), [README.md](README.md)(설치·실행 안내 갱신)
- 검증: 중괄호/괄호 균형 OK(136/136, 840/840). 단, **컴파일 검증은 미실시**(컴파일러 부재).

### 0단계 확인 방법(사용자 환경)
1. `cd game` 후 위 빌드 명령으로 컴파일 → 경고/오류 확인.
2. `game/` 에서 실행 → 기존처럼 메뉴 진입/탐험/저장 동작 확인.
3. `test/data.txt`를 임시로 지우고 실행 → 크래시 없이 "처음 입장" 흐름으로 진입하는지 확인.

## 5. 남은 알려진 문제 (다음 단계 후보, 우선순위순)

1. **🔴 재귀적 `main()` 호출** — `monster`/`damage`/`hp_out`/`shop_choose`/`potion_shop`/`slot_machine` 등이 메뉴 복귀 시 `main()`/`adventure()`/`fight()`를 다시 호출 → 콜스택 누적. **1단계 핵심.**
2. **🔴 `auto_save()` 저장 포맷 불일치** — `mp`/`maxmp` 누락(7필드)인데 로더는 9필드+포션 6 기대 → 사냥 후 저장본을 다음 실행에서 잘못 로드. **2단계에서 수정 예정(0단계에서는 의도적으로 보존).**
3. **🟠 `while(!feof(fp))` 루프** — `mob_load`/`magic_load`가 EOF 뒤 쓰레기 1개 더 읽음 + `free` 없음(누수).
4. **🟡 저장 로직 4중 중복** (`save`/`inside_save`/`auto_save`/`clear`) → 단일 함수로 통합 필요.
5. **🟡 입력 검증 부재** — `menu()` 외 ~15곳 `scanf` 미검증 → 문자 입력 시 무한루프.
6. **🟡 포션 로직 중복** — `potion_use` ≈ `fight_pouse` (~120줄 거의 동일).
7. **🟢 미사용 자산** — `speed`/`magic`/`crit`/`mp`가 전투 미반영, `user.item`·`cheat()`·`magic()`/`magic_shop()` 죽은 코드.
8. **🟢 매직넘버 산재** — 회복량 20/100/500, 가격 100/500/1000, 스폰확률 등.
9. **🟢 인코딩 불일치** — `code.c`/`magicspell.txt`=UTF-8 인데 `monsters.txt`=CP949 → 몬스터 이름 깨질 위험.
10. **🟢 Windows 종속** — 크로스플랫폼 시 추상화 필요(우선순위 낮음, C 유지).

## 6. 다음 단계 제안

**1단계: 제어 흐름 정상화.** 함수들이 `main()`을 재귀 호출하는 구조를 "값을 반환하고 `main`의 `while` 루프가 흐름을 제어"하는 방식으로 전환. 이게 정리되어야 이후 파일 분리·기능 확장이 안전해집니다.

---

### 참고: 작업 규칙 (CLAUDE.md 요약)
- 한 번에 한 주제만, 작은 단위로. 기존 감성/흐름 보존. C 언어 유지.
- 변경 전 현재 구조 파악 → 문제 설명 → 작은 수정 → 변경 파일 안내 → 실행/테스트 방법 안내.
- **이 PROGRESS.md는 각 작업 종료 시 갱신한다.**

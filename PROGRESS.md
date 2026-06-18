# 리팩토링 진행 문서 (PROGRESS)

> 이 문서는 **다른 컴퓨터에서도 작업을 이어가기 위한 인수인계용 기록**입니다.
> 작업이 끝날 때마다 갱신합니다. 게임 자체 설명은 [README.md](README.md), 작업 지침은 [CLAUDE.md](CLAUDE.md)를 참고하세요.

- **마지막 업데이트**: 2026-06-18
- **작업 브랜치**: `refactor` (기준: `main`)
- **현재 단계**: 5단계 완료 → 다음은 6단계(게임성: crit/speed/magic 전투 반영, 파이어볼)
- **자율 진행**: 사용자 승인하에 2~7단계를 연속 진행하며, 각 단계 종료 시 이 문서 갱신 + 커밋 + 푸시.

---

## 1. 프로젝트 한눈에 보기

- 고등학교 때 C로 만든 콘솔 Text RPG를 점진적으로 리팩토링하는 프로젝트.
- 소스: [game/](game/) — **모듈 분리됨** (5단계): `game.h` + `main/util/save/player/battle/item.c`
- 데이터/저장: [game/test/](game/test/) — `data.txt`/`stat.txt`(저장), `monsters.txt`/`magicspell.txt`(콘텐츠)
- Windows 전용(`windows.h`, `system`, `Sleep`).

### 파일 구조 (5단계 이후)
| 파일 | 역할 |
|---|---|
| `game.h` | 공용 헤더: 상수·구조체·extern 전역·전체 함수 프로토타입 |
| `main.c` | 전역 변수 정의, `main`/`menu`/`slot_machine`/`help` |
| `util.c` | `open_or_warn`/`read_int`/`cleanup` (입출력·입력 유틸) |
| `save.c` | 저장/불러오기/초기화 (`*_load`, `write_save_files`, `save`, `auto_save`, `new_user` 등) |
| `player.c` | `level`/`level_up`/`stat_view`/`stat_set` |
| `battle.c` | `adventure(_control)`/`monster`/`fight`/`attack`/`damage`/`hp_out` |
| `item.c` | `potion_earn`/`use_one_potion`/`potion_menu`/`item_show`/`shop_choose`/`buy_potion`/`potion_shop` |

## 2. 빌드 & 실행 (중요)

데이터 경로가 `test/` **상대경로**라 **반드시 `game/` 폴더에서** 빌드·실행해야 합니다.
이제 **다중 파일**이므로 `Makefile` 또는 모든 `.c`를 함께 컴파일합니다.

```bash
cd game
make                 # game.exe 생성 (gcc/MinGW)
# 또는 직접:
gcc -std=c11 -Wall -Wextra main.c util.c save.c player.c battle.c item.c -o game.exe
./game.exe
```
Visual Studio: `game/`의 모든 `.c`를 한 프로젝트에 넣고 빌드, 작업 디렉터리를 `game/`로.

> ⚠️ 현재 작업 PC에는 컴파일러가 없어 **빌드 검증은 사용자 환경에서** 해야 합니다.
> 0~5단계는 컴파일 검증 전. 5단계 분할은 파이썬 스크립트로 함수 본문을 그대로 복사하고
> (36개 함수 각 1회, 중복/누락 0) 파일별 중괄호 균형·헤더 프로토타입·extern 전역을 점검했습니다.
> **특히 5단계(분할)는 빌드 한번 돌려보길 권장합니다.**

## 3. 리팩토링 로드맵 & 상태

| 단계 | 주제 | 상태 |
|---|---|---|
| **0** | 빌드·실행 확보 (경로 상대화 + `fopen` NULL 체크) | ✅ 완료 |
| **1** | 제어 흐름 정상화 (재귀 `main()` 호출 제거) | ✅ 완료 |
| **2** | 저장 일원화 + `auto_save` 버그 + `feof` + 누수 | ✅ 완료 |
| **3** | 입력 처리 공통화 (`read_int()`) | ✅ 완료 |
| **4** | 중복 제거(포션) + 매직넘버 상수화 + 미사용 정리 | ✅ 완료 |
| **5** | 파일 분리 (모듈 .c/.h + Makefile) | ✅ 완료 |
| **6** | 게임성·사용성 (crit/speed/magic 전투 반영, 파이어볼 구현) | ⬜ 예정 (다음) |
| **7** | 포트폴리오화 (README Before/After, 빌드 문서) | ⬜ |

## 4. 완료 단계 기록 (요약)

- **0단계** — 경로 상대화 + `open_or_warn()` NULL 처리
- **1단계** — 재귀 `main()` 제거 → 반환 신호(`COMBAT_*`) 전파, 단일 `main` 루프
- **2단계** — `write_save_files()` 통합, `auto_save` 7→9필드 버그, `feof`/누수 수정
- **3단계** — `read_int()` 도입, `scanf` 11곳 교체, 슬롯/스텟 범위 가드
- **4단계** — 포션/상점 중복을 `use_one_potion`/`buy_potion`/`potion_menu`로 통합, 매직넘버 상수화, 죽은 코드 제거 (−224줄)
- **5단계 (이번 작업)** — 단일 `code.c`를 공용 헤더 `game.h` + 역할별 6개 `.c`로 분리, `Makefile` 추가
  - 분할 방법: 임시 파이썬 스크립트로 36개 함수 본문을 원본 그대로 복사(전사 오류 0), 전역은 `main.c`에 정의하고 `game.h`에 `extern` 선언
  - 검증: 함수 36개 각 1회·중복/누락 0, 파일별 중괄호 균형, 미정의(`magic`) 호출 없음

### 5단계 확인 방법(사용자 환경)
1. `cd game && make` (또는 위 gcc 명령) → 경고/오류 확인. **링크 오류(undefined reference)가 없는지** 특히 확인.
2. `./game.exe` 실행 → 메뉴/전투/상점/저장이 이전과 동일하게 동작하는지.

## 5. 남은 작업

1. **🟢 미사용 스탯** — `speed`/`magic`/`crit`/`mp`가 전투 미반영. (6단계에서 실제 사용 + 파이어볼)
2. **🟢 인코딩 불일치** — `monsters.txt`=CP949. (7단계 메모)
3. **🟢 README 갱신** — 빌드법(다중 파일/Makefile) 반영, Before/After. (7단계)

## 6. 다음 단계 제안

**6단계: 게임성.** 전투에서 미사용이던 `crit`(치명타)·`speed`(선제/회피)·`magic`/`mp`(마법)을 실제로 반영하고, 주석 처리된 파이어볼(`magic()`)을 MP 소모형으로 구현한다. 밸런스를 크게 흔들지 않는 선에서.

---

### 참고: 작업 규칙 (CLAUDE.md 요약)
- 한 번에 한 주제만, 작은 단위로. 기존 감성/흐름 보존. C 언어 유지.
- 변경 전 현재 구조 파악 → 문제 설명 → 작은 수정 → 변경 파일 안내 → 실행/테스트 방법.
- **이 PROGRESS.md는 각 작업 종료 시 갱신한다.**

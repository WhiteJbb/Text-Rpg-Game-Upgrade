# 리팩토링 진행 문서 (PROGRESS)

> 이 문서는 **다른 컴퓨터에서도 작업을 이어가기 위한 인수인계용 기록**입니다.
> 게임 설명은 [README.md](README.md), 작업 지침은 [CLAUDE.md](CLAUDE.md)를 참고하세요.

- **마지막 업데이트**: 2026-06-18
- **작업 브랜치**: `feature/magic-shop` (기준: `main`) — 리팩토링 `refactor`는 PR #1로 `main`에 머지 완료
- **현재 상태**: 리팩토링 0~7단계 + 한글 UTF-8 수정 완료(→ main 머지). 후속으로 **마법 상점 + 마법 5종** 추가(이 브랜치, PR 예정).
- **빌드 검증**: 사용자 환경에서 빌드·실행·한글 출력 확인 완료.

---

## ⚠️ 가장 먼저 할 일: 빌드 검증

이 PC에는 컴파일러(gcc/clang/MSVC)가 없어 **0~7단계 전체가 컴파일 검증 전**입니다.
각 단계는 파일별 중괄호/괄호 균형·grep·코드 리뷰·(5단계는 함수 카운트)로 수동 점검했지만,
**실제 빌드는 사용자 환경에서 반드시 한 번 돌려야 합니다.**

```bash
cd game
make
./game.exe
```
경고/오류(특히 5단계 분할의 link 오류, 6단계 전투 변경)를 확인하세요. 문제 시 이 문서의 단계별 기록을 참고.

## 1. 프로젝트 한눈에 보기

- 고등학교 때 C로 만든 콘솔 Text RPG를 점진적으로 리팩토링.
- 소스: [game/](game/) — `game.h` + `main/util/save/player/battle/item.c` + `Makefile`
- 데이터/저장: [game/test/](game/test/)
- Windows 전용(`windows.h`, `system`, `Sleep`).

### 파일 구조
| 파일 | 역할 |
|---|---|
| `game.h` | 공용 헤더: 상수·구조체·extern 전역·전체 프로토타입 |
| `main.c` | 전역 정의, `main`/`menu`/`slot_machine`/`help` |
| `util.c` | `open_or_warn`/`read_int`/`cleanup` |
| `save.c` | 저장/불러오기/초기화/로드 |
| `player.c` | `level`/`level_up`/`stat_view`/`stat_set` |
| `battle.c` | 탐험·전투(`monster`/`fight`/`attack`/`magic`/`monster_counterattack`/`damage`/`hp_out`) |
| `item.c` | 포션·상점 |

## 2. 빌드 & 실행

`test/` 상대경로라 **반드시 `game/`에서** 빌드·실행. 위 "빌드 검증" 참고.

## 3. 리팩토링 로드맵 & 상태 (전부 완료)

| 단계 | 주제 | 상태 |
|---|---|---|
| **0** | 경로 상대화 + `fopen` NULL 체크 | ✅ |
| **1** | 제어 흐름 정상화 (재귀 `main()` 제거) | ✅ |
| **2** | 저장 일원화 + `auto_save` 버그 + `feof` + 누수 | ✅ |
| **3** | 입력 처리 공통화 (`read_int()`) | ✅ |
| **4** | 중복 제거 + 매직넘버 상수화 + 미사용 정리 | ✅ |
| **5** | 파일 분리 (모듈 .c/.h + Makefile) | ✅ |
| **6** | 게임성 (crit/speed/magic 전투 반영, 파이어볼) | ✅ |
| **7** | 포트폴리오화 (README Before/After, 도움말) | ✅ |

## 4. 단계별 완료 기록 (요약)

- **0** — 경로 상대화 + `open_or_warn()` NULL 처리
- **1** — 재귀 `main()` 제거 → 반환 신호(`COMBAT_*`) 전파, 단일 `main` 루프
- **2** — `write_save_files()` 통합, `auto_save` 7→9필드 버그, `feof`/누수 수정
- **3** — `read_int()` 도입, `scanf` 11곳 교체, 슬롯/스텟 범위 가드
- **4** — 포션/상점 중복 통합(`use_one_potion`/`buy_potion`), 매직넘버 상수화, 죽은 코드 제거(−224줄)
- **5** — 단일 `code.c` → `game.h` + 6개 모듈 + `Makefile` (파이썬 스크립트로 함수 무손실 복사)
- **6** — 치명타·회피·파이어볼(MP) 전투 반영, `monster_counterattack` 공용화, 레벨업 MP 회복/성장, 전투창 MP 표시
- **7** — README 전면 갱신(Before/After 표, 단계별 포인트, 파일 구조), 게임 내 `help`에 전투 팁 추가

## 5. 향후(선택) 작업

1. **빌드 검증** (필수, 위 참고) → 통과하면 `refactor` 브랜치를 `main`으로 PR/머지 고려.
2. ✅ 마법 상점(`magic_shop`) 구현 완료 — 마법 5종, 골드로 습득, 보유 상태는 `user.spells` 비트마스크로 저장.
3. 추가 던전(늑대의 굴 등)·몬스터 확장.
4. 크로스 플랫폼(`windows.h`/`system` 추상화).
5. ✅ **데이터 인코딩 통일 완료** — `monsters.txt`를 CP949→UTF-8로 변환, `main()`에서 콘솔을 UTF-8로 설정(`SetConsoleOutputCP`)하여 한글 깨짐 수정. (MSVC 빌드 시 `/utf-8` 필요)

---

### 참고: 작업 규칙 (CLAUDE.md 요약)
- 한 번에 한 주제만, 작은 단위로. 기존 감성/흐름 보존. C 언어 유지.
- **이 PROGRESS.md는 각 작업 종료 시 갱신한다.**

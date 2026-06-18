# 리팩토링 진행 문서 (PROGRESS)

> 이 문서는 **다른 컴퓨터에서도 작업을 이어가기 위한 인수인계용 기록**입니다.
> 작업이 끝날 때마다 갱신합니다. 게임 자체 설명은 [README.md](README.md), 작업 지침은 [CLAUDE.md](CLAUDE.md)를 참고하세요.

- **마지막 업데이트**: 2026-06-18
- **작업 브랜치**: `refactor` (기준: `main`)
- **현재 단계**: 6단계 완료 → 다음은 7단계(포트폴리오화: README/문서)
- **자율 진행**: 사용자 승인하에 2~7단계를 연속 진행하며, 각 단계 종료 시 이 문서 갱신 + 커밋 + 푸시.

---

## 1. 프로젝트 한눈에 보기

- 고등학교 때 C로 만든 콘솔 Text RPG를 점진적으로 리팩토링하는 프로젝트.
- 소스: [game/](game/) — 모듈 분리됨: `game.h` + `main/util/save/player/battle/item.c`
- 데이터/저장: [game/test/](game/test/) — `data.txt`/`stat.txt`(저장), `monsters.txt`/`magicspell.txt`(콘텐츠)
- Windows 전용(`windows.h`, `system`, `Sleep`).

### 파일 구조
| 파일 | 역할 |
|---|---|
| `game.h` | 공용 헤더: 상수·구조체·extern 전역·전체 프로토타입 |
| `main.c` | 전역 정의, `main`/`menu`/`slot_machine`/`help` |
| `util.c` | `open_or_warn`/`read_int`/`cleanup` |
| `save.c` | 저장/불러오기/초기화 |
| `player.c` | `level`/`level_up`/`stat_view`/`stat_set` |
| `battle.c` | 탐험·전투(`monster`/`fight`/`attack`/`magic`/`monster_counterattack`/`damage`/`hp_out`) |
| `item.c` | 포션·상점(`use_one_potion`/`potion_menu`/`buy_potion`/`potion_shop`/`item_show`) |

## 2. 빌드 & 실행 (중요)

데이터 경로가 `test/` **상대경로**라 **반드시 `game/` 폴더에서** 빌드·실행해야 합니다.

```bash
cd game
make                 # game.exe (gcc/MinGW)
# 또는: gcc -std=c11 -Wall -Wextra main.c util.c save.c player.c battle.c item.c -o game.exe
./game.exe
```

> ⚠️ 현재 작업 PC에는 컴파일러가 없어 **빌드 검증은 사용자 환경에서** 해야 합니다.
> 0~6단계는 컴파일 검증 전. 각 단계는 파일별 중괄호/괄호 균형·grep·코드 리뷰로 점검.
> **특히 5단계(분할)·6단계(전투 변경)는 빌드를 한번 돌려보길 권장합니다.**

## 3. 리팩토링 로드맵 & 상태

| 단계 | 주제 | 상태 |
|---|---|---|
| **0** | 빌드·실행 확보 (경로 상대화 + `fopen` NULL 체크) | ✅ 완료 |
| **1** | 제어 흐름 정상화 (재귀 `main()` 호출 제거) | ✅ 완료 |
| **2** | 저장 일원화 + `auto_save` 버그 + `feof` + 누수 | ✅ 완료 |
| **3** | 입력 처리 공통화 (`read_int()`) | ✅ 완료 |
| **4** | 중복 제거(포션) + 매직넘버 상수화 + 미사용 정리 | ✅ 완료 |
| **5** | 파일 분리 (모듈 .c/.h + Makefile) | ✅ 완료 |
| **6** | 게임성 (crit/speed/magic 전투 반영, 파이어볼) | ✅ 완료 |
| **7** | 포트폴리오화 (README Before/After, 빌드 문서) | ⬜ 예정 (다음) |

## 4. 완료 단계 기록 (요약)

- **0단계** — 경로 상대화 + `open_or_warn()` NULL 처리
- **1단계** — 재귀 `main()` 제거 → 반환 신호 전파, 단일 `main` 루프
- **2단계** — `write_save_files()` 통합, `auto_save` 7→9필드 버그, `feof`/누수 수정
- **3단계** — `read_int()` 도입, `scanf` 11곳 교체, 범위 가드
- **4단계** — 포션/상점 중복 통합, 매직넘버 상수화, 죽은 코드 제거 (−224줄)
- **5단계** — 단일 파일 → `game.h` + 6개 모듈 + `Makefile`
- **6단계 (이번 작업)** — 미사용 스탯을 전투에 실제 반영:
  - **치명타(crit)**: `attack`에서 `crit%` 확률로 2배 피해
  - **민첩(speed)**: `monster_counterattack`에서 `speed%` 확률로 회피
  - **지력+MP(magic)**: 주석이던 `magic()`을 **파이어볼**로 구현 (피해 = `mg_data[0].dam + 지력`, MP 소모). `fight`의 "2. 마법" 연결
  - 공용 `monster_counterattack()`로 반격 로직 통합(attack/magic 공유)
  - `level_up`에서 **최대 MP 증가 + MP 전체 회복**, 전투 화면에 MP 표시
  - 변경 파일: [game/battle.c](game/battle.c), [game/player.c](game/player.c), [game/game.h](game/game.h)

### 6단계 확인 방법(사용자 환경)
1. 전투에서 "2. 마법" → MP 소모하며 파이어볼 피해가 들어가는지, MP 부족 시 막히는지.
2. 공격 반복 시 가끔 "치명타", 몬스터 공격 시 가끔 "회피"가 뜨는지(스탯 투자 시 빈도↑).
3. 레벨업 시 MP가 차고 최대 MP가 오르는지.

## 5. 남은 작업 / 알려진 메모

1. **README 갱신(7단계)** — 다중 파일/Makefile 빌드법, Before/After, 기능 목록.
2. **🟢 마법 상점(`magic_shop`)** — 여전히 미구현 스텁(주석). 추후 확장 여지.
3. **🟢 인코딩** — `monsters.txt`=CP949. 빌드 시 한글 깨지면 UTF-8/CP949 일치 필요.

## 6. 다음 단계 제안

**7단계: 포트폴리오화.** README를 갱신한다 — 다중 파일 빌드법(Makefile), Before/After 구조 비교, 리팩토링 포인트(0~6단계), 기능 목록, 향후 계획. 게임 내 `help`에도 전투 메커니즘(치명타/회피/마법) 안내 추가 고려.

---

### 참고: 작업 규칙 (CLAUDE.md 요약)
- 한 번에 한 주제만, 작은 단위로. 기존 감성/흐름 보존. C 언어 유지.
- 변경 전 현재 구조 파악 → 문제 설명 → 작은 수정 → 변경 파일 안내 → 실행/테스트 방법.
- **이 PROGRESS.md는 각 작업 종료 시 갱신한다.**

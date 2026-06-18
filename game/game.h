#ifndef GAME_H
#define GAME_H

#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>
#include <limits.h>
#ifdef _WIN32
#include <windows.h>
#else
#include <unistd.h>   /* usleep (POSIX) */
#include <sys/stat.h> /* mkdir (POSIX) */
#endif

/* 데이터 / 저장 파일 경로 (실행 폴더 기준 상대 경로)
   - 기존에는 "C:\\test\\..." 절대 경로로 고정되어 다른 PC에서 실행이 불가능했음
   - 게임은 game/ 폴더(= test/ 하위 폴더가 있는 위치)에서 실행해야 함 */
#define DATA_PATH      "test/data.txt"
#define STAT_PATH      "test/stat.txt"
#define MONSTERS_PATH  "test/monsters.txt"
#define MAGIC_PATH     "test/magicspell.txt"
#define EQUIP_PATH     "test/equipment.txt"

/* 전투/조우 결과 신호 (제어 흐름 정상화: 재귀 호출 대신 값을 반환) */
#define COMBAT_CONTINUE 0   /* 탐험 계속 (다음 몬스터 조우) */
#define COMBAT_TO_MENU  1   /* 메인 메뉴로 복귀 */
#define COMBAT_ONGOING  2   /* 전투 진행 중 (같은 몬스터 계속) */

/* 포션 회복량 / 상점 가격 (매직넘버 분리) */
#define POTION_S_HEAL  20
#define POTION_M_HEAL  100
#define POTION_L_HEAL  500
#define POTION_S_PRICE 100
#define POTION_M_PRICE 500
#define POTION_L_PRICE 1000

/* 레벨업에 필요한 경험치 (level -> level+1). 밸런스: 기존 *100 에서 *50 으로 완화 */
#define LEVELUP_EXP(lv) ((lv) * (lv) * 50)

typedef struct user {
	int first;
	char name[32];
	int level;
	int exp;
	int hp;
	int maxhp;
	int mp;
	int maxmp;
	int gold;
	int spells;    /* 보유 마법 비트마스크 (bit i = mg_data[i] 보유) */
	int equips;    /* 보유 장비 비트마스크 (bit i = eq_data[i] 보유) */
	int weapon;    /* 착용 무기 인덱스 (-1 = 없음) */
	int armor;     /* 착용 방어구 인덱스 (-1 = 없음) */
}user;

typedef struct mob {
	char name[32];
	int level;
	int emin;
	int emax;
	int hp;
	int maxhp;
	int gmin;
	int gmax;
	int dam;
	int pos;
	int pom;
	int pob;
}mob;

typedef struct healthpotion {
	int count;
	int healing;
}hpo;

typedef struct stat {
	int power;
	int speed;
	int def;
	int magic;
	float crit;
	int luck;
	int point;
}stat;

typedef struct magicspell {
	char name[32];
	int mp;
	int dam;
	int price;     /* 마법상점 가격 (파이어볼=0, 기본 보유) */
	int learned;   /* 보유 여부 0/1 (저장은 user.spells 비트마스크로) */
}mgs;

typedef struct equipment {
	char name[32];
	int slot;      /* 0 = 무기, 1 = 방어구 */
	int power;     /* 힘 보너스 */
	int def;       /* 방어 보너스 */
	int magic;     /* 지력 보너스 */
	int price;
	int owned;     /* 보유 여부 0/1 (저장은 user.equips 비트마스크로) */
}equip;

FILE* open_or_warn(const char* path, const char* mode);
int read_int();
void clear_screen();   /* 화면 지우기 (플랫폼별) */
void pause_screen();   /* 키 입력 대기 (플랫폼별) */
void sleep_ms(int ms); /* ms 대기 (플랫폼별) */
void init_console();   /* 콘솔 UTF-8 설정 (Windows 전용, POSIX는 no-op) */
void ensure_dir(const char* path);  /* 폴더 생성 (이미 있으면 무시) */
void ensure_data_files();           /* 데이터 파일이 없으면 내장 기본값으로 생성 */
void clear();
void user_load();
void mob_load();
void stat_load();
void magic_load();
void equip_load();
void adventure(int region);
void adventure_control();
void potion_earn(int x, int y, int z);
void potion_menu();
int use_one_potion(hpo* potion, int heal, const char* name);
void buy_potion(hpo* potion, int price, const char* name);
void item_show();
void save();
void write_save_files();
void cleanup();
void inside_save();
void level();
void level_up();
void new_user();
int menu();
void end();
void auto_save(int x, int y, int a, int b);  // 매개변수 추가
int monster(int num);
int damage(int dam, int num);
void shop_choose();
void potion_shop();
void magic_shop();
void equip_shop();
int total_power();
int total_def();
int total_magic();
int fight(int num);
int attack(int num);
int magic(int num);
int monster_counterattack(int num);
void stat_view();
int hp_out(int num);
void stat_set();
void slot_machine();
void help();

/* 전역 변수 (정의는 main.c) */
extern user user_data;
extern stat stat_data;
extern mob* mob_data;
extern mgs* mg_data;
extern hpo potions_data;
extern hpo potionm_data;
extern hpo potionb_data;
extern int spell_count;   /* magic_load가 설정하는 마법 개수 */
extern int mob_count;     /* mob_load가 설정하는 몬스터 개수 */
extern equip* eq_data;
extern int equip_count;   /* equip_load가 설정하는 장비 개수 */

#endif /* GAME_H */

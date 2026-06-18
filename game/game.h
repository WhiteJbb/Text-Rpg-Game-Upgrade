#ifndef GAME_H
#define GAME_H

#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>
#include <limits.h>
#include <windows.h>

/* 데이터 / 저장 파일 경로 (실행 폴더 기준 상대 경로)
   - 기존에는 "C:\\test\\..." 절대 경로로 고정되어 다른 PC에서 실행이 불가능했음
   - 게임은 game/ 폴더(= test/ 하위 폴더가 있는 위치)에서 실행해야 함 */
#define DATA_PATH      "test/data.txt"
#define STAT_PATH      "test/stat.txt"
#define MONSTERS_PATH  "test/monsters.txt"
#define MAGIC_PATH     "test/magicspell.txt"

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

typedef struct user {
	int first;
	char name[20];
	int level;
	int exp;
	int hp;
	int maxhp;
	int mp;
	int maxmp;
	int gold;
}user;

typedef struct mob {
	char name[20];
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
	char name[20];
	int mp;
	int dam;
}mgs;

FILE* open_or_warn(const char* path, const char* mode);
int read_int();
void clear();
void user_load();
void mob_load();
void stat_load();
void magic_load();
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

#endif /* GAME_H */

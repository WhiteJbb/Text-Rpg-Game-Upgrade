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
void magic();
void stat_view();
int hp_out(int num);
void stat_set();
void slot_machine();
void help();

user user_data;
stat stat_data;
mob* mob_data;
mgs* mg_data;
hpo potions_data;
hpo potionm_data;
hpo potionb_data;

/* 파일을 열고, 실패하면 경고 메시지를 출력한 뒤 NULL을 반환한다.
   호출부에서 NULL 여부를 확인해 복구 방식을 결정한다. */
FILE* open_or_warn(const char* path, const char* mode) {
	FILE* fp = fopen(path, mode);
	if (fp == NULL) {
		printf(":: [오류] 파일을 열 수 없습니다: %s ::\n", path);
	}
	return fp;
}

/* 동적 할당한 몬스터/마법 데이터 해제 (프로그램 종료 시 호출) */
void cleanup() {
	free(mob_data);
	free(mg_data);
	mob_data = NULL;
	mg_data = NULL;
}

/* 정수 입력을 안전하게 읽는다. 잘못된 입력이면 입력 버퍼를 비우고 INT_MIN을 반환.
   호출부는 INT_MIN(또는 범위 밖)을 잘못된 입력으로 처리한다. */
int read_int() {
	int value;
	int c;
	if (scanf("%d", &value) != 1) {
		while ((c = getchar()) != '\n' && c != EOF);
		return INT_MIN;
	}
	while ((c = getchar()) != '\n' && c != EOF);
	return value;
}

int main() {
	srand((int)time(NULL));
	user_load();
	mob_load();
	stat_load();
	magic_load();
	printf("-------------------------------------\n\n");
	printf(":: 유저 %s님 '행성 : 지구' 입장! ::\n\n", user_data.name);
	while (1) {
		int menu_choice = menu();
        if (menu_choice == INT_MIN) {
            printf(":: 잘못된 입력입니다. 다시 시도해주세요 ::\n");
            continue;
        }
		switch (menu_choice) {
		case 1:
			adventure_control();
			break;
		case 2:
			level();
			stat_view();
			break;
		case 3:
			item_show();
			break;
		case 4:
			shop_choose();
			break;
		case 5:
			stat_set();
			break;
		case 6:
			slot_machine();
			break;
		case 7:
			help();
			break;
		case 8:
			end();
			cleanup();
			return 0;
		case 9:
			clear();
			cleanup();
			return 0;
		default:
			printf(":: 다시 선택해주세요 ::");
			break;
		}
		system("cls");

	}


	return 0;

}



int menu() {
	int sel;
	printf("-------------------------------------\n\n");
	printf("1. 탐험\n");
	printf("2. 정보확인\n");
	printf("3. 아이템 확인\n");
	printf("4. 상점\n");
	printf("5. 스텟 설정\n");
	printf("6. 슬롯머신\n");
	printf("7. 이용가이드\n");
	printf("8. 종료\n");
	printf("9. 초기화\n");
	printf("\n-------------------------------------\n");
	printf("무엇을 하시겠습니까? : ");


	sel = read_int();
	return sel;


}

void user_load() {

	FILE* fp;

	fp = open_or_warn(DATA_PATH, "r");
	if (fp == NULL) {
		/* 저장 파일이 없으면 첫 실행으로 간주하고 새 캐릭터를 생성한다 */
		new_user();
		return;
	}

	fscanf(fp, "%d %s %d %d %d %d %d %d %d %d %d %d %d %d %d ", &user_data.first, user_data.name, &user_data.gold, &user_data.exp, &user_data.level, &user_data.hp, &user_data.maxhp, &user_data.mp, &user_data.maxmp, &potions_data.count, &potionm_data.count, &potionb_data.count, &potions_data.healing, &potionm_data.healing, &potionb_data.healing);

	fclose(fp);

	if (user_data.first == 0) {
		new_user();
	}
}

void stat_load() {
	FILE* fp;

	fp = open_or_warn(STAT_PATH, "r");
	if (fp == NULL) {
		/* 스텟 파일이 없으면 기본값(전역 0 초기화 / new_user 설정)을 유지한다 */
		return;
	}

	fscanf(fp, "%d %d %d %f %d %d %d ", &stat_data.power, &stat_data.speed, &stat_data.def, &stat_data.crit, &stat_data.magic, &stat_data.luck, &stat_data.point);

	fclose(fp);

}

void mob_load() {
	mob_data = (mob*)malloc(sizeof(mob));

	int i = 0;

	FILE* fp;

	fp = open_or_warn(MONSTERS_PATH, "r");
	if (fp == NULL) {
		printf(":: 몬스터 데이터를 불러올 수 없어 게임을 종료합니다 ::\n");
		exit(1);
	}

	/* EOF 뒤 쓰레기 1개를 더 읽던 while(!feof) 버그 수정: 읽기 성공 필드 수로 판단 */
	while (1) {
		mob* tmp = realloc(mob_data, sizeof(mob) * (i + 1));
		if (tmp == NULL) break;
		mob_data = tmp;
		if (fscanf(fp, "%s %d %d %d %d %d %d %d %d %d %d %d",
			mob_data[i].name, &mob_data[i].level, &mob_data[i].emin, &mob_data[i].emax,
			&mob_data[i].gmin, &mob_data[i].gmax, &mob_data[i].hp, &mob_data[i].maxhp,
			&mob_data[i].dam, &mob_data[i].pos, &mob_data[i].pom, &mob_data[i].pob) != 12)
			break;
		i++;
	}

	fclose(fp);
}

void magic_load() {
	mg_data = (mgs*)malloc(sizeof(mgs));

	int i = 0;

	FILE* fp;

	fp = open_or_warn(MAGIC_PATH, "r");
	if (fp == NULL) {
		printf(":: 마법 데이터를 불러올 수 없어 게임을 종료합니다 ::\n");
		exit(1);
	}

	while (1) {
		mgs* tmp = realloc(mg_data, sizeof(mgs) * (i + 1));
		if (tmp == NULL) break;
		mg_data = tmp;
		if (fscanf(fp, "%s %d %d", mg_data[i].name, &mg_data[i].mp, &mg_data[i].dam) != 3)
			break;
		i++;
	}

	fclose(fp);
}

void stat_view() {
	system("cls");
	printf("-------------------------------------\n\n");
	printf("%s의 스텟\n\n", user_data.name);
	printf("-------------------------------------\n");
	printf(":: 힘 : %d ::\n", stat_data.power);
	printf(":: 지력 : %d ::\n", stat_data.magic);
	printf(":: 민첩 : %d ::\n", stat_data.speed);
	printf(":: 방어 : %d ::\n", stat_data.def);
	printf(":: 크리티컬 : %.1f%% ::\n", stat_data.crit);
	printf(":: 운 : %d ::\n", stat_data.luck);
	printf("==> 투자 가능한 스텟 포인트 : %d\n", stat_data.point);
	printf("-------------------------------------\n");
	system("pause");
}

void stat_set() {
	int sel = 0;
	int much = 0;
	while (1) {
		system("cls");
		printf("-------------------------------------\n\n");
		printf(":: 스텟 설정 ::   - 남은 스텟 포인트 : %d\n", stat_data.point);
		printf("\n-------------------------------------\n\n");
		printf("1. 힘        2. 민첩        3.방어\n\n");
		printf("4. 지력      5. 크리티컬    6.운\n");
		printf("\n-------------------------------------\n");
		if (stat_data.point == 0) {
			Sleep(200);
			printf(":: 투자할 스텟 포인트가 없습니다 ::\n");
			system("pause");
			system("cls");
			return;
		}
		else if (stat_data.point > 0) {
			printf("어떤 스텟을 올리시겠습니까? : ");
			sel = read_int();
			printf("%d번 스텟을 선택하셨습니다.\n", sel);
			printf("얼마나 올리시겠습니까? : ");
			much = read_int();
			if (much <= 0 || much > stat_data.point) {
				printf(":: 1 이상, 남은 포인트(%d) 이하로 입력해주세요 ::\n", stat_data.point);
				system("pause");
				system("cls");
				continue;
			}
			switch (sel) {
			case 1:
				printf("\n-------------------------------------\n\n");
				printf(":: 열심히 운동하는 중입니다..... ::\n");
				printf("\n-------------------------------------\n\n");
				Sleep(1000);
				printf(":: 힘 + %d ::\n", much);
				printf("\n-------------------------------------\n");
				stat_data.power += much;
				stat_data.point -= much;
				inside_save();
				system("pause");
				system("cls");
				return;
				break;
			case 2:
				printf("\n-------------------------------------\n\n");
				printf(":: 더욱 빨라지는 중입니다..... ::\n");
				printf("\n-------------------------------------\n\n");
				Sleep(1000);
				printf(":: 민첩 + %d ::\n", much);
				printf("\n-------------------------------------\n");
				stat_data.speed += much;
				stat_data.point -= much;
				inside_save();
				system("pause");
				system("cls");
				return;
				break;
			case 3:
				printf("\n-------------------------------------\n\n");
				printf(":: 몸이 단단해지는 중입니다..... ::\n");
				printf("\n-------------------------------------\n\n");
				Sleep(1000);
				printf(":: 방어 + %d ::\n", much);
				printf("\n-------------------------------------\n");
				stat_data.def += much;
				stat_data.point -= much;
				inside_save();
				system("pause");
				system("cls");
				return;
				break;
			case 4:
				printf("\n-------------------------------------\n\n");
				printf(":: 마법에 대한 이해가 늘어나는 중입니다..... ::\n");
				printf("\n-------------------------------------\n\n");
				Sleep(1000);
				printf(":: 지력 + %d ::\n", much);
				printf("\n-------------------------------------\n");
				stat_data.magic += much;
				stat_data.point -= much;
				inside_save();
				system("pause");
				system("cls");
				return;
				break;
			case 5:
				printf("\n-------------------------------------\n\n");
				printf(":: 급소를 더 잘 찌르는법을 배우는 중입니다..... ::\n");
				printf("\n-------------------------------------\n\n");
				Sleep(1000);
				printf(":: 크리티컬 + %.1f%% ::\n", much * 0.5);
				printf("\n-------------------------------------\n");
				stat_data.crit += much * 0.5;
				stat_data.point -= much;
				inside_save();
				system("pause");
				system("cls");
				return;
				break;
			case 6:
				printf("\n-------------------------------------\n\n");
				printf(":: 네잎클로버를 찾는 중입니다..... ::\n");
				printf("\n-------------------------------------\n\n");
				Sleep(1000);
				printf(":: 운 + %d ::\n", much);
				printf("\n-------------------------------------\n");
				stat_data.luck += much;
				stat_data.point -= much;
				inside_save();
				system("pause");
				system("cls");
				return;
				break;
			default:
				printf(":: 다시 선택해주세요 ::\n");
				system("pause");
				break;
			}

		}

	}

}

void new_user() {
	printf("------------------------------------------------\n\n");
	printf(":: '행성 : 지구'에 처음 입장하셨습니다 ::\n\n");
	printf("------------------------------------------------\n");
	printf("닉네임을 등록해주세요 : ");
	fgets(user_data.name, sizeof(user_data.name), stdin);
    user_data.name[strcspn(user_data.name, "\n")] = 0;
    printf("\n:: %s님 환영합니다! ::\n", user_data.name);

	user_data.first = 1;
	user_data.exp = 0;
	user_data.level = 1;
	user_data.hp = 100;
	user_data.maxhp = 100;
	user_data.mp = 100;
	user_data.maxmp = 100;
	user_data.gold = 0;
	potions_data.count = 3;
	potionm_data.count = 1;
	potionb_data.count = 0;
	potions_data.healing = POTION_S_HEAL;
	potionm_data.healing = POTION_M_HEAL;
	potionb_data.healing = POTION_L_HEAL;
	stat_data.power = 5;
	stat_data.speed = 5;
	stat_data.def = 5;
	stat_data.crit = 0.5;
	stat_data.magic = 5;
	stat_data.luck = 1;
	stat_data.point = 0;
	save();
	system("cls");
}

/* 저장 파일(data.txt, stat.txt)에 현재 상태를 기록한다.
   기존에 save/inside_save/auto_save/clear에 중복되어 있던 파일쓰기를 일원화. */
void write_save_files() {
	FILE* fp = open_or_warn(DATA_PATH, "w");
	if (fp == NULL) return;
	fprintf(fp, "%d %s %d %d %d %d %d %d %d ", user_data.first, user_data.name, user_data.gold, user_data.exp, user_data.level, user_data.hp, user_data.maxhp, user_data.mp, user_data.maxmp);
	fprintf(fp, "%d %d %d %d %d %d ", potions_data.count, potionm_data.count, potionb_data.count, potions_data.healing, potionm_data.healing, potionb_data.healing);
	fclose(fp);

	fp = open_or_warn(STAT_PATH, "w");
	if (fp == NULL) return;
	fprintf(fp, "%d %d %d %f %d %d %d\n", stat_data.power, stat_data.speed, stat_data.def, stat_data.crit, stat_data.magic, stat_data.luck, stat_data.point);
	fclose(fp);
}

void save() {
	write_save_files();
	printf("-------------------------------------\n\n");
	printf(":: 저장중,,,, ::\n");
	printf(":: 저장이 완료되었습니다 ::\n");
	printf("\n-------------------------------------\n");
	system("pause");
}

void inside_save() {
	write_save_files();
}

void end() {
	save();
	printf("-------------------------------------\n\n");
	printf(":: 로그아웃 ::\n\n");
	printf("-------------------------------------\n");
	return;
}

void clear() {
	/* 초기화: 전역 상태를 기본값으로 되돌리고 저장한다 (이후 main에서 프로그램 종료).
	   기존에는 같은 이름의 지역 변수로 전역을 가려(shadow) 실제 전역은 그대로 두던
	   잠재 버그가 있었음. first=0 이므로 다음 실행 시 new_user()가 새 캐릭터를 만든다. */
	user_data.first = 0;
	user_data.name[0] = '\0';
	user_data.level = 0;
	user_data.exp = 0;
	user_data.hp = 0;
	user_data.maxhp = 0;
	user_data.mp = 0;
	user_data.maxmp = 0;
	user_data.gold = 0;
	potions_data.count = 0; potions_data.healing = POTION_S_HEAL;
	potionm_data.count = 0; potionm_data.healing = POTION_M_HEAL;
	potionb_data.count = 0; potionb_data.healing = POTION_L_HEAL;
	stat_data.power = 5; stat_data.speed = 5; stat_data.def = 5;
	stat_data.crit = 0.1; stat_data.magic = 5; stat_data.luck = 1; stat_data.point = 0;

	write_save_files();

	printf("-------------------------------------\n\n");
	printf(":: 초기화 완료 ::\n\n");
	printf("-------------------------------------\n");
}

void adventure_control() {
	system("cls");
	int num;
	printf("-------------------------------------\n\n");
	printf("1. 몬스터의 초원 (Lv.1 ~)\n");
	printf("2. 늑대의 굴 \n");
	printf("3. 엘프의 숲\n");
	printf("4. 뱀파이어의 성\n");
	printf("5. 메뉴\n");
	printf("\n-------------------------------------\n");
	printf("어느지역으로 가시겠습니까? : ");
	num = read_int();
	system("cls");
	adventure(num);
}

void adventure(int region) {
	int num;
	while (1) {
		num = rand() % 10000 + 1;
		switch (region) {
		case 1:
			printf("-------------------------------------\n");
			printf(":: %s님의 HP : %d / %d ::\n", user_data.name, user_data.hp, user_data.maxhp);
			printf("-------------------------------------\n");
			printf(":: 몬스터의 초원 ::\n");
			printf(":: 곳곳에서 섬뜩한 눈빛이 느껴집니다... ::\n");
			if (num <= 5000) {
				if (monster(0) == COMBAT_TO_MENU) return;
				break;
			}
			if (num > 5000 && num <= 8000) {
				if (monster(1) == COMBAT_TO_MENU) return;
				break;
			}
			if (num > 8000 && num <= 9900) {
				if (monster(2) == COMBAT_TO_MENU) return;
				break;
			}
			if (num > 9900) {
				if (monster(3) == COMBAT_TO_MENU) return;
				break;
			}
			break;
		case 5:
			return;
		default:
			printf(":: 아직 갈 수 없는 지역입니다 ::\n");
			system("pause");
			return;
		}

	}
}

void level() {
	system("cls");
	printf("-------------------------------------\n");
	printf(":: 닉네임 : %s ::\n", user_data.name);
	printf(":: HP : %d / %d ::\n", user_data.hp, user_data.maxhp);
	printf(":: MP : %d / %d ::\n", user_data.mp, user_data.maxmp);
	printf(":: 골드 : %dGold ::\n", user_data.gold);
	printf(":: 경험치 : %dexp ::\n", user_data.exp);
	printf("::  레벨  : %d ::\n", user_data.level);
	printf("==> %d레벨까지 %dexp\n", user_data.level + 1, user_data.level * user_data.level * 100 - user_data.exp);
	printf("-------------------------------------\n");
	system("pause");
}

void level_up() {
	if (user_data.exp >= user_data.level * user_data.level * 100) {
		user_data.level += 1;
		printf("-------------------------------------\n");
		printf("\n:: 레벨업! :: 현재 레벨 : %d\n", user_data.level);
		user_data.maxhp += user_data.level * 10;
		printf(":: 최대 HP  + %d ::\n", user_data.level * 10);
		user_data.hp = user_data.maxhp;
		printf(":: HP가 회복되었습니다 ::\n");
		printf(":: 스텟 포인트 + 5 ::\n\n");
		stat_data.point += 5;
		printf("-------------------------------------\n");

		if (user_data.level < 11) {
			printf(":: [보상] HP포션(소) + 1 ::\n");
			potions_data.count++;
		}
		else if (user_data.level >= 11 && user_data.level < 31) {
			printf(":: [보상] HP포션(소) + 10 ::\n");
			printf(":: [보상] HP포션(중) + 1 ::\n");
			potions_data.count += 10;
			potionm_data.count++;
		}
		else if (user_data.level >= 31 && user_data.level < 51) {
			printf(":: [보상] HP포션(소) + 100 ::\n");
			printf(":: [보상] HP포션(중) + 10 ::\n");
			printf(":: [보상] HP포션(대) + 1 ::\n");
			potions_data.count += 100;
			potionm_data.count += 10;
			potionb_data.count++;
		}
	}
}

void auto_save(int x, int y, int a, int b) {
	int exp;
	exp = (rand() % x + y);
	printf("\n+++++++++++++++++++++++++++++++++++++\n");
	printf("\n:: %dexp를 획득했습니다 ::\n\n", exp);
	user_data.exp += exp;
	printf(":: 현재 경험치 : %dexp ::\n", user_data.exp);
	printf("\n+++++++++++++++++++++++++++++++++++++\n");
	level_up();

	int gold;
	gold = (rand() % a + b);
	user_data.gold += gold;

	printf("+++++++++++++++++++++++++++++++++++++\n\n");
	printf(":: %dGold를 획득하셨습니다. :: \n\n", gold);
	printf(":: 현재 골드 : %d Gold ::\n\n", user_data.gold);
	printf("+++++++++++++++++++++++++++++++++++++\n");

	write_save_files();   /* 버그 수정: 이전엔 mp/maxmp 누락(7필드)으로 직접 기록했음 */
}

int monster(int num) {
	int sel, run;
	run = rand() % 100 + 1;
	while (1) {
		printf("-------------------------------------\n");
		printf("\n:: %s(이)가 나타났다 ::\n\n", mob_data[num].name);
		printf("-------------------------------------\n");
		printf(":: 무엇을 하시겠습니까? ::\n");
		printf("\n1. 싸운다 , 2. 도망간다 , 3. 아이템사용 : ");
		sel = read_int();
		switch (sel) {
		case 1:
			return fight(num);
		case 2:
			if (run >= 30) {
				printf("\n:: 무사히 도망쳤다 ::\n");
				inside_save();
				system("pause");
				system("cls");
				return COMBAT_TO_MENU;
			}
			else {
				printf("\n:: 도망치지 못했다 ::\n");
				Sleep(500);
				return fight(num);
			}
		case 3:
			potion_menu();
			break;   /* 회복 후 다시 조우 프롬프트로 돌아감 */
		default:
			break;
		}
	}
}

int damage(int dam, int num) {
	user_data.hp -= dam;
	if (user_data.hp <= 0 && user_data.level != 1) {
		system("cls");
		printf("-------------------------------------\n\n");
		printf(":: 사망하셨습니다 ::\n");
		printf(":: 레벨 - 1 ::\n\n");
		user_data.maxhp -= user_data.level * 10;
		user_data.level -= 1;
		user_data.exp = (user_data.level - 1) * (user_data.level - 1) * 100 + 1;
		user_data.hp = user_data.maxhp;
		save();
		mob_data[num].hp = mob_data[num].maxhp;
		printf("-------------------------------------\n\n");
		printf(":: System :: 메인화면으로 돌아갑니다 \n");
		printf("\n-------------------------------------\n");
		system("pause");
		system("cls");
		return 1;
	}
	else if (user_data.hp <= 0 && user_data.level == 1) {
		system("cls");
		printf("-------------------------------------\n\n");
		printf(":: 사망하셨습니다 ::\n");
		printf(":: 레벨이 1이므로 페널티를 받지 않습니다 ::\n\n");
		user_data.hp = user_data.maxhp;
		save();
		mob_data[num].hp = mob_data[num].maxhp;
		printf("-------------------------------------\n\n");
		printf(":: System :: 메인화면으로 돌아갑니다 \n");
		printf("\n-------------------------------------\n");
		system("pause");
		system("cls");
		return 1;
	}
	printf("-------------------------------------\n\n");
	printf(":: HP - %d ::\n", dam);
	printf("\n-------------------------------------\n");
	return 0;
}

void potion_earn(int x, int y, int z) {
	int earns;
	int earnm;
	int earnb;
	earns = rand() % 10000 + 1;
	earnm = rand() % 10000 + 1;
	earnb = rand() % 10000 + 1;
	if (earns <= x) {
		potions_data.count++;
		printf("\n++ 획득 -> HP포션(소) ++\n");
		printf(":: HP포션(소) : %d개 -> %d개 ::\n", potions_data.count - 1, potions_data.count);
	}
	if (earnm <= y) {
		potionm_data.count++;
		printf("\n++ 획득 -> HP포션(중) ++\n");
		printf(":: HP포션(중) : %d개 -> %d개 ::\n", potionm_data.count - 1, potionm_data.count);
	}
	if (earnb <= z) {
		potionb_data.count++;
		printf("\n++ 획득 -> HP포션(대) ++\n");
		printf(":: HP포션(대) : %d개 -> %d개 ::\n", potionb_data.count - 1, potionb_data.count);
	}
	else {
		printf("\n:: 아무것도 나오지않았다... ::\n\n");
	}
}

/* 포션 1종 사용 시도: 회복했으면 1, 보유 없음/회복 불필요면 메시지 출력 후 0 반환.
   기존 potion_use/fight_pouse의 거의 동일한 ~120줄 중복을 이 함수로 일원화. */
int use_one_potion(hpo* potion, int heal, const char* name) {
	int sub = user_data.maxhp - user_data.hp;
	if (potion->count == 0) {
		printf(":: [system] %s을(를) 보유하고있지 않습니다. ::\n", name);
		return 0;
	}
	if (sub == 0) {
		printf(":: [system] 데미지를 입지 않아 회복할 수 없습니다. ::\n");
		return 0;
	}
	int recovered = (sub < heal) ? sub : heal;   /* 최대 HP를 넘지 않도록 부족분만큼만 */
	user_data.hp += recovered;
	potion->count--;
	printf("\n:: %s 사용 ::\n", name);
	printf(":: HP %d이 회복되었습니다 ::\n", recovered);
	printf(":: HP %d -> %d ::\n", user_data.hp - recovered, user_data.hp);
	printf(":: %s %d개 -> %d개 ::\n", name, potion->count + 1, potion->count);
	return 1;
}

/* 포션 사용 메뉴 (조우 중·전투 중 공용). 4번 선택 시 저장 후 복귀. */
void potion_menu() {
	int sel;
	while (1) {
		system("cls");
		printf("-------------------------------------\n\n");
		printf(":: 어떤 포션을 사용하시겠습니까? ::\n\n");
		printf("-------------------------------------\n");
		printf(":: HP포션(소) %d개 보유 ::\n", potions_data.count);
		printf(":: HP포션(중) %d개 보유 ::\n", potionm_data.count);
		printf(":: HP포션(대) %d개 보유 ::\n", potionb_data.count);
		printf("-------------------------------------\n\n");
		printf("1. HP포션(소) , 2. HP포션(중), 3. HP포션(대) 4. 돌아가기 : ");
		sel = read_int();
		switch (sel) {
		case 1:
			use_one_potion(&potions_data, POTION_S_HEAL, "HP포션(소)");
			system("pause");
			break;
		case 2:
			use_one_potion(&potionm_data, POTION_M_HEAL, "HP포션(중)");
			system("pause");
			break;
		case 3:
			use_one_potion(&potionb_data, POTION_L_HEAL, "HP포션(대)");
			system("pause");
			break;
		case 4:
			printf(":: 돌아갑니다. ::\n");
			save();
			return;
		default:
			break;
		}
	}
}

void item_show() {
	system("cls");
	printf("-------------------------------------\n\n");
	printf(":: %s님의 아이템 보유목록 ::\n\n", user_data.name);
	printf("-------------------------------------\n");
	printf(":: HP포션(소) %d개 보유 ::\n", potions_data.count);
	printf(":: HP포션(중) %d개 보유 ::\n", potionm_data.count);
	printf(":: HP포션(대) %d개 보유 ::\n", potionb_data.count);
	printf("-------------------------------------\n");
	system("pause");
	system("cls");
	printf("-------------------------------------\n\n");
	printf(":: %s님의 마법 보유목록 ::\n\n", user_data.name);
	printf("-------------------------------------\n");
	printf(":: %s 보유 ::\n", mg_data[0].name);
	printf("-------------------------------------\n");
	system("pause");
	system("cls");
}

void shop_choose() {
	int sel;
	while (1) {
		system("cls");
		printf("-------------------------------------\n\n");
		printf("1. 물약상점\n");
		printf("2. 마법상점\n");
		printf("3. 돌아가기\n");
		printf("\n-------------------------------------\n\n");
		printf("어느 곳으로 가시겠습니까? : ");
		sel = read_int();

		switch (sel) {
		case 1:
			potion_shop();
			break;
		case 2:
			//magic_shop();
			break;
		case 3:
			system("cls");
			return;
		default:
			printf(":: 다시 입력해주세요 ::");
			break;
		}
	}
}

/* 포션 구매 시도: 골드가 충분하면 구매, 아니면 부족 안내. (3종 중복 로직 일원화) */
void buy_potion(hpo* potion, int price, const char* name) {
	printf("-------------------------------------\n\n");
	if (user_data.gold >= price) {
		potion->count += 1;
		user_data.gold -= price;
		printf(":: %s 구매완료 ::\n", name);
		printf(":: 현재 보유 : %d개 -> %d개 ::\n", potion->count - 1, potion->count);
	}
	else {
		printf(":: 골드가 부족합니다 ::\n");
	}
	printf("\n-------------------------------------\n\n");
	system("pause");
}

void potion_shop() {
	int sel;
	while (1) {
		system("cls");
		printf("-------------------------------------\n\n");
		printf(":: 물약상점 ::       골드 보유량  : %d Gold\n", user_data.gold);
		printf("\n-------------------------------------\n\n");
		printf("1. HP포션(소) : %d Gold\n", POTION_S_PRICE);
		printf("2. HP포션(중) : %d Gold\n", POTION_M_PRICE);
		printf("3. HP포션(대) : %d Gold\n", POTION_L_PRICE);
		printf("4. 돌아가기\n");
		printf("\n-------------------------------------\n\n");
		printf("무엇을 구매하시겠습니까? : ");
		sel = read_int();

		switch (sel) {
		case 1:
			buy_potion(&potions_data, POTION_S_PRICE, "HP포션(소)");
			break;
		case 2:
			buy_potion(&potionm_data, POTION_M_PRICE, "HP포션(중)");
			break;
		case 3:
			buy_potion(&potionb_data, POTION_L_PRICE, "HP포션(대)");
			break;
		case 4:
			printf("-------------------------------------\n\n");
			printf(":: 돌아갑니다 ::\n");
			printf("\n-------------------------------------\n");
			save();
			system("cls");
			return;
		default:
			break;
		}
	}
}

/*void magic_shop() {

}*/

int fight(int num) {
	int sel = 0;

	while (1) {
		system("cls");
		printf("------------------------------------------\n\n");
		printf(":: Fight!! ::");
		printf("\n\n------------------------------------------");
		printf("\n\n:: Lv.%d ::\n:: %s :: \n:: HP : %d / %d ::", mob_data[num].level, mob_data[num].name, mob_data[num].hp, mob_data[num].maxhp);
		printf("\n\n     VS              \n\n");
		printf(":: Lv.%d :: \n:: %s :: \n:: HP : %d / %d ::\n\n", user_data.level, user_data.name, user_data.hp, user_data.maxhp);
		printf("------------------------------------------\n");
		printf("1. 공격, 2. 마법, 3. 아이템사용 : ");
		sel = read_int();
		int result = COMBAT_ONGOING;
		switch (sel) {
		case 1:
			result = attack(num);
			break;
		case 2:
			//magic();
			break;
		case 3:
			potion_menu();
			break;
		}
		if (result == COMBAT_CONTINUE || result == COMBAT_TO_MENU)
			return result;
	}

}

int attack(int num) {
	int dam = mob_data[num].dam - stat_data.def;
	if (dam < 0)
		dam = 0;

	printf("------------------------------------------\n");
	printf("\n:: %s의 공격! ::\n\n", user_data.name);
	printf("------------------------------------------\n\n");
	Sleep(500);
	if (mob_data[num].hp >= stat_data.power) {
		mob_data[num].hp -= stat_data.power;
		printf(":: ""%s"":에게 %d의 피해를 주었다. ::\n\n", mob_data[num].name, stat_data.power);
		printf("------------------------------------------\n");
	}
	else if (mob_data[num].hp < stat_data.power && mob_data[num].hp > 0) {
		printf(":: ""%s"":에게 %d의 피해를 주었다. ::\n\n", mob_data[num].name, mob_data[num].hp);
		printf("------------------------------------------\n");
		mob_data[num].hp -= mob_data[num].hp;
	}
	if (hp_out(num))
		return COMBAT_CONTINUE;   /* 몬스터 처치 -> 반격 생략, 탐험 계속 */
	Sleep(500);
	printf("------------------------------------------\n");
	printf("\n:: %s의 공격! ::\n\n", mob_data[num].name);
	Sleep(500);
	if (damage(dam, num))
		return COMBAT_TO_MENU;    /* 플레이어 사망 -> 메뉴 복귀 */
	system("pause");
	return COMBAT_ONGOING;
}

/*void magic(int num, int reg) {

}
*/

int hp_out(int num) {

	if (mob_data[num].hp <= 0) {
		printf("\n:: %s을 죽였습니다 ::\n", mob_data[num].name);
		auto_save(mob_data[num].emin, mob_data[num].emax, mob_data[num].gmin, mob_data[num].gmax);
		potion_earn(mob_data[num].pos, mob_data[num].pom, mob_data[num].pob);
		system("pause");
		mob_data[num].hp = mob_data[num].maxhp;
		system("cls");
		return 1;
	}
	return 0;
}

void slot_machine() {
	int i = 0;
	int much = 0;;
	int percent = 0;
	percent = rand() % 10000 + 1;
	while (i < 6) {
		system("cls");
		printf("■□■□■□■□■□■□■□■□■□■□■□■□\n\n");
		printf(":: 슬롯머신 :: - 넣은 돈을 2배 불려드립니다\n\n");
		printf("■□■□■□■□■□■□■□■□■□■□■□■□\n\n");
		Sleep(100);
		system("cls");
		printf("□■□■□■□■□■□■□■□■□■□■□■□■\n\n");
		printf(":: 슬롯머신 :: - 넣은 돈을 2배 불려드립니다\n\n");
		printf("□■□■□■□■□■□■□■□■□■□■□■□■\n\n");
		Sleep(100);
		i++;
	}
	if (user_data.gold == 0) {
		printf(":: 보유하고 계신 Gold가 없습니다 ::\n");
		system("pause");
		system("cls");
		return;
	}
	else if (user_data.gold > 0) {
		printf("얼마를 넣으시겠습니까? - %d Gold 보유 : ", user_data.gold);
		much = read_int();
		if (much <= 0) {
			printf(":: 올바른 금액을 입력해주십시오 ::\n");
			system("pause");
		}
		else if (much > user_data.gold) {
			printf(":: 보유 골드보다 많이 걸 수 없습니다 ::\n");
			system("pause");
		}
		else {
			printf("\n\n:: - %d Gold ::\n\n", much);
			printf(":: 슬롯머신을 가동합니다 ::\n\n");
			user_data.gold -= much;
			Sleep(250);
			printf("★      ●       ◆\n\n");
			Sleep(250);
			printf("★      ◆       ●\n\n");
			Sleep(250);
			printf("◆      ●       ★\n\n");
			Sleep(250);
			printf("●      ★       ◆\n\n");
			Sleep(250);
			printf("●      ◆       ★\n\n");
			if (percent < 50 + (stat_data.luck * 50)) {
				Sleep(1000);
				printf("★       ★       ★\n\n");
				Sleep(100);
				printf(":: 당첨!!!! ::\n");
				printf(":: 축하드립니다 ::\n\n");
				printf("------------------------------------------\n\n");
				printf(":: + %d Gold ::\n\n", much * 2);
				printf("------------------------------------------\n");
				user_data.gold += much * 2;
				inside_save();
				system("pause");
				system("cls");
				return;
			}
			else {
				Sleep(1000);
				printf("★      ★       ◆\n\n");
				Sleep(50);
				printf(":: 당첨되지 않으셨습니다 ::\n");
				inside_save();
				system("pause");
				system("cls");
				return;
			}
		}
	}
}

void help() {
	system("cls");
	printf("----------------------------------------\n\n");
	printf(":: 행성 지구 이용 가이드 ::\n\n");
	printf("1. 탐험 : 탐험을 통해 골드와 경험치, 아이템을 얻을 수 있습니다. \n");
	printf("2. 정보확인 : 캐릭터의 정보와 스텟을 확인할 수 있습니다. \n");
	printf("3. 아이템 확인 : 아이템을 확인합니다. \n");
	printf("   - HP포션(소) HP + 20 \n");
	printf("   - HP포션(중) HP + 100 \n");
	printf("   - HP포션(대) HP + 500 \n");
	printf("4. 상점 : 아이템을 살 수 있는 상점입니다. \n");
	printf("5. 스텟 설정 : 레벨업으로 얻은 스텟을 분배하는 기능입니다. \n");
	printf("6. 슬롯머신 : 모험에서 얻은 골드를 2배로 불릴 수 있는 기회! \n");
	printf("\n----------------------------------------\n");
	system("pause");
}
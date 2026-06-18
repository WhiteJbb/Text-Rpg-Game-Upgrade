#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>
#include <windows.h>

/* 데이터 / 저장 파일 경로 (실행 폴더 기준 상대 경로)
   - 기존에는 "C:\\test\\..." 절대 경로로 고정되어 다른 PC에서 실행이 불가능했음
   - 게임은 game/ 폴더(= test/ 하위 폴더가 있는 위치)에서 실행해야 함 */
#define DATA_PATH      "test/data.txt"
#define STAT_PATH      "test/stat.txt"
#define MONSTERS_PATH  "test/monsters.txt"
#define MAGIC_PATH     "test/magicspell.txt"

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
	char** item;
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
void clear();
void user_load();
void mob_load();
void stat_load();
void magic_load();
void adventure(int region);
void adventure_control();
void potion_earn(int x, int y, int z);
void potion_use();
void item_show();
void save();
void inside_save();
void level();
void level_up();
void new_user();
int menu();
void end();
void auto_save(int x, int y, int a, int b);  // 매개변수 추가
void monster(int num, int region);           // 매개변수 추가
void damage(int dam, int num);               // 매개변수 추가
void shop_choose();
void potion_shop();
void magic_shop();
void fight(int num, int reg);                // 매개변수 추가
void fight_pouse(int num, int reg);          // 매개변수 추가
void attack(int num, int reg);               // 매개변수 추가
void magic();
void stat_view();
void hp_out(int num, int reg);               // 매개변수 추가
void stat_set();
void slot_machine();
void help();
void cheat();

user user_data;
stat stat_data;
mob* mob_data;
mgs* mg_data;
hpo potions_data;
hpo potionm_data;
hpo potionb_data;

int finish;

/* 파일을 열고, 실패하면 경고 메시지를 출력한 뒤 NULL을 반환한다.
   호출부에서 NULL 여부를 확인해 복구 방식을 결정한다. */
FILE* open_or_warn(const char* path, const char* mode) {
	FILE* fp = fopen(path, mode);
	if (fp == NULL) {
		printf(":: [오류] 파일을 열 수 없습니다: %s ::\n", path);
	}
	return fp;
}

void cheat() {
	stat_data.point += 100;
	printf("스텟포인트 100 지급");
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
        if (menu_choice == -1) {
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
			return 0;
		case 9:
			clear();
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


	if (scanf("%d", &sel) != 1) {
        // 입력 오류 처리
        while (getchar() != '\n'); // 버퍼 정리
        return -1; // 오류 값 반환
    }
    while (getchar() != '\n'); // 버퍼 정리

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

	finish = 0;


	if (user_data.first == 0) {

		finish = 0;
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

	while (!feof(fp)) {
		mob_data = realloc(mob_data, sizeof(mob) * (i + 1));
		fscanf(fp, "%s %d %d %d %d %d ", mob_data[i].name, &mob_data[i].level, &mob_data[i].emin, &mob_data[i].emax, &mob_data[i].gmin, &mob_data[i].gmax);
		fscanf(fp, "%d %d %d %d %d %d\n", &mob_data[i].hp, &mob_data[i].maxhp, &mob_data[i].dam, &mob_data[i].pos, &mob_data[i].pom, &mob_data[i].pob);
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

	while (!feof(fp)) {
		mg_data = realloc(mg_data, sizeof(mgs) * (i + 1));
		fscanf(fp, "%s %d %d\n", mg_data[i].name, &mg_data[i].mp, &mg_data[i].dam);
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
			scanf("%d", &sel);
			printf("%d번 스텟을 선택하셨습니다.\n", sel);
			printf("얼마나 올리시겠습니까? : ");
			scanf("%d", &much);
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
	potions_data.healing = 20;
	potionm_data.healing = 100;
	potionb_data.healing = 500;
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

void save() {
	FILE* fp;

	fp = open_or_warn(DATA_PATH, "w");
	if (fp == NULL) return;

	fprintf(fp, "%d %s %d %d %d %d %d %d %d ", user_data.first, user_data.name, user_data.gold, user_data.exp, user_data.level, user_data.hp, user_data.maxhp, user_data.mp, user_data.maxmp);

	fprintf(fp, "%d %d %d %d %d %d ", potions_data.count, potionm_data.count, potionb_data.count, potions_data.healing, potionm_data.healing, potionb_data.healing);

	fclose(fp);

	fp = open_or_warn(STAT_PATH, "w");
	if (fp == NULL) return;

	fprintf(fp, "%d %d %d %f %d %d %d\n", stat_data.power, stat_data.speed, stat_data.def, stat_data.crit, stat_data.magic, stat_data.luck, stat_data.point);

	fclose(fp);

	printf("-------------------------------------\n\n");
	printf(":: 저장중,,,, ::\n");
	printf(":: 저장이 완료되었습니다 ::\n");
	printf("\n-------------------------------------\n");
	system("pause");
}

void inside_save() {
	FILE* fp;

	fp = open_or_warn(DATA_PATH, "w");
	if (fp == NULL) return;

	fprintf(fp, "%d %s %d %d %d %d %d %d %d ", user_data.first, user_data.name, user_data.gold, user_data.exp, user_data.level, user_data.hp, user_data.maxhp, user_data.mp, user_data.maxmp);

	fprintf(fp, "%d %d %d %d %d %d ", potions_data.count, potionm_data.count, potionb_data.count, potions_data.healing, potionm_data.healing, potionb_data.healing);

	fclose(fp);

	fp = open_or_warn(STAT_PATH, "w");
	if (fp == NULL) return;

	fprintf(fp, "%d %d %d %f %d %d %d\n", stat_data.power, stat_data.speed, stat_data.def, stat_data.crit, stat_data.magic, stat_data.luck, stat_data.point);

	fclose(fp);
}

void end() {
	save();
	printf("-------------------------------------\n\n");
	printf(":: 로그아웃 ::\n\n");
	printf("-------------------------------------\n");
	return;
}

void clear() {
	user user_data = { 0, "", 0, 0, 0, 0, 0, 0, 0 };

	hpo potions_data = { 0, 20 };

	hpo potionm_data = { 0, 100 };

	hpo potionb_data = { 0, 500 };

	stat stat_data = { 5, 5, 5, 0.1, 5, 1 , 0 };

	FILE* fp;

	fp = open_or_warn(DATA_PATH, "w");
	if (fp == NULL) return;

	fprintf(fp, "%d %s %d %d %d %d %d %d %d ", user_data.first, user_data.name, user_data.gold, user_data.exp, user_data.level, user_data.hp, user_data.maxhp, user_data.mp, user_data.maxmp);

	fprintf(fp, "%d %d %d %d %d %d ", potions_data.count, potionm_data.count, potionb_data.count, potions_data.healing, potionm_data.healing, potionb_data.healing);

	fclose(fp);

	fp = open_or_warn(STAT_PATH, "w");
	if (fp == NULL) return;

	fprintf(fp, "%d %d %d %f %d %d %d\n", stat_data.power, stat_data.speed, stat_data.def, stat_data.crit, stat_data.magic, stat_data.luck, stat_data.point);

	fclose(fp);

	printf("-------------------------------------\n\n");
	printf(":: 초기화 완료 ::\n\n");
	printf("-------------------------------------\n");

	return;

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
	scanf("%d", &num);
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
				monster(0, 1);
				break;
			}
			if (num > 5000 && num <= 8000) {
				monster(1, 1);
				break;
			}
			if (num > 8000 && num <= 9900) {
				monster(2, 1);
				break;
			}
			if (num > 9900) {
				monster(3, 1);
				break;
			}
			break;
		case 5:
			return;
		default:
			printf(":: 다시 입력해주세요 ::");
			break;
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

	FILE* fp;

	fp = open_or_warn(DATA_PATH, "w");
	if (fp == NULL) return;

	fprintf(fp, "%d %s %d %d %d %d %d ", user_data.first, user_data.name, user_data.gold, user_data.exp, user_data.level, user_data.hp, user_data.maxhp);

	fprintf(fp, "%d %d %d %d %d %d ", potions_data.count, potionm_data.count, potionb_data.count, potions_data.healing, potionm_data.healing, potionb_data.healing);

	fclose(fp);

	fp = open_or_warn(STAT_PATH, "w");
	if (fp == NULL) return;

	fprintf(fp, "%d %d %d %f %d %d %d\n", stat_data.power, stat_data.speed, stat_data.def, stat_data.crit, stat_data.magic, stat_data.luck, stat_data.point);

	fclose(fp);

}

void monster(int num, int region) {
	int sel, run;
	run = rand() % 100 + 1;
	printf("-------------------------------------\n");
	printf("\n:: %s(이)가 나타났다 ::\n\n", mob_data[num].name);
	printf("-------------------------------------\n");
	printf(":: 무엇을 하시겠습니까? ::\n");
	printf("\n1. 싸운다 , 2. 도망간다 , 3. 아이템사용 : ");
	scanf("%d", &sel);
	switch (sel) {
	case 1:
		fight(num, region);
		break;
	case 2:
		if (run >= 30) {
			printf("\n:: 무사히 도망쳤다 ::\n");
			inside_save();
			system("pause");
			system("cls");
			main();
		}
		else if (run < 30) {
			printf("\n:: 도망치지 못했다 ::\n");
			Sleep(500);
			fight(num, region);
		}
		break;
	case 3:
		potion_use();
		break;
	}
}

void damage(int dam, int num) {
	int hurt;
	hurt = dam;
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
		main();
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
		main();
	}
	else
		printf("-------------------------------------\n\n");
	printf(":: HP - %d ::\n", dam);
	printf("\n-------------------------------------\n");
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

void potion_use() {
	int sub;
	int sel;
	while (1) {
		sub = user_data.maxhp - user_data.hp;
		system("cls");
		printf("-------------------------------------\n\n");
		printf(":: 어떤 포션을 사용하시겠습니까? ::\n\n");
		printf("-------------------------------------\n");
		printf(":: HP포션(소) %d개 보유 ::\n", potions_data.count);
		printf(":: HP포션(중) %d개 보유 ::\n", potionm_data.count);
		printf(":: HP포션(대) %d개 보유 ::\n", potionb_data.count);
		printf("-------------------------------------\n\n");
		printf("1. HP포션(소) , 2. HP포션(중), 3. HP포션(대) 4. 돌아가기 : ");
		scanf("%d", &sel);
		switch (sel) {
		case 1:
			if (potions_data.count != 0) {
				if (sub < 20 && sub != 0) {
					user_data.hp += sub;
					potions_data.count--;
					printf("\n:: HP포션(소) 사용 ::\n");
					printf(":: HP %d이 회복되었습니다 ::\n", sub);
					printf(":: HP %d -> %d ::\n", user_data.hp - sub, user_data.hp);
					printf(":: HP포션(소) %d개 -> %d개 ::\n", potions_data.count + 1, potions_data.count);
					system("pause");
					break;
				}
				else if (sub >= 20) {
					user_data.hp += potions_data.healing;
					potions_data.count--;
					printf("\n:: HP포션(소) 사용 ::");
					printf(":: HP %d이 회복되었습니다 ::\n", potions_data.healing);
					printf(":: HP %d -> %d ::\n", user_data.hp - potions_data.healing, user_data.hp);
					printf(":: HP포션(소) %d개 -> %d개 ::\n", potions_data.count + 1, potions_data.count);
					system("pause");
					break;
				}
				else {
					printf(":: [system] 데미지를 입지 않아 회복할 수 없습니다. ::\n");
					system("pause");
					break;
				}
			}
			else if (potions_data.count == 0) {
				printf(":: [system] HP포션(소)을 보유하고있지 않습니다. ::\n");
				system("pause");
				break;
			}
		case 2:
			if (potionm_data.count != 0) {
				if (sub < 100 && sub != 0) {
					user_data.hp += sub;
					potionm_data.count--;
					printf("\n:: HP포션(중) 사용 ::\n");
					printf(":: HP %d이 회복되었습니다 ::\n", sub);
					printf(":: HP %d -> %d ::\n", user_data.hp - sub, user_data.hp);
					printf(":: HP포션(중) %d개 -> %d개 ::\n", potionm_data.count + 1, potionm_data.count);
					system("pause");
					break;
				}
				else if (sub >= 100) {
					user_data.hp += potionm_data.healing;
					potionm_data.count--;
					printf("\n:: HP포션(중) 사용 ::");
					printf(":: HP %d이 회복되었습니다 ::\n", potionm_data.healing);
					printf(":: HP %d -> %d ::\n", user_data.hp - potionm_data.healing, user_data.hp);
					printf(":: HP포션(중) %d개 -> %d개 ::\n", potionm_data.count + 1, potionm_data.count);
					system("pause");
					break;
				}
				else {
					printf(":: [system] 데미지를 입지 않아 회복할 수 없습니다. ::\n");
					system("pause");
					break;
				}
			}
			else if (potionm_data.count == 0) {
				printf(":: [system] HP포션(중)을 보유하고있지 않습니다. ::\n");
				system("pause");
				break;
			}
		case 3:
			if (potionb_data.count != 0) {
				if (sub < 500 && sub != 0) {
					user_data.hp += sub;
					potionb_data.count--;
					printf("\n:: HP포션(대) 사용 ::\n");
					printf(":: HP %d이 회복되었습니다 ::\n", sub);
					printf(":: HP %d -> %d ::\n", user_data.hp - sub, user_data.hp);
					printf(":: HP포션(대) %d개 -> %d개 ::\n", potionb_data.count + 1, potionb_data.count);
					system("pause");
					break;
				}
				else if (sub >= 500) {
					user_data.hp += potionb_data.healing;
					potionb_data.count--;
					printf("\n:: HP포션(대) 사용 ::");
					printf(":: HP %d이 회복되었습니다 ::\n", potionb_data.healing);
					printf(":: HP %d -> %d ::\n", user_data.hp - potionb_data.healing, user_data.hp);
					printf(":: HP포션(대) %d개 -> %d개 ::\n", potionb_data.count + 1, potionb_data.count);
					system("pause");
					break;
				}
				else {
					printf(":: [system] 데미지를 입지 않아 회복할 수 없습니다. ::\n");
					system("pause");
					break;
				}
			}
			else if (potionb_data.count == 0) {
				printf(":: [system] HP포션(대)를 보유하고있지 않습니다. ::\n");
				system("pause");
				break;
			}
		case 4:
			printf(":: 돌아갑니다. ::\n");
			save();
			adventure_control();
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
	menu();
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
		scanf("%d", &sel);

		switch (sel) {
		case 1:
			potion_shop();
			break;
		case 2:
			//magic_shop();
			break;
		case 3:
			system("cls");
			main();
			break;
		default:
			printf(":: 다시 입력해주세요 ::");
			break;
		}
	}
}

void potion_shop() {
	int sel;
	while (1) {
		system("cls");
		printf("-------------------------------------\n\n");
		printf(":: 물약상점 ::       골드 보유량  : %d Gold\n", user_data.gold);
		printf("\n-------------------------------------\n\n");
		printf("1. HP포션(소) : 100 Gold\n");
		printf("2. HP포션(중) : 500 Gold\n");
		printf("3. HP포션(대) : 1000 Gold\n");
		printf("4. 돌아가기\n");
		printf("\n-------------------------------------\n\n");
		printf("무엇을 구매하시겠습니까? : ");
		scanf("%d", &sel);

		switch (sel) {
		case 1:
			if (user_data.gold >= 100) {
				potions_data.count += 1;
				user_data.gold -= 100;
				printf("-------------------------------------\n\n");
				printf(":: HP포션(소) 구매완료 ::\n");
				printf(":: 현재 보유 : %d개 -> %d개 ::\n", potions_data.count - 1, potions_data.count);
				printf("\n-------------------------------------\n\n");
				system("pause");
				break;
			}
			else if (user_data.gold < 100) {
				printf("-------------------------------------\n\n");
				printf(":: 골드가 부족합니다 ::\n");
				printf("\n-------------------------------------\n\n");
				system("pause");
				break;
			}
		case 2:
			if (user_data.gold >= 500) {
				potionm_data.count += 1;
				user_data.gold -= 500;
				printf("-------------------------------------\n\n");
				printf(":: HP포션(중) 구매완료 ::\n");
				printf(":: 현재 보유 : %d개 -> %d개 ::\n", potionm_data.count - 1, potionm_data.count);
				printf("\n-------------------------------------\n\n");
				system("pause");
				break;
			}
			else if (user_data.gold < 500) {
				printf("-------------------------------------\n\n");
				printf(":: 골드가 부족합니다 ::\n");
				printf("\n-------------------------------------\n\n");
				system("pause");
				break;
			}
		case 3:
			if (user_data.gold >= 1000) {
				potionb_data.count += 1;
				user_data.gold -= 1000;
				printf("-------------------------------------\n\n");
				printf(":: HP포션(대) 구매완료 ::\n");
				printf(":: 현재 보유 : %d개 -> %d개 ::\n", potionb_data.count - 1, potionb_data.count);
				printf("\n-------------------------------------\n\n");
				system("pause");
				break;
			}
			else if (user_data.gold < 1000) {
				printf("-------------------------------------\n\n");
				printf(":: 골드가 부족합니다 ::\n");
				printf("\n-------------------------------------\n\n");
				system("pause");
				break;
			}
		case 4:
			printf("-------------------------------------\n\n");
			printf(":: 돌아갑니다 ::\n");
			printf("\n-------------------------------------\n");
			save();
			system("cls");
			main();
			break;
		}

	}


}

/*void magic_shop() {

}*/

void fight(int num, int reg) {
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
		scanf("%d", &sel);
		switch (sel) {
		case 1:
			attack(num, reg);
			break;
		case 2:
			//magic();
			break;
		case 3:
			fight_pouse(num, reg);
		}
		hp_out(num, reg);
	}

}

void attack(int num, int reg) {
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
	hp_out(num, reg);
	Sleep(500);
	printf("------------------------------------------\n");
	printf("\n:: %s의 공격! ::\n\n", mob_data[num].name);
	Sleep(500);
	damage(dam, num);
	system("pause");

}

/*void magic(int num, int reg) {

}
*/

void fight_pouse(int num, int reg) {
	int sub;
	int sel;
	while (1) {
		sub = user_data.maxhp - user_data.hp;
		system("cls");
		printf("-------------------------------------\n\n");
		printf(":: 어떤 포션을 사용하시겠습니까? ::\n\n");
		printf("-------------------------------------\n");
		printf(":: HP포션(소) %d개 보유 ::\n", potions_data.count);
		printf(":: HP포션(중) %d개 보유 ::\n", potionm_data.count);
		printf(":: HP포션(대) %d개 보유 ::\n", potionb_data.count);
		printf("-------------------------------------\n\n");
		printf("1. HP포션(소) , 2. HP포션(중), 3. HP포션(대) 4. 돌아가기 : ");
		scanf("%d", &sel);
		switch (sel) {
		case 1:
			if (potions_data.count != 0) {
				if (sub < 20 && sub != 0) {
					user_data.hp += sub;
					potions_data.count--;
					printf("\n:: HP포션(소) 사용 ::\n");
					printf(":: HP %d이 회복되었습니다 ::\n", sub);
					printf(":: HP %d -> %d ::\n", user_data.hp - sub, user_data.hp);
					printf(":: HP포션(소) %d개 -> %d개 ::\n", potions_data.count + 1, potions_data.count);
					system("pause");
					break;
				}
				else if (sub >= 20) {
					user_data.hp += potions_data.healing;
					potions_data.count--;
					printf("\n:: HP포션(소) 사용 ::");
					printf(":: HP %d이 회복되었습니다 ::\n", potions_data.healing);
					printf(":: HP %d -> %d ::\n", user_data.hp - potions_data.healing, user_data.hp);
					printf(":: HP포션(소) %d개 -> %d개 ::\n", potions_data.count + 1, potions_data.count);
					system("pause");
					break;
				}
				else {
					printf(":: [system] 데미지를 입지 않아 회복할 수 없습니다. ::\n");
					system("pause");
					break;
				}
			}
			else if (potions_data.count == 0) {
				printf(":: [system] HP포션(소)을 보유하고있지 않습니다. ::\n");
				system("pause");
				break;
			}
		case 2:
			if (potionm_data.count != 0) {
				if (sub < 100 && sub != 0) {
					user_data.hp += sub;
					potionm_data.count--;
					printf("\n:: HP포션(중) 사용 ::\n");
					printf(":: HP %d이 회복되었습니다 ::\n", sub);
					printf(":: HP %d -> %d ::\n", user_data.hp - sub, user_data.hp);
					printf(":: HP포션(중) %d개 -> %d개 ::\n", potionm_data.count + 1, potionm_data.count);
					system("pause");
					break;
				}
				else if (sub >= 100) {
					user_data.hp += potionm_data.healing;
					potionm_data.count--;
					printf("\n:: HP포션(중) 사용 ::");
					printf(":: HP %d이 회복되었습니다 ::\n", potionm_data.healing);
					printf(":: HP %d -> %d ::\n", user_data.hp - potionm_data.healing, user_data.hp);
					printf(":: HP포션(중) %d개 -> %d개 ::\n", potionm_data.count + 1, potionm_data.count);
					system("pause");
					break;
				}
				else {
					printf(":: [system] 데미지를 입지 않아 회복할 수 없습니다. ::\n");
					system("pause");
					break;
				}
			}
			else if (potionm_data.count == 0) {
				printf(":: [system] HP포션(중)을 보유하고있지 않습니다. ::\n");
				system("pause");
				break;
			}
		case 3:
			if (potionb_data.count != 0) {
				if (sub < 500 && sub != 0) {
					user_data.hp += sub;
					potionb_data.count--;
					printf("\n:: HP포션(대) 사용 ::\n");
					printf(":: HP %d이 회복되었습니다 ::\n", sub);
					printf(":: HP %d -> %d ::\n", user_data.hp - sub, user_data.hp);
					printf(":: HP포션(대) %d개 -> %d개 ::\n", potionb_data.count + 1, potionb_data.count);
					system("pause");
					break;
				}
				else if (sub >= 500) {
					user_data.hp += potionb_data.healing;
					potionb_data.count--;
					printf("\n:: HP포션(대) 사용 ::");
					printf(":: HP %d이 회복되었습니다 ::\n", potionb_data.healing);
					printf(":: HP %d -> %d ::\n", user_data.hp - potionb_data.healing, user_data.hp);
					printf(":: HP포션(대) %d개 -> %d개 ::\n", potionb_data.count + 1, potionb_data.count);
					system("pause");
					break;
				}
				else {
					printf(":: [system] 데미지를 입지 않아 회복할 수 없습니다. ::\n");
					system("pause");
					break;
				}
			}
			else if (potionb_data.count == 0) {
				printf(":: [system] HP포션(대)를 보유하고있지 않습니다. ::\n");
				system("pause");
				break;
			}
		case 4:
			printf(":: 돌아갑니다. ::\n");
			save();
			fight(num, reg);
		}

	}
}

void hp_out(int num, int reg) {

	if (mob_data[num].hp <= 0) {
		printf("\n:: %s을 죽였습니다 ::\n", mob_data[num].name);
		auto_save(mob_data[num].emin, mob_data[num].emax, mob_data[num].gmin, mob_data[num].gmax);
		potion_earn(mob_data[num].pos, mob_data[num].pom, mob_data[num].pob);
		system("pause");
		mob_data[num].hp = mob_data[num].maxhp;
		system("cls");
		adventure(reg);
	}
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
		main();
	}
	else if (user_data.gold > 0) {
		printf("얼마를 넣으시겠습니까? - %d Gold 보유 : ", user_data.gold);
		scanf("%d", &much);
		if (much == 0) {
			printf(":: 더 많은 금액을 입력해주십시오 ::\n");
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
				main();
			}
			else {
				Sleep(1000);
				printf("★      ★       ◆\n\n");
				Sleep(50);
				printf(":: 당첨되지 않으셨습니다 ::\n");
				inside_save();
				system("pause");
				system("cls");
				main();
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
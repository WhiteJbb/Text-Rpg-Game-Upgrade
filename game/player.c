#include "game.h"


/* 착용 장비 보너스를 합산한 실효 스텟 (미착용 슬롯은 -1 이라 건너뜀) */
int total_power() {
	int v = stat_data.power;
	if (user_data.weapon >= 0) v += eq_data[user_data.weapon].power;
	if (user_data.armor >= 0)  v += eq_data[user_data.armor].power;
	return v;
}
int total_def() {
	int v = stat_data.def;
	if (user_data.weapon >= 0) v += eq_data[user_data.weapon].def;
	if (user_data.armor >= 0)  v += eq_data[user_data.armor].def;
	return v;
}
int total_magic() {
	int v = stat_data.magic;
	if (user_data.weapon >= 0) v += eq_data[user_data.weapon].magic;
	if (user_data.armor >= 0)  v += eq_data[user_data.armor].magic;
	return v;
}


void stat_view() {
	clear_screen();
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
	printf(":: 무기   : %s ::\n", user_data.weapon >= 0 ? eq_data[user_data.weapon].name : "없음");
	printf(":: 방어구 : %s ::\n", user_data.armor >= 0 ? eq_data[user_data.armor].name : "없음");
	printf(":: (장비 포함) 힘 %d / 방어 %d / 지력 %d ::\n", total_power(), total_def(), total_magic());
	printf("-------------------------------------\n");
	pause_screen();
}


void stat_set() {
	int sel = 0;
	int much = 0;
	while (1) {
		clear_screen();
		printf("-------------------------------------\n\n");
		printf(":: 스텟 설정 ::   - 남은 스텟 포인트 : %d\n", stat_data.point);
		printf("\n-------------------------------------\n\n");
		printf("1. 힘        2. 민첩        3.방어\n\n");
		printf("4. 지력      5. 크리티컬    6.운\n");
		printf("\n-------------------------------------\n");
		if (stat_data.point == 0) {
			sleep_ms(200);
			printf(":: 투자할 스텟 포인트가 없습니다 ::\n");
			pause_screen();
			clear_screen();
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
				pause_screen();
				clear_screen();
				continue;
			}
			switch (sel) {
			case 1:
				printf("\n-------------------------------------\n\n");
				printf(":: 열심히 운동하는 중입니다..... ::\n");
				printf("\n-------------------------------------\n\n");
				sleep_ms(1000);
				printf(":: 힘 + %d ::\n", much);
				printf("\n-------------------------------------\n");
				stat_data.power += much;
				stat_data.point -= much;
				inside_save();
				pause_screen();
				clear_screen();
				return;
				break;
			case 2:
				printf("\n-------------------------------------\n\n");
				printf(":: 더욱 빨라지는 중입니다..... ::\n");
				printf("\n-------------------------------------\n\n");
				sleep_ms(1000);
				printf(":: 민첩 + %d ::\n", much);
				printf("\n-------------------------------------\n");
				stat_data.speed += much;
				stat_data.point -= much;
				inside_save();
				pause_screen();
				clear_screen();
				return;
				break;
			case 3:
				printf("\n-------------------------------------\n\n");
				printf(":: 몸이 단단해지는 중입니다..... ::\n");
				printf("\n-------------------------------------\n\n");
				sleep_ms(1000);
				printf(":: 방어 + %d ::\n", much);
				printf("\n-------------------------------------\n");
				stat_data.def += much;
				stat_data.point -= much;
				inside_save();
				pause_screen();
				clear_screen();
				return;
				break;
			case 4:
				printf("\n-------------------------------------\n\n");
				printf(":: 마법에 대한 이해가 늘어나는 중입니다..... ::\n");
				printf("\n-------------------------------------\n\n");
				sleep_ms(1000);
				printf(":: 지력 + %d ::\n", much);
				printf("\n-------------------------------------\n");
				stat_data.magic += much;
				stat_data.point -= much;
				inside_save();
				pause_screen();
				clear_screen();
				return;
				break;
			case 5:
				printf("\n-------------------------------------\n\n");
				printf(":: 급소를 더 잘 찌르는법을 배우는 중입니다..... ::\n");
				printf("\n-------------------------------------\n\n");
				sleep_ms(1000);
				printf(":: 크리티컬 + %.1f%% ::\n", much * 0.5);
				printf("\n-------------------------------------\n");
				stat_data.crit += much * 0.5;
				stat_data.point -= much;
				inside_save();
				pause_screen();
				clear_screen();
				return;
				break;
			case 6:
				printf("\n-------------------------------------\n\n");
				printf(":: 네잎클로버를 찾는 중입니다..... ::\n");
				printf("\n-------------------------------------\n\n");
				sleep_ms(1000);
				printf(":: 운 + %d ::\n", much);
				printf("\n-------------------------------------\n");
				stat_data.luck += much;
				stat_data.point -= much;
				inside_save();
				pause_screen();
				clear_screen();
				return;
				break;
			default:
				printf(":: 다시 선택해주세요 ::\n");
				pause_screen();
				break;
			}

		}

	}

}


void level() {
	clear_screen();
	printf("-------------------------------------\n");
	printf(":: 닉네임 : %s ::\n", user_data.name);
	printf(":: HP : %d / %d ::\n", user_data.hp, user_data.maxhp);
	printf(":: MP : %d / %d ::\n", user_data.mp, user_data.maxmp);
	printf(":: 골드 : %dGold ::\n", user_data.gold);
	printf(":: 경험치 : %dexp ::\n", user_data.exp);
	printf("::  레벨  : %d ::\n", user_data.level);
	printf("==> %d레벨까지 %dexp\n", user_data.level + 1, LEVELUP_EXP(user_data.level) - user_data.exp);
	printf("-------------------------------------\n");
	pause_screen();
}


void level_up() {
	if (user_data.exp >= LEVELUP_EXP(user_data.level)) {
		user_data.level += 1;
		printf("-------------------------------------\n");
		printf("\n:: 레벨업! :: 현재 레벨 : %d\n", user_data.level);
		user_data.maxhp += user_data.level * 10;
		printf(":: 최대 HP  + %d ::\n", user_data.level * 10);
		user_data.hp = user_data.maxhp;
		printf(":: HP가 회복되었습니다 ::\n");
		user_data.maxmp += user_data.level * 5;
		user_data.mp = user_data.maxmp;
		printf(":: 최대 MP + %d, MP가 회복되었습니다 ::\n", user_data.level * 5);
		printf(":: 스텟 포인트 + 7 ::\n\n");
		stat_data.point += 7;
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


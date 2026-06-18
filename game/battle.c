#include "game.h"


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


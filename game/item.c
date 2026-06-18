#include "game.h"


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
	for (int i = 0; i < spell_count; i++) {
		if (mg_data[i].learned)
			printf(":: %s (MP %d, 피해 %d+지력) 보유 ::\n", mg_data[i].name, mg_data[i].mp, mg_data[i].dam);
	}
	printf("-------------------------------------\n");
	system("pause");
	system("cls");
}


/* 마법상점: 보유하지 않은 마법을 골드로 학습한다. (스펠 정의는 magicspell.txt) */
void magic_shop() {
	int sel, i;
	while (1) {
		system("cls");
		printf("-------------------------------------\n\n");
		printf(":: 마법상점 ::       골드 보유량 : %d Gold\n", user_data.gold);
		printf("\n-------------------------------------\n");
		for (i = 0; i < spell_count; i++) {
			if (mg_data[i].learned)
				printf("%d. %s  - 보유중  (MP %d, 피해 %d)\n", i + 1, mg_data[i].name, mg_data[i].mp, mg_data[i].dam);
			else
				printf("%d. %s  - %d Gold  (MP %d, 피해 %d)\n", i + 1, mg_data[i].name, mg_data[i].price, mg_data[i].mp, mg_data[i].dam);
		}
		printf("0. 돌아가기\n");
		printf("\n-------------------------------------\n\n");
		printf("어떤 마법을 배우시겠습니까? : ");
		sel = read_int();
		if (sel == 0) {
			save();
			system("cls");
			return;
		}
		if (sel < 1 || sel > spell_count) {
			printf(":: 다시 입력해주세요 ::\n");
			system("pause");
			continue;
		}
		mgs* spell = &mg_data[sel - 1];
		printf("-------------------------------------\n\n");
		if (spell->learned) {
			printf(":: 이미 보유한 마법입니다 ::\n");
		}
		else if (user_data.gold >= spell->price) {
			user_data.gold -= spell->price;
			spell->learned = 1;
			user_data.spells |= (1 << (sel - 1));
			printf(":: %s 습득 완료! ::\n", spell->name);
		}
		else {
			printf(":: 골드가 부족합니다 ::\n");
		}
		printf("\n-------------------------------------\n\n");
		system("pause");
	}
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
			magic_shop();
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


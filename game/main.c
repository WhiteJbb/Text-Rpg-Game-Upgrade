#include "game.h"

user user_data;
stat stat_data;
mob* mob_data;
mgs* mg_data;
hpo potions_data;
hpo potionm_data;
hpo potionb_data;
int spell_count;
int mob_count;
equip* eq_data;
int equip_count;


int main() {
	init_console();   /* 콘솔 UTF-8 설정 (플랫폼별 래퍼) */
	srand((int)time(NULL));
	user_load();
	mob_load();
	stat_load();
	magic_load();
	equip_load();
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
		clear_screen();

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


void slot_machine() {
	int i = 0;
	int much = 0;;
	int percent = 0;
	percent = rand() % 10000 + 1;
	while (i < 6) {
		clear_screen();
		printf("■□■□■□■□■□■□■□■□■□■□■□■□\n\n");
		printf(":: 슬롯머신 :: - 넣은 돈을 2배 불려드립니다\n\n");
		printf("■□■□■□■□■□■□■□■□■□■□■□■□\n\n");
		sleep_ms(100);
		clear_screen();
		printf("□■□■□■□■□■□■□■□■□■□■□■□■\n\n");
		printf(":: 슬롯머신 :: - 넣은 돈을 2배 불려드립니다\n\n");
		printf("□■□■□■□■□■□■□■□■□■□■□■□■\n\n");
		sleep_ms(100);
		i++;
	}
	if (user_data.gold == 0) {
		printf(":: 보유하고 계신 Gold가 없습니다 ::\n");
		pause_screen();
		clear_screen();
		return;
	}
	else if (user_data.gold > 0) {
		printf("얼마를 넣으시겠습니까? - %d Gold 보유 : ", user_data.gold);
		much = read_int();
		if (much <= 0) {
			printf(":: 올바른 금액을 입력해주십시오 ::\n");
			pause_screen();
		}
		else if (much > user_data.gold) {
			printf(":: 보유 골드보다 많이 걸 수 없습니다 ::\n");
			pause_screen();
		}
		else {
			printf("\n\n:: - %d Gold ::\n\n", much);
			printf(":: 슬롯머신을 가동합니다 ::\n\n");
			user_data.gold -= much;
			sleep_ms(250);
			printf("★      ●       ◆\n\n");
			sleep_ms(250);
			printf("★      ◆       ●\n\n");
			sleep_ms(250);
			printf("◆      ●       ★\n\n");
			sleep_ms(250);
			printf("●      ★       ◆\n\n");
			sleep_ms(250);
			printf("●      ◆       ★\n\n");
			if (percent < 50 + (stat_data.luck * 50)) {
				sleep_ms(1000);
				printf("★       ★       ★\n\n");
				sleep_ms(100);
				printf(":: 당첨!!!! ::\n");
				printf(":: 축하드립니다 ::\n\n");
				printf("------------------------------------------\n\n");
				printf(":: + %d Gold ::\n\n", much * 2);
				printf("------------------------------------------\n");
				user_data.gold += much * 2;
				inside_save();
				pause_screen();
				clear_screen();
				return;
			}
			else {
				sleep_ms(1000);
				printf("★      ★       ◆\n\n");
				sleep_ms(50);
				printf(":: 당첨되지 않으셨습니다 ::\n");
				inside_save();
				pause_screen();
				clear_screen();
				return;
			}
		}
	}
}


void help() {
	clear_screen();
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
	printf("\n[전투 팁]\n");
	printf("   - 공격 : 크리티컬 스텟 확률로 2배 피해\n");
	printf("   - 마법 : 파이어볼 (MP 소모, 피해 = 기본 + 지력)\n");
	printf("   - 민첩 : 적의 공격을 회피할 확률\n");
	printf("\n----------------------------------------\n");
	pause_screen();
}


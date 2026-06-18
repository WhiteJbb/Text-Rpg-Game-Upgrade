#include "game.h"


void adventure_control() {
	clear_screen();
	int num;
	printf("-------------------------------------\n\n");
	printf("1. 몬스터의 초원 (Lv.1 ~)\n");
	printf("2. 늑대의 굴     (Lv.5 ~)\n");
	printf("3. 엘프의 숲     (Lv.15 ~)\n");
	printf("4. 뱀파이어의 성 (Lv.30 ~)\n");
	printf("5. 메뉴\n");
	printf("\n-------------------------------------\n");
	printf("어느지역으로 가시겠습니까? : ");
	num = read_int();
	clear_screen();
	adventure(num);
}


void adventure(int region) {
	/* 지역(1~4)별 정보. 지역당 몬스터 4종(일반3 + 보스1)을 mob_data에서 연속 배치. */
	static const char* names[] = { "", "몬스터의 초원", "늑대의 굴", "엘프의 숲", "뱀파이어의 성" };
	static const char* flavor[] = { "",
		"곳곳에서 섬뜩한 눈빛이 느껴집니다...",
		"축축한 굴 속에서 으르렁거리는 소리가 들립니다...",
		"고요한 숲에 정령들의 기척이 감돕니다...",
		"차가운 성 안에서 피냄새가 진동합니다..." };
	static const int min_level[] = { 0, 1, 5, 15, 30 };

	if (region == 5)
		return;                       /* 메뉴로 복귀 */
	if (region < 1 || region > 4) {
		printf(":: 잘못된 지역입니다 ::\n");
		pause_screen();
		return;
	}

	int base = (region - 1) * 4;      /* 이 지역의 첫 몬스터 인덱스 */
	if (base + 3 >= mob_count) {
		printf(":: 아직 준비되지 않은 지역입니다 ::\n");
		pause_screen();
		return;
	}
	if (user_data.level < min_level[region]) {
		printf(":: '%s'은(는) 권장 레벨 %d 이상부터 입장할 수 있습니다. (현재 Lv.%d) ::\n",
			names[region], min_level[region], user_data.level);
		pause_screen();
		return;
	}

	while (1) {
		int num = rand() % 10000 + 1;
		int idx;
		if (num <= 5000)       idx = base + 0;
		else if (num <= 8000)  idx = base + 1;
		else if (num <= 9900)  idx = base + 2;
		else                   idx = base + 3;   /* 1% 확률 보스 */

		printf("-------------------------------------\n");
		printf(":: %s님의 HP : %d / %d ::\n", user_data.name, user_data.hp, user_data.maxhp);
		printf("-------------------------------------\n");
		printf(":: %s ::\n", names[region]);
		printf(":: %s ::\n", flavor[region]);

		if (monster(idx) == COMBAT_TO_MENU)
			return;
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
				pause_screen();
				clear_screen();
				return COMBAT_TO_MENU;
			}
			else {
				printf("\n:: 도망치지 못했다 ::\n");
				sleep_ms(500);
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
		clear_screen();
		printf("-------------------------------------\n\n");
		printf(":: 사망하셨습니다 ::\n");
		printf(":: 레벨 - 1 ::\n\n");
		user_data.maxhp -= user_data.level * 10;
		user_data.level -= 1;
		user_data.exp = LEVELUP_EXP(user_data.level - 1) + 1;
		user_data.hp = user_data.maxhp;
		save();
		mob_data[num].hp = mob_data[num].maxhp;
		printf("-------------------------------------\n\n");
		printf(":: System :: 메인화면으로 돌아갑니다 \n");
		printf("\n-------------------------------------\n");
		pause_screen();
		clear_screen();
		return 1;
	}
	else if (user_data.hp <= 0 && user_data.level == 1) {
		clear_screen();
		printf("-------------------------------------\n\n");
		printf(":: 사망하셨습니다 ::\n");
		printf(":: 레벨이 1이므로 페널티를 받지 않습니다 ::\n\n");
		user_data.hp = user_data.maxhp;
		save();
		mob_data[num].hp = mob_data[num].maxhp;
		printf("-------------------------------------\n\n");
		printf(":: System :: 메인화면으로 돌아갑니다 \n");
		printf("\n-------------------------------------\n");
		pause_screen();
		clear_screen();
		return 1;
	}
	printf("-------------------------------------\n\n");
	printf(":: HP - %d ::\n", dam);
	printf("\n-------------------------------------\n");
	return 0;
}


int fight(int num) {
	int sel = 0;

	while (1) {
		clear_screen();
		printf("------------------------------------------\n\n");
		printf(":: Fight!! ::");
		printf("\n\n------------------------------------------");
		printf("\n\n:: Lv.%d ::\n:: %s :: \n:: HP : %d / %d ::", mob_data[num].level, mob_data[num].name, mob_data[num].hp, mob_data[num].maxhp);
		printf("\n\n     VS              \n\n");
		printf(":: Lv.%d :: \n:: %s :: \n:: HP : %d / %d ::\n:: MP : %d / %d ::\n\n", user_data.level, user_data.name, user_data.hp, user_data.maxhp, user_data.mp, user_data.maxmp);
		printf("------------------------------------------\n");
		printf("1. 공격, 2. 마법, 3. 아이템사용 : ");
		sel = read_int();
		int result = COMBAT_ONGOING;
		switch (sel) {
		case 1:
			result = attack(num);
			break;
		case 2:
			result = magic(num);
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
	printf("------------------------------------------\n");
	printf("\n:: %s의 공격! ::\n\n", user_data.name);
	printf("------------------------------------------\n\n");
	sleep_ms(500);

	/* 치명타: stat_data.crit 는 퍼센트(예: 0.5%). rand()%10000 으로 소수 1자리까지 판정 */
	int pdam = stat_data.power;
	if ((rand() % 10000) < (int)(stat_data.crit * 100)) {
		pdam *= 2;
		printf(":: 치명타!! ::\n");
	}
	int dealt = (mob_data[num].hp < pdam) ? mob_data[num].hp : pdam;
	mob_data[num].hp -= dealt;
	printf(":: ""%s"":에게 %d의 피해를 주었다. ::\n\n", mob_data[num].name, dealt);
	printf("------------------------------------------\n");

	if (hp_out(num))
		return COMBAT_CONTINUE;   /* 몬스터 처치 -> 반격 생략, 탐험 계속 */
	return monster_counterattack(num);
}

/* 몬스터의 반격: 민첩(speed%) 회피 판정 후 피해 적용.
   플레이어 사망 시 COMBAT_TO_MENU, 아니면 COMBAT_ONGOING. (attack/magic 공용) */
int monster_counterattack(int num) {
	int dam = mob_data[num].dam - stat_data.def;
	if (dam < 0)
		dam = 0;
	sleep_ms(500);
	printf("------------------------------------------\n");
	printf("\n:: %s의 공격! ::\n\n", mob_data[num].name);
	sleep_ms(500);
	if ((rand() % 100) < stat_data.speed) {
		printf(":: 민첩하게 공격을 회피했다! ::\n");
		printf("\n-------------------------------------\n");
		pause_screen();
		return COMBAT_ONGOING;
	}
	if (damage(dam, num))
		return COMBAT_TO_MENU;    /* 플레이어 사망 -> 메뉴 복귀 */
	pause_screen();
	return COMBAT_ONGOING;
}

/* 보유한 마법 중 하나를 선택해 시전. 피해 = 기본피해(dam) + 지력(magic), MP 소모.
   취소/MP부족/미보유 시 턴을 소비하지 않고 전투를 계속한다. */
int magic(int num) {
	int i;
	clear_screen();
	printf("------------------------------------------\n");
	printf(":: 마법 선택 ::   MP %d / %d\n", user_data.mp, user_data.maxmp);
	printf("------------------------------------------\n");
	for (i = 0; i < spell_count; i++) {
		if (mg_data[i].learned)
			printf("%d. %s  (MP %d, 피해 %d+지력)\n", i + 1, mg_data[i].name, mg_data[i].mp, mg_data[i].dam);
	}
	printf("0. 취소\n");
	printf("------------------------------------------\n");
	printf("시전할 마법 : ");
	int sel = read_int();
	if (sel == 0)
		return COMBAT_ONGOING;
	if (sel < 1 || sel > spell_count || !mg_data[sel - 1].learned) {
		printf(":: 보유하지 않은 마법입니다 ::\n");
		pause_screen();
		return COMBAT_ONGOING;
	}

	mgs* spell = &mg_data[sel - 1];
	if (user_data.mp < spell->mp) {
		printf("\n:: MP가 부족합니다 (필요 %d / 보유 %d) ::\n", spell->mp, user_data.mp);
		pause_screen();
		return COMBAT_ONGOING;
	}
	user_data.mp -= spell->mp;
	int mdam = spell->dam + stat_data.magic;

	printf("------------------------------------------\n");
	printf("\n:: %s 시전! ::\n\n", spell->name);
	printf("------------------------------------------\n\n");
	sleep_ms(500);
	int dealt = (mob_data[num].hp < mdam) ? mob_data[num].hp : mdam;
	mob_data[num].hp -= dealt;
	printf(":: ""%s"":에게 %d의 마법 피해! (MP -%d) ::\n\n", mob_data[num].name, dealt, spell->mp);
	printf("------------------------------------------\n");

	if (hp_out(num))
		return COMBAT_CONTINUE;
	return monster_counterattack(num);
}

int hp_out(int num) {

	if (mob_data[num].hp <= 0) {
		printf("\n:: %s을 죽였습니다 ::\n", mob_data[num].name);
		auto_save(mob_data[num].emin, mob_data[num].emax, mob_data[num].gmin, mob_data[num].gmax);
		potion_earn(mob_data[num].pos, mob_data[num].pom, mob_data[num].pob);
		pause_screen();
		mob_data[num].hp = mob_data[num].maxhp;
		clear_screen();
		return 1;
	}
	return 0;
}


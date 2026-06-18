#include "game.h"


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


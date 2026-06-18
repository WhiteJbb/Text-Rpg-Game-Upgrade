#include "game.h"

/* ===== 내장 기본 데이터 (단일 exe 배포용) =====
   첫 실행 시 test/ 에 파일이 없으면 아래 내용으로 생성한다.
   파일이 이미 있으면 그대로 사용하므로 txt를 고쳐 데이터를 바꿀 수 있다. */
static const char* EMBED_MONSTERS =
"슬라임 1 10 1 5 1 10 10 5 5 0 0\n"
"고블린 3 30 11 20 11 20 20 7 20 0 0\n"
"코볼트 5 50 21 40 21 30 30 10 100 10 0\n"
"[BOSS]오우거 10 50 101 100 201 50 50 30 2000 1000 100\n"
"들개 6 60 30 30 30 60 60 15 200 20 0\n"
"늑대 8 90 50 50 50 90 90 22 300 50 0\n"
"알파늑대 12 150 80 80 80 130 130 35 500 100 5\n"
"[BOSS]웨어울프 18 300 300 300 500 200 200 70 3000 1500 200\n"
"픽시 16 200 100 100 100 200 200 45 300 100 10\n"
"다크엘프 20 300 200 200 200 300 300 65 500 200 20\n"
"트렌트 25 450 350 350 350 450 450 90 800 300 50\n"
"[BOSS]엘프여왕 32 700 800 800 1000 600 600 130 4000 2000 500\n"
"박쥐 32 400 300 300 300 400 400 100 400 200 50\n"
"좀비 38 600 500 500 500 600 600 130 600 300 100\n"
"가고일 45 900 800 800 800 900 900 170 1000 500 200\n"
"[BOSS]뱀파이어로드 55 1500 2000 2000 3000 1200 1200 250 5000 3000 1000\n";

static const char* EMBED_MAGIC =
"파이어볼 50 10 0\n"
"아이스스피어 30 8 500\n"
"윈드커터 20 5 300\n"
"라이트닝볼트 60 18 1500\n"
"메테오 100 40 5000\n";

static const char* EMBED_EQUIP =
"나무막대기 0 3 0 0 50\n"
"청동검 0 8 0 0 300\n"
"강철검 0 18 0 2 1200\n"
"미스릴소드 0 35 0 5 4000\n"
"마법지팡이 0 5 0 15 2500\n"
"천옷 1 0 3 0 50\n"
"가죽갑옷 1 0 8 0 300\n"
"판금갑옷 1 0 20 0 1500\n"
"미스릴갑옷 1 0 40 3 5000\n"
"마법로브 1 0 10 10 3000\n";

/* 파일이 없으면 내장 문자열로 생성 (있으면 그대로 둔다) */
static void write_if_missing(const char* path, const char* content) {
	FILE* fp = fopen(path, "r");
	if (fp) { fclose(fp); return; }
	fp = fopen(path, "w");
	if (fp == NULL) return;
	fputs(content, fp);
	fclose(fp);
}

/* 데이터 폴더와 콘텐츠 파일을 보장한다 (단일 exe로 어디서나 실행 가능하도록). */
void ensure_data_files() {
	ensure_dir("test");
	write_if_missing(MONSTERS_PATH, EMBED_MONSTERS);
	write_if_missing(MAGIC_PATH, EMBED_MAGIC);
	write_if_missing(EQUIP_PATH, EMBED_EQUIP);
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

	/* 보유 마법 비트마스크 (구버전 세이브엔 없음 -> 파이어볼만 보유로 호환) */
	if (fscanf(fp, "%d", &user_data.spells) != 1)
		user_data.spells = 1;

	/* 장비: 보유 비트마스크 + 착용 무기/방어구 인덱스 (구버전 호환: 없으면 기본값) */
	if (fscanf(fp, "%d %d %d", &user_data.equips, &user_data.weapon, &user_data.armor) != 3) {
		user_data.equips = 0;
		user_data.weapon = -1;
		user_data.armor = -1;
	}

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
		if (fscanf(fp, "%31s %d %d %d %d %d %d %d %d %d %d %d",
			mob_data[i].name, &mob_data[i].level, &mob_data[i].emin, &mob_data[i].emax,
			&mob_data[i].gmin, &mob_data[i].gmax, &mob_data[i].hp, &mob_data[i].maxhp,
			&mob_data[i].dam, &mob_data[i].pos, &mob_data[i].pom, &mob_data[i].pob) != 12)
			break;
		i++;
	}

	fclose(fp);
	mob_count = i;
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
		if (fscanf(fp, "%31s %d %d %d", mg_data[i].name, &mg_data[i].mp, &mg_data[i].dam, &mg_data[i].price) != 4)
			break;
		mg_data[i].learned = 0;
		i++;
	}

	fclose(fp);
	spell_count = i;

	/* 보유(학습) 상태를 user_data.spells 비트마스크에서 복원. 파이어볼(0)은 항상 보유. */
	for (int j = 0; j < spell_count; j++)
		mg_data[j].learned = (user_data.spells >> j) & 1;
	if (spell_count > 0) {
		mg_data[0].learned = 1;
		user_data.spells |= 1;
	}
}


void equip_load() {
	eq_data = (equip*)malloc(sizeof(equip));
	int i = 0;
	FILE* fp = open_or_warn(EQUIP_PATH, "r");
	if (fp == NULL) {
		printf(":: 장비 데이터를 불러올 수 없어 게임을 종료합니다 ::\n");
		exit(1);
	}
	while (1) {
		equip* tmp = realloc(eq_data, sizeof(equip) * (i + 1));
		if (tmp == NULL) break;
		eq_data = tmp;
		if (fscanf(fp, "%31s %d %d %d %d %d", eq_data[i].name, &eq_data[i].slot,
			&eq_data[i].power, &eq_data[i].def, &eq_data[i].magic, &eq_data[i].price) != 6)
			break;
		eq_data[i].owned = 0;
		i++;
	}
	fclose(fp);
	equip_count = i;

	/* 보유 상태를 user_data.equips 비트마스크에서 복원 */
	for (int j = 0; j < equip_count; j++)
		eq_data[j].owned = (user_data.equips >> j) & 1;
	/* 착용 인덱스 유효성 검사 (범위 밖이면 미착용) */
	if (user_data.weapon >= equip_count) user_data.weapon = -1;
	if (user_data.armor >= equip_count) user_data.armor = -1;
}


void new_user() {
	printf("------------------------------------------------\n\n");
	printf(":: '행성 : 지구'에 처음 입장하셨습니다 ::\n\n");
	printf("------------------------------------------------\n");
	printf("닉네임을 등록해주세요 : ");
#ifdef __EMSCRIPTEN__
	web_read_line(user_data.name, sizeof(user_data.name));
#else
	fgets(user_data.name, sizeof(user_data.name), stdin);
	user_data.name[strcspn(user_data.name, "\n")] = 0;
#endif
    printf("\n:: %s님 환영합니다! ::\n", user_data.name);

	user_data.first = 1;
	user_data.exp = 0;
	user_data.level = 1;
	user_data.hp = 100;
	user_data.maxhp = 100;
	user_data.mp = 100;
	user_data.maxmp = 100;
	user_data.gold = 0;
	user_data.spells = 1;   /* 파이어볼만 보유하고 시작 */
	user_data.equips = 0;   /* 장비 없음 */
	user_data.weapon = -1;
	user_data.armor = -1;
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
	clear_screen();
}


/* 저장 파일(data.txt, stat.txt)에 현재 상태를 기록한다.
   기존에 save/inside_save/auto_save/clear에 중복되어 있던 파일쓰기를 일원화. */
void write_save_files() {
	FILE* fp = open_or_warn(DATA_PATH, "w");
	if (fp == NULL) return;
	fprintf(fp, "%d %s %d %d %d %d %d %d %d ", user_data.first, user_data.name, user_data.gold, user_data.exp, user_data.level, user_data.hp, user_data.maxhp, user_data.mp, user_data.maxmp);
	fprintf(fp, "%d %d %d %d %d %d ", potions_data.count, potionm_data.count, potionb_data.count, potions_data.healing, potionm_data.healing, potionb_data.healing);
	fprintf(fp, "%d ", user_data.spells);
	fprintf(fp, "%d %d %d ", user_data.equips, user_data.weapon, user_data.armor);
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
	pause_screen();
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
	user_data.spells = 1;
	user_data.equips = 0;
	user_data.weapon = -1;
	user_data.armor = -1;
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


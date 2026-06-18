#include "game.h"


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
	free(eq_data);
	mob_data = NULL;
	mg_data = NULL;
	eq_data = NULL;
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


/* ===== 플랫폼별 콘솔 래퍼 (Windows / POSIX 공용) ===== */

/* 콘솔을 UTF-8로 설정 (Windows 전용). POSIX 터미널은 보통 UTF-8이라 no-op. */
void init_console() {
#ifdef _WIN32
	SetConsoleOutputCP(CP_UTF8);
	SetConsoleCP(CP_UTF8);
#endif
}

/* 화면 지우기 */
void clear_screen() {
#ifdef _WIN32
	system("cls");
#else
	system("clear");
#endif
}

/* 키 입력 대기 */
void pause_screen() {
#ifdef _WIN32
	system("pause");
#else
	int c;
	printf("계속하려면 Enter 키를 누르세요...");
	while ((c = getchar()) != '\n' && c != EOF);
#endif
}

/* ms 밀리초 대기 */
void sleep_ms(int ms) {
#ifdef _WIN32
	Sleep(ms);
#else
	usleep((useconds_t)ms * 1000);
#endif
}


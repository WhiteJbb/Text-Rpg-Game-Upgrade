#include "game.h"

#ifdef __EMSCRIPTEN__
#include <stdarg.h>

/* xterm.js 입력 큐(Module.inq, UTF-8 바이트)에서 한 바이트 꺼냄 (없으면 -1) */
EM_JS(int, em_getch, (), {
	if (!Module.inq || Module.inq.length === 0) return -1;
	return Module.inq.shift();
});
/* 브라우저 터미널에 문자열 출력 */
EM_JS(void, em_write, (const char* s), {
	if (Module.termWrite) Module.termWrite(UTF8ToString(s));
});

/* printf 대체: 포맷 후 터미널로 출력 */
void web_printf(const char* fmt, ...) {
	char buf[2048];
	va_list ap;
	va_start(ap, fmt);
	vsnprintf(buf, sizeof(buf), fmt, ap);
	va_end(ap);
	em_write(buf);
}

/* 입력이 올 때까지 양보(emscripten_sleep)하며 한 바이트 받기 */
static int web_getch(void) {
	int c;
	while ((c = em_getch()) < 0) emscripten_sleep(16);
	return c;
}

/* 한 줄 입력: 에코 + 백스페이스 처리. 개행 제외하고 buf(UTF-8)에 저장 */
void web_read_line(char* buf, int size) {
	int n = 0;
	for (;;) {
		int c = web_getch();
		if (c == '\r' || c == '\n') { em_write("\r\n"); break; }
		if (c == 8 || c == 127) {
			if (n > 0) { n--; em_write("\b \b"); }
			continue;
		}
		if (n < size - 1) {
			buf[n++] = (char)c;
			char e[2] = { (char)c, '\0' };
			em_write(e);
		}
	}
	buf[n] = '\0';
}
#endif


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
#if defined(__EMSCRIPTEN__)
	char line[64];
	web_read_line(line, sizeof(line));
	int v;
	if (sscanf(line, "%d", &v) != 1) return INT_MIN;
	return v;
#else
	int value;
	int c;
	if (scanf("%d", &value) != 1) {
		while ((c = getchar()) != '\n' && c != EOF);
		return INT_MIN;
	}
	while ((c = getchar()) != '\n' && c != EOF);
	return value;
#endif
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
#if defined(__EMSCRIPTEN__)
	em_write("\033[2J\033[H");   /* ANSI: 화면 지우고 커서 홈으로 (xterm.js 해석) */
#elif defined(_WIN32)
	system("cls");
#else
	system("clear");
#endif
}

/* 키 입력 대기 */
void pause_screen() {
#if defined(__EMSCRIPTEN__)
	char tmp[8];
	web_printf("계속하려면 Enter 키를 누르세요...");
	web_read_line(tmp, sizeof(tmp));
#elif defined(_WIN32)
	system("pause");
#else
	int c;
	printf("계속하려면 Enter 키를 누르세요...");
	while ((c = getchar()) != '\n' && c != EOF);
#endif
}

/* ms 밀리초 대기 */
void sleep_ms(int ms) {
#if defined(__EMSCRIPTEN__)
	emscripten_sleep(ms);
#elif defined(_WIN32)
	Sleep(ms);
#else
	usleep((useconds_t)ms * 1000);
#endif
}

/* 폴더 생성 (이미 있으면 무시). 단일 exe 실행 시 데이터/세이브 폴더 자동 생성용. */
void ensure_dir(const char* path) {
#ifdef _WIN32
	CreateDirectoryA(path, NULL);
#else
	mkdir(path, 0755);
#endif
}


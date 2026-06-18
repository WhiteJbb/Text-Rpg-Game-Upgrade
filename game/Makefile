# 행성: 지구 (Text RPG) 빌드
#
# 사용법 (반드시 game/ 폴더에서 — 데이터가 test/ 상대경로):
#   make          # game.exe 빌드
#   ./game.exe    # 실행
#   make clean    # 산출물 정리
#
# Windows에서는 MinGW(gcc) + Git Bash/MSYS 환경을 권장. (rm 사용)

CC      = gcc
CFLAGS  = -std=c11 -Wall -Wextra
SRCS    = main.c util.c save.c player.c battle.c item.c
OBJS    = $(SRCS:.c=.o)
TARGET  = game.exe

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) -o $(TARGET) $(OBJS)

# 모든 .o는 공용 헤더 game.h에 의존
%.o: %.c game.h
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f $(OBJS) $(TARGET)

.PHONY: all clean

# 定义变量
CC_X86 := gcc
CC_ARM :=arm-linux-gnueabihf-gcc # 编译器
CFLAGS := -Wall -O2 # 编译选项
SRCS := hello.c # 源文件
OBJS := $(SRCS:.c=.o) # 目标文件
OBJS_ARM := $(SRCS:.c=_arm.o)
TARGET_X86 := hello_x86
TARGET_ARM := hello_arm  # 输出可执行文件

# 默认目标
.PHONY: all
all: $(TARGET_X86) $(TARGET_ARM)

# X86编译规则
$(TARGET_X86): $(OBJS)
	$(CC) $(CFLAGS) -o $@ $^
# ARM 编译规则
$(TARGET_ARM): $(OBJS_ARM)
	$(CC_ARM) $(CFLAGS) -o $@ $^

# 模式规则
%.o: %.c
	$(CC_X86) $(CFLAGS) -c -o $@ $<
%_arm.o: %.c
	$(CC_ARM) $(CFLAGS) -c -o $@ $<


# 清理生成文件
.PHONY: clean
clean:
	rm -f $(OBJS) *_arm.o $(TARGET_X86) $(TARGET_ARM)


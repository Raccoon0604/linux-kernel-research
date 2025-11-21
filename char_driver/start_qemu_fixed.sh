#!/bin/bash

echo "=== 启动QEMU测试内置驱动 ==="

KERNEL_IMAGE="$HOME/workspace/linux-source/arch/x86/boot/bzImage"
INITRD_IMAGE="$HOME/char_driver/rootfs.cpio.gz"

# 检查文件是否存在
if [ ! -f "$KERNEL_IMAGE" ]; then
    echo "❌ 内核镜像不存在: $KERNEL_IMAGE"
    echo "请先编译内核"
    exit 1
fi

if [ ! -f "$INITRD_IMAGE" ]; then
    echo "❌ rootfs不存在: $INITRD_IMAGE"
    echo "请先创建rootfs"
    exit 1
fi

echo "内核: $KERNEL_IMAGE"
echo "Initrd: $INITRD_IMAGE"
echo ""

echo "启动QEMU..."
echo "控制命令:"
echo "  - Ctrl+A 然后 X: 退出QEMU"
echo "  - poweroff: 在QEMU中关闭系统"
echo ""

qemu-system-x86_64 \
    -kernel "$KERNEL_IMAGE" \
    -initrd "$INITRD_IMAGE" \
    -nographic \
    -append "console=ttyS0 earlyprintk=serial,ttyS0,115200 nokaslr" \
    -m 512M

echo ""
echo "QEMU已退出"

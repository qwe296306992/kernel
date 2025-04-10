#!/bin/bash
UPGRADE_PATH=/home/ys/develop/rk3568/SDK/tools/linux/Linux_Upgrade_Tool/Linux_Upgrade_Tool
../device/rockchip/common/mk-fitimage.sh kernel/boot.img device/rockchip/rk356x/boot.its
echo "Please press UPDATE and reboot cecu!!!"
sleep 5
echo "Start to flash..."
sudo ${UPGRADE_PATH}/upgrade_tool UL /home/ys/develop/rk3568/SDK/IMAGE/RK3568-ATK-EVB1-DDR4-V10-LINUX_20231224.1802_RELEASE_TEST/IMAGES/MiniLoaderAll.bin  -noreset
sudo ${UPGRADE_PATH}/upgrade_tool DI -p /home/ys/develop/rk3568/SDK/IMAGE/RK3568-ATK-EVB1-DDR4-V10-LINUX_20231224.1802_RELEASE_TEST/IMAGES/parameter.txt
sudo ${UPGRADE_PATH}/upgrade_tool DI -boot /home/ys/develop/rk3568/SDK/kernel/boot.img
sudo ${UPGRADE_PATH}/upgrade_tool RD
#!/bin/bash

# 默认值
RK_ARCH=arm64
RK_DEFCONFIG=rockchip_linux_prompt_on_defconfig
RK_DEFCONFIG_FRAGMENT=
RK_DTS=rk3568-atk-evb1-mipi-dsi-720p
RK_JOBS=24

while [ $# -gt 0 ]; do
	case $1 in
		arch=*)
			arg=${1#*=}
			if [ -n "$arg" ]; then
				RK_ARCH=$arg
			fi
			shift 1
			;;
		defconfig=*)
			arg=${1#*=}
			if [ -n "$arg" ]; then
				RK_DEFCONFIG=$arg
			fi
			shift 1
			;;
		defconfig_fragment=*)	# xxx.config文件
			RK_DEFCONFIG_FRAGMENT=${1#*=}
			shift 1
			;;
		dts=*)
			arg=${1#*=}
			if [ -n "$arg" ]; then
				RK_DTS=$arg
			fi
			shift 1
			;;
		jobs=*)
			arg=${1#*=}
			if [ -n "$arg" ]; then
				RK_JOBS=$arg
			fi
			shift 1
			;;
		*)
			shift 1
			;;
	esac
done

# 配置
#make ARCH=$RK_ARCH $RK_DEFCONFIG $RK_DEFCONFIG_FRAGMENT
# 编译
bear -- make ARCH=$RK_ARCH $RK_DTS.img -j$RK_JOBS
# 编译出720p和1080p MIPI屏设备树镜像
echo "#################################################"
make ARCH=$RK_ARCH rockchip/rk3568-atk-evb1-ddr4-v10-linux.dtb -j$RK_JOBS
make ARCH=$RK_ARCH rockchip/rk3568-atk-evb1-mipi-dsi-1080p.dtb -j$RK_JOBS
make ARCH=$RK_ARCH rockchip/rk3568-atk-evb1-mipi-dsi-720p.dtb -j$RK_JOBS
make ARCH=$RK_ARCH rockchip/rk3568-atk-evb1-mipi-dsi-10p1_800x1280.dtb -j$RK_JOBS
make ARCH=$RK_ARCH rockchip/rk3568-atk-atompi-ca1.dtb -j$RK_JOBS
make ARCH=$RK_ARCH rockchip/rk3568-atk-atompi-ca1-720p.dtb -j$RK_JOBS
make ARCH=$RK_ARCH rockchip/rk3568-atk-atompi-ca1-1080p.dtb -j$RK_JOBS
make ARCH=$RK_ARCH rockchip/rk3568-atk-atompi-ca1-10p1_800x1280.dtb -j$RK_JOBS
# 生成包含多个dtb的resource.img镜像
rm -rf resource.img
python2 scripts/mkmultidtb.py RK3568-ATK-EVB1

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

make ARCH=$RK_ARCH $RK_DEFCONFIG $RK_DEFCONFIG_FRAGMENT
bear -- make ARCH=$RK_ARCH $RK_DTS.img -j$RK_JOBS

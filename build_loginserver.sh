#!/bin/bash

build() {
    yum -y install cmake
    yum -y install libuuid-devel
    yum -y install openssl-devel
    yum -y install curl-devel

	echo "#ifndef __VERSION_H__" > base/version.h
	echo "#define __VERSION_H__" >> base/version.h
	echo "#define VERSION \"$1\"" >> base/version.h
	echo "#endif" >> base/version.h

    if [ ! -d lib ]
    then
        mkdir lib
    fi

	cd base
    cmake .
	make
    if [ $? -eq 0 ]; then
        echo "make base successed";
    else
        echo "make base failed";
        exit;
    fi
    if [ -f libbase.a ]
    then
        cp libbase.a ../lib/
    fi
    cd ../slog
    cmake .
    make
    if [ $? -eq 0 ]; then
        echo "make slog successed";
    else
        echo "make slog failed";
        exit;
    fi
    mkdir ../base/slog/lib
    cp slog_api.h ../base/slog/
    cp libslog.so ../base/slog/lib/
    cp -a lib/liblog4cxx* ../base/slog/lib/

    cd ../login_server
    cmake .
	make
    if [ $? -eq 0 ]; then
        echo "make login_server successed";
    else
        echo "make login_server failed";
        exit;
    fi

	cd ../

    mkdir -p ../run/login_server


	#copy executables to run/ dir
	cp login_server/login_server ../run/login_server/


    build_version=im-server-$1
    build_name=$build_version.tar.gz
	if [ -e "$build_name" ]; then
		rm $build_name
	fi
    mkdir -p ../$build_version
    mkdir -p ../$build_version/login_server
    mkdir -p ../$build_version/route_server
    mkdir -p ../$build_version/msg_server
    mkdir -p ../$build_version/file_server
    mkdir -p ../$build_version/msfs
    mkdir -p ../$build_version/http_msg_server
    mkdir -p ../$build_version/push_server
    mkdir -p ../$build_version/db_proxy_server
    mkdir -p ../$build_version/lib

    cp login_server/loginserver.conf ../$build_version/login_server/
    cp login_server/login_server ../$build_version/login_server/

    cp slog/log4cxx.properties ../$build_version/lib/
    cp slog/libslog.so  ../$build_version/lib/
    cp -a slog/lib/liblog4cxx.so* ../$build_version/lib/
    cp sync_lib_for_zip.sh ../$build_version/

    cp tools/daeml ../$build_version/
    cp ../run/restart.sh ../$build_version/

    cd ../
    tar zcvf    $build_name $build_version

    rm -rf $build_version
}

clean() {
	cd base
	make clean
	cd ../login_server
	make clean
	
}

print_help() {
	echo "Usage: "
	echo "  $0 clean --- clean all build"
	echo "  $0 version version_str --- build a version"
}

case $1 in
	clean)
		echo "clean all build..."
		clean
		;;
	version)
		if [ $# != 2 ]; then 
			echo $#
			print_help
			exit
		fi

		echo $2
		echo "build..."
		build $2
		;;
	*)
		print_help
		;;
esac

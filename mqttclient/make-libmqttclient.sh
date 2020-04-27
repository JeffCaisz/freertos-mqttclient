#!/bin/bash

mkdir -p libmqttclient libmqttclient/include libmqttclient/lib

cd libmqttclient

if [ ! -f "Makefile" ]; then

cat  <<-EOF > Makefile
CC=gcc

SRC = \$(wildcard ../*.c \\
        ../common/*.c \\
        ../common/log/*.c \\
        ../common/log/arch/linux/*.c \\
        ../common/mbedtls/library/*.c \\
        ../common/mbedtls/wrapper/*.c \\
        ../mqtt/*.c \\
        ../mqttclient/*.c \\
        ../network/*.c \\
        ../platform/linux/*.c \\
        )

INC =   -lpthread \\
        -I../common \\
        -I../common/log \\
        -I../common/mbedtls/include \\
        -I../common/mbedtls/include/mbedtls \\
        -I../common/mbedtls/wrapper \\
        -I../mqtt \\
        -I../mqttclient \\
        -I../network \\
        -I../platform/linux \\
        -I../test

OBJS = \$(patsubst %.c, %.o, \$(SRC))

FLAG = -g -fPIC -I. -Iinclude \$(INC) 
TARGET = libmqttclient.so

EOF

echo -e "\n\$(TARGET):\$(OBJS)" >> Makefile
echo -e "\t\$(CC) -shared -o \$@ \$^ \$(FLAG)" >> Makefile

echo -e "\n%.o:%.c" >> Makefile
echo -e "\t\$(CC) -shared -o \$@ \$^ \$(FLAG)" >> Makefile

echo -e "\nclean:" >> Makefile
echo -e "\trm -rf \$(TARGET) \$(OBJS)" >> Makefile

echo -e "\ninstall:" >> Makefile
echo -e "\tsudo cp -rdf \$(TARGET) /usr/lib/." >> Makefile

echo -e "\nremove:" >> Makefile
echo -e "\tsudo rm -rdf /usr/lib/\$(TARGET)" >> Makefile

echo -e "\n.PHONY:clean" >> Makefile

fi

cp -r ../mqtt/*.h include/.
cp -r ../common/*.h include/.
cp -r ../network/*.h include/.
cp -r ../mqttclient/*.h include/.
cp -r ../common/log/*.h include/.
cp -r ../platform/linux/*.h include/.
cp -r ../common/mbedtls/include include/.
cp -r ../common/mbedtls/wrapper/*.h include/.

if [ " $1" == " " ]; then
    make
    make install
    mv libmqttclient.so lib/.
    make clean

elif [ "$1" == "remove" ]; then
    make remove
fi

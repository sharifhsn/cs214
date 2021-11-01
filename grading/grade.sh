#/bin/bash

cp -r /common/home/bz186/cs214pa2grade/Makefile .
make

MYDIR=$(pwd)
MYLS=$(pwd)/ls
MYFIND=$(pwd)/find
MYTREE=$(pwd)/tree

mkdir test
cp -r /common/home/bz186/cs214pa2grade/testcase test


echo "###############"
echo "1-1"
cd $MYDIR/test/testcase/hw2
$MYLS

echo "1-2"
cp $MYLS .
./ls

echo "1-3"
./ls -l

echo "1-4"
cd $MYDIR/test/testcase
cp $MYLS .
./ls test1

echo "1-5"
cd $MYDIR/test/testcase/test1
cp $MYLS .
./ls -l .

echo "###############"
echo "2-1"
cd $MYDIR/test/testcase/hw2
$MYFIND .c

echo "2-2"
cp $MYFIND .
./find ls.c

echo "2-3"
./find .c

echo "2-4"
./find .h

echo "2-5"
cd $MYDIR/test/testcase/test1
cp $MYFIND .
./find d

echo "###############"
echo "3-1"
cd $MYDIR/test
$MYTREE

echo "3-2"
cd $MYDIR/test/testcase
cp $MYTREE .
./tree

echo "3-3"
cd $MYDIR/test/testcase/hw3
cp $MYTREE .
./tree

echo "3-4"
cd $MYDIR/test/testcase/test1
cp $MYTREE .
./tree


#!/bin/bash

mk_absent_dir()
{
    mkdir -p $1
}

delete_class()
{
    rm -rvf $1/*.class
}

compile_and_mv()
{
    TARGET=$1
    CURRENT=`pwd`
    cd ../$TARGET && javac *.java && mv *.class $CURRENT/$TARGET
    cd $CURRENT
}

build_rt_jar()
{
    jar cf rt.jar java/*
}

main()
{
    mk_absent_dir "java/lang"
    mk_absent_dir "java/io"

    delete_class "java/lang"
    delete_class "java/io"

    echo "compiling..."
    compile_and_mv "java/lang"
    compile_and_mv "java/io"

    build_rt_jar
    echo "build rt.jar finish."
}

main

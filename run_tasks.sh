#!/bin/bash

FILES=./task/*

main()
{
    for f in $FILES
    do
        #echo $f
        local f1=$(echo $f | sed 's/task/initial/g')
        build/tsp $f $f1.txt
    done
}

main

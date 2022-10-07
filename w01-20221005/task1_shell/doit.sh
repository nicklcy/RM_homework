#!/bin/bash
# 用法：./doit.sh dir
time for file in $(find $1 | grep -E '.\.(c|h)'); do if [ "$(grep 'hello' $file)" != "" ]; then echo $file; fi; done

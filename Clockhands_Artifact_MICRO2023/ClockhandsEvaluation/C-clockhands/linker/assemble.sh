#!/bin/sh

  cat $1 \
| sed 's/\r//' \
| awk 'BEGIN{nops=0}{if($3=="NOP"&&$4=="s,"){nops++}else if(nops==0){print}else{print " RPINC    s,",nops;print;nops=0}}END{if(nops!=0){print " RPINC    s,", nops}}' \
| awk 'BEGIN{nops=0}{if($3=="NOP"&&$4=="t,"){nops++}else if(nops==0){print}else{print " RPINC    t,",nops;print;nops=0}}END{if(nops!=0){print " RPINC    t,", nops}}' \
| awk 'BEGIN{nops=0}{if($3=="NOP"&&$4=="u,"){nops++}else if(nops==0){print}else{print " RPINC    u,",nops;print;nops=0}}END{if(nops!=0){print " RPINC    u,", nops}}' \
| awk 'BEGIN{nops=0}{if($3=="NOP"&&$4=="v,"){nops++}else if(nops==0){print}else{print " RPINC    v,",nops;print;nops=0}}END{if(nops!=0){print " RPINC    v,", nops}}' \
| awk '{ if( $0 !~ /^.\[.*(PHI|IMPLICIT)/ ) { print $0 } }' \
| awk 'BEGIN{print " [start ] JMP      n, Function_main"}{print}' \
| $(dirname $0)/linker \
| sed '/:$/d' \
| sed 's/^.\[......\]//g' \
| sed 's/#.*$//g' \
| sed 'y/<>,/   /' \
| sed 's/JMP/J  /' \
| sed 's/RMOV\(.*\)/ADDi.64 \1 0/' \
| sed 's/ zero/  0 15/g' \
| sed 's/  n/  0 /g' \
| sed 's/  s/  0 /g' \
| sed 's/  t/  1 /g' \
| sed 's/  u/  2 /g' \
| sed 's/  v/  3 /g'

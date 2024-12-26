include ../stuff/make.inc

LIB  := lib/lputchar.s

.PHONY : clean
.DEFAULT_GOAL = chbin/coremark.chbin

XCFLAGS := \
    -DCLOCKS_PER_SEC=1 \
    -DFLAGS_STR=\"\" \
    -DRUN_ON_64BIT

LIBLL := $(addprefix intermediate/,memset.ll memcpy.ll puts.ll ee_printf.ll)
LLS := $(addprefix intermediate/,core_list_join.ll core_main.ll core_matrix.ll core_portme.ll core_state.ll core_util.ll)

$(LIBLL) : intermediate/%.ll : src/%.c
	mkdir -p intermediate/
	$(clang12) $(XCFLAGS) --target=riscv64-pc_linux-gnu -fno-jump-tables -O2 -S -emit-llvm $< -o $@

$(LLS) : intermediate/%.ll : src/%.c
	mkdir -p intermediate/
	$(clang12) -DITERATIONS=10 $(XCFLAGS) -DTIME --target=riscv64-pc_linux-gnu -fno-jump-tables -O2 -S -emit-llvm $< -o $@


intermediate/coremark_linked.ll : $(LLS) $(LIBLL)
	$(llvm-link12) $^ -S -o $@

#intermediate/coremark.sz : intermediate/coremark_linked.ll
#	$(llc) -csr_num=8 -march=straight -O2 $< -o $@ > $@.log 2>&1
intermediate/coremark.sz : lib/coremark.sz
	mkdir -p intermediate
	cp $< $@

# .sz must be placed last.
intermediate/coremark.sl : $(LIB) intermediate/coremark.sz
	cat $^ > $@

intermediate/coremark.s : intermediate/coremark.sl ../linker/linker
	../linker/assemble.sh < $< > $@

chbin/coremark.chbin : intermediate/coremark.s ../clockhands-assembler2/clockhands-assembler2
	mkdir -p chbin/
	../clockhands-assembler2/clockhands-assembler2 --file $< --output $@

clean :
	rm -rf intermediate/ chbin/

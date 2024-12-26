include ../stuff/make.inc

.PHONY : clean
.DEFAULT_GOAL = rvbin/coremark.rvbin

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

intermediate/coremark.s : intermediate/coremark_linked.ll
	mkdir -p rvbin
	$(llc12) -march=riscv64 -mattr=+m,+f,+d -O2 $< -o $@ > $@.log 2>&1
	sed -e '/^\t\.attribute/d' -i $@

rvbin/coremark.rvbin : intermediate/coremark.s $(musl-gcc)
	$(musl-gcc) -static -o $@ $<

clean :
	rm -rf intermediate/ rvbin/

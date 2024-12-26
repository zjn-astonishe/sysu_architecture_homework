onikiri:=../onikiri2

.PHONY : all

all : $(addprefix coremark, .rvbin.xml .stbin.xml .chbin.xml)

$(addprefix coremark, .rvbin.xml .stbin.xml .chbin.xml) : coremark.% : ../0.coremark/stuff/param.%
	$(onikiri) $< $(param) 2>/dev/null

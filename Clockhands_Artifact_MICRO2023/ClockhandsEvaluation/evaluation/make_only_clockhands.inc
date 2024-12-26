onikiri:=../onikiri2

coremark10.chbin.xml : coremark10.% : ../0.coremark/stuff/param.%
	$(onikiri) $< $(param) 2>/dev/null

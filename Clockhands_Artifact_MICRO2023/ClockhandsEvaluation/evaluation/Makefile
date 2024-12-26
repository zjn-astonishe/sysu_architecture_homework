jobs:= $(shell echo *-result)

.PHONY : all $(jobs)

all : $(jobs)

clean :
	rm -f *-result/bzip*.xml *-result/mcf*.xml *-result/lbm*.xml *-result/xz*.xml *-result/coremark*.xml

$(jobs) :
	cd $@; $(MAKE)

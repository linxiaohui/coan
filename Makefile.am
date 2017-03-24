# not a GNU package. You can remove this line, if
# have all needed files, that a GNU package needs
AUTOMAKE_OPTIONS = foreign 1.4

EXTRA_DIST := man/coan.1.gz man/html/coan_man_1.html man/man_pod.pl\
test_coan/wordsize.cpp test_coan/scrap_dir_tree.py test_coan/coan_case_tester.py \
test_coan/coan_bulk_tester.py test_coan/coan_softlink_tester.py \
test_coan/coan_symbol_rewind_tester.py test_coan/coan_spin_tester.py \
test_coan/coan_test_metrics.py test_coan/class_TestCase.py python/coanlib.py \
python/argparse.py \
$(wildcard test_coan/test_cases/*.c) \
$(wildcard test_coan/test_cases/*.expect) \
test_coan/test_cases/altfiles \
Doxyfile LICENSE.BSD README

SUBDIRS = src man test_coan

.phony:  clean-recursive doxy-clean doxygen

clean-recursive: doxy-clean man-clean

doxy-clean:
	rm -fr ./html/*

man-clean:
	$(MAKE) -C man clean

doxygen: ./html/index.html

./html/index.html:	./src/coan
	if test -d ./html;	then rm -fr ./html/*; fi
	doxygen Doxyfile

man/coan.1.gz:
	$(MAKE) -C man

man/html/coan_man_1.html: man/coan.1.gz

./src/coan:
	$(MAKE) -C src

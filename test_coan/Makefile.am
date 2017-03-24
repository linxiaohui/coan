if MAKE_CHECK_TIMING
TIMING_METRICS_ENABLED = 1
else
TIMING_METRICS_ENABLED = 0
endif

noinst_PROGRAMS = wordsize
wordsize_SOURCES = wordsize.cpp
CORE_TESTS = coan_case_tester.py coan_bulk_tester.py coan_spin_tester.py \
coan_symbol_rewind_tester.py coan_softlink_tester.py
if MAKE_CHECK_TIMING
TESTS = $(CORE_TESTS) coan_test_metrics.py
else
TESTS = $(CORE_TESTS)
endif

AM_TESTS_ENVIRONMENT = COAN_PKGDIR=$(top_srcdir); \
	COAN_BUILDDIR=$(top_builddir); TIMING_METRICS=$(TIMING_METRICS_ENABLED); \
	rm -f coan.test_timer.time.txt; \
	export COAN_PKGDIR; export COAN_BUILDDIR; export TIMING_METRICS_ENABLED;
LOG_COMPILER = python
	 
.phony: all clean clean-metrics distclean-local cat-failure-logs

cat-failure-logs: test-suite.log
	for trs in *.trs; do \
		if test ! -z "`cat $$trs | grep -E ':test-result: FAIL' -`"; then \
			log_file=$${trs%%.trs}.log; \
			echo '---------------- BEGIN failure log' $$log_file \
				'----------------'; \
			cat $$log_file; \
			echo '---------------- END failure log' $$log_file \
				'----------------'; \
		fi; \
	done
	
check: cat-failure-logs

clean-metrics:
	rm -f *.time.txt *.size.txt
	
check-am: clean-metrics
	
clean: clean-metrics
	rm -f *.o wordsize *.tmp
	cd $(top_srcdir)/test_coan/test_cases && \
		rm -f *.stderr *.stdout *.output

distclean-local: clean


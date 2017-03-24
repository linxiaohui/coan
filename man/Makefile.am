man1_MANS = coan.1.gz

.phony: all man clean distlean

all:	man

clean:
	rm -f coan.1* ./html/* *.tmp

distclean: clean
	rm -f Makefile

man:	coan.1.gz
 
coan.1: man_pod.pl
	rel_month='AUGUST 2014' && \
	if [ ! -d html ]; then \
		mkdir html; \
	fi && \
	sed -e "s/{{REL_MONTH}}/$$rel_month/" < $< > man_pod_rel.tmp && \
	pod2man man_pod_rel.tmp --section=1 --name=coan --center="User Commmands" --date="burroingroingjoing.com" --release="$$rel_month" > coan.1 && \
	pod2html --noindex --title="Coan Man Page" --infile=man_pod_rel.tmp --outfile=html/coan_man_1.html && \
	rm -f *.tmp

coan.1.gz: coan.1
	gzip -f $<


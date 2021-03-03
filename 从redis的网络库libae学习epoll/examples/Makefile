all:
	gcc echo.c ../libae/ae.c ../libae/ae_epoll.c ../libae/anet.c \
	../libae/monotonic.c ../libae/zmalloc.c -I ../libae -o echo

clean:
	rm -f echo
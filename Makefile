all: test_mpx override

test_mpx:
	g++ -g3 test_mpx.cpp -o run_presses -lX11 -lXi -lXtst

override:
	g++ -g3 set_override_redirect.cpp -o set_override_redirect -lX11

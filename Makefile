all: test_mpx override

test_mpx:
	g++ -g3 test_mpx.cpp -o run_presses -lX11 -lXi -lXtst

lib_mpx_input:
	gcc -g3 -fPIC -shared -o lib_mpx_input.so lib_mpx_input.c -lX11 -lXi -lXtst

override:
	g++ -g3 set_override_redirect.cpp -o set_override_redirect -lX11

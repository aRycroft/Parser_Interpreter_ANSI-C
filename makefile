compiler = clang
regFlags = -Wall -Wextra -Wfloat-equal -pedantic -ansi -O2 -Wno-overlength-strings
fFlags = -fsanitize=address -fsanitize=undefined -g3 
vFlags = -Wall -Wextra -Wfloat-equal -pedantic -ansi -g3

all: parse interp

parse: NAL.c NAL.h mvm.c mvm.h
	$(compiler) NAL.c mvm.c $(regFlags) -o parse
parse_S: NAL.c NAL.h mvm.c mvm.h
	$(compiler) NAL.c mvm.c $(fFlags) -o parse_S
parse_D: NAL.c NAL.h mvm.c mvm.h	
	$(compiler) NAL.c mvm.c $(vFlags) -o parse_D
parse_T: NAL.c NAL.h mvm.c mvm.h	
	$(compiler) NAL.c mvm.c $(regFlags) -o parse_T -DTESTING

interp: NAL.c NAL.h mvm.c mvm.h
	$(compiler) NAL.c mvm.c $(regFlags) -o interp -DINTERP
interp_S: NAL.c NAL.h mvm.c mvm.h
	$(compiler) NAL.c mvm.c $(fFlags) -o interp_S -DINTERP
interp_D: NAL.c NAL.h mvm.c mvm.h	
	$(compiler) NAL.c mvm.c $(vFlags) -o interp_D -DINTERP
interp_T: NAL.c NAL.h mvm.c mvm.h	
	$(compiler) NAL.c mvm.c $(regFlags) -o interp_T -DTESTING -DINTERP

test: testparse testinterp

testparse : parse_S parse_D parse_T
	./parse_S test1.nal
	./parse_S test2.nal
	./parse_S test3.nal
	./parse_S test4.nal
	./parse_S test5.nal
	./parse_S test6.nal
	./parse_S testing/extraParse1.nal
	./parse_S testing/extraParse2.nal
	./parse_S testing/extraParse3.nal
	./parse_S testing/emptyParse.nal
	valgrind ./parse_D test1.nal
	valgrind ./parse_D test2.nal
	valgrind ./parse_D test3.nal
	valgrind ./parse_D test4.nal
	valgrind ./parse_D test5.nal
	valgrind ./parse_D test6.nal
	valgrind ./parse_D testing/extraParse1.nal
	valgrind ./parse_D testing/extraParse2.nal
	valgrind ./parse_D testing/extraParse3.nal
	valgrind ./parse_D testing/emptyParse.nal
	valgrind ./parse_T

testinterp: interp_S interp_D interp_T
	./interp_S test1.nal
	./interp_S test2.nal
	./interp_S test4.nal
	./interp_S test5.nal
	valgrind ./interp_D test1.nal
	valgrind ./interp_D test2.nal
	valgrind ./interp_D test4.nal
	valgrind ./interp_D test5.nal
	valgrind ./interp_T
	

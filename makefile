add_cover: flac_meta.o flac_add_cover.o
	gcc -o add_cover flac_meta.o flac_add_cover.o
flac_meta.o: flac_meta.c
	gcc -c flac_meta.c -o flac_meta.o
flac_add_cover.o: flac_add_cover.c
	gcc -c flac_add_cover.c -o flac_add_cover.o

clean:
	rm -rf *.o
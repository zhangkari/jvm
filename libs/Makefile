all:
	@echo "Compiling zlib"
	@make -C zlib-1.2.8
	@echo "Compile zlib OK"
	@make -C zlib-1.2.8/contrib/minizip
	@echo "Build libjar.a OK"
	@cp zlib-1.2.8/contrib/minizip/libjar.a ./
	@echo "Copy libjar.a OK"

clean:
	@make -C zlib-1.2.8 clean
	@make -C zlib-1.2.8/contrib/minizip clean
	@rm -rvf libjar.so libjar.a

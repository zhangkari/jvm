all:
	@echo "Compiling zlib"
	@make -C zlib-1.2.8
	@echo "Compile zlib OK"
	@make -C zlib-1.2.8/contrib/minizip
	@echo "Build libjar.so OK"
	@cp zlib-1.2.8/contrib/minizip/libjar.so ../target/libs
	@echo "Copy libjar.so OK"

clean:
	@make -C zlib-1.2.8 clean

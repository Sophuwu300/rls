ifeq ($(shell command -v cmake), )
COMPCMD = g++ --std=c++17 -o build/rls rls.cpp
COMPMSG = g++
else
ifeq ($(shell command -v ninja), )
COMPCMD = cmake -S . -B build && cmake --build build
COMPMSG = cmake
else
COMPCMD = cmake -GNinja -S . -B build && cmake --build build
COMPMSG = cmake and ninja
endif
CMAKELISTS = CMakeLists.txt
endif

ifeq ($(shell command -v strip), )
STRIPCMD = echo Strip not found. Binary size may be larger than expected.
else
STRIPCMD = strip --strip-all build/rls
endif
ifeq ($(shell command -v upx), )
UPXCMD = echo UPX not found. Binary size may be larger than expected.
else
UPXCMD = upx --best build/rls > /dev/null
endif

build: rls.cpp $(CMAKELISTS)
	@echo Building with $(COMPMSG).
	@mkdir -p build
	@$(COMPCMD)
	@echo Compiled successfully.
	@echo Program is located at build/rls.
	@echo "make small" to reduce binary size. Requires strip and/or upx.
	@echo "sudo make install" to install the program into /usr/local/bin.
	@echo And "make clean" to remove the build directory.

clean: build/
	@echo Cleaning up.
	@rm -rf build
	@echo Cleaned up successfully.

small: build/rls
	@cp build/rls build/rls.bak
	@echo Stripping binary.
	@$(STRIPCMD)
	@$(UPXCMD)
	@echo Old Size: && du -sh build/rls.bak
	@echo New size: && du -sh build/rls


install: build/rls
	@echo Installing to /usr/local/bin.
	@sudo cp build/rls /usr/local/bin/rls
	@echo Installed successfully.
	@echo Run "rls" to use the program.

arm: rls.cpp CMakeLists.txt arm64_toolchain.cmake
	@echo building arm
	@mkdir -p build/arm
	@cmake -DCMAKE_TOOLCHAIN_FILE=arm64_toolchain.cmake -B build/arm
	@cmake --build build/arm

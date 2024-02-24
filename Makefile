BUILDDIR?=./build
ifeq ($(shell command -v cmake), )
COMPCMD = g++ --std=c++17 -o $(BUILDDIR)/rls rls.cpp
else
ifeq ($(shell command -v ninja), )
COMPCMD = cmake -S . -B $(BUILDDIR) && cmake --build $(BUILDDIR)
else
COMPCMD = cmake -GNinja -S . -B $(BUILDDIR) && cmake --build $(BUILDDIR)
endif
CMAKELISTS = CMakeLists.txt
endif

build: rls.cpp $(CMAKELISTS)
	@mkdir -p $(BUILDDIR)
	@$(COMPCMD)
	@echo Compiled successfully.

clean: $(BUILDDIR)/
	@echo Cleaning up.
	@rm -rf $(BUILDDIR)
	@echo Cleaned up successfully.

install: $(BUILDDIR)/rls
	@echo Installing to /usr/local/bin.
	@sudo cp $(BUILDDIR)/rls /usr/local/bin/rls
	@echo Installed successfully.
	@echo Run "rls" to use the program.

.PHONY: build-native
.PHONY: build-web

EXEC = raymarch

# WEB = true
TYPE = debug
# TYPE = release

build:
	if [ "$(WEB)" = "true" ]; then $(MAKE) build-web; else $(MAKE) build-native; fi

build-setup:
	if [ "$(WEB)" = "true" ]; then $(MAKE) build-setup-web; else $(MAKE) build-setup-native; fi

build-setup-native:
	cmake . -B build-native/debug \
		-G Ninja \
		-D CMAKE_BUILD_TYPE=Debug \
		-D CMAKE_C_COMPILER=clang \
		-D CMAKE_CXX_COMPILER=clang++ \
		-D CMAKE_C_COMPILER_LAUNCHER=ccache \
		-D CMAKE_CXX_COMPILER_LAUNCHER=ccache \
		-D CMAKE_COLOR_DIAGNOSTICS=ON \

build-setup-web:
	emcmake cmake . -B build-web/debug \
		# -G Ninja \
		-D CMAKE_BUILD_TYPE=Debug \
		-D CMAKE_C_COMPILER_LAUNCHER=ccache \
		-D CMAKE_CXX_COMPILER_LAUNCHER=ccache \
		-D CMAKE_COLOR_DIAGNOSTICS=ON

build-native:
	cmake --build build-native/$(TYPE) --target $(EXEC)
	cp build-native/$(TYPE)/compile_commands.json .

build-web:
	# cmake --build build-web/$(TYPE) --target $(EXEC)
	cmake --build build-web/$(TYPE) -j4 --target $(EXEC)
	cp build-web/$(TYPE)/compile_commands.json .

run:
	if [ "$(WEB)" = "true" ]; then $(MAKE) run-web; else $(MAKE) run-native; fi

run-native:
	build-native/$(TYPE)/$(EXEC)

run-web:
	emrun --browser chrome build-web/$(TYPE)/$(EXEC).html

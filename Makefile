BUILD_DIR = build.make

test: clean all
	$(MAKE) -C $(BUILD_DIR) test

all:
	$(MAKE) $(BUILD_DIR)/Makefile
	$(MAKE) -C $(BUILD_DIR) all

$(BUILD_DIR)/Makefile: $(BUILD_DIR) CMakeLists.txt
	( cd $(BUILD_DIR); cmake -G "Unix Makefiles" .. )

$(BUILD_DIR):
	mkdir $@

clean:
	$(RM) -R $(BUILD_DIR)

report:
	cat build.make/Testing/Temporary/LastTest.log


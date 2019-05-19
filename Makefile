OUT_DIR = .build
GEN_DIRS += $(OUT_DIR)

USERID ?= $(shell id -u)
USERID := $(USERID)
DOCKER_BUILDFLAGS += --build-arg 'USERID=$(USERID)'

.PHONY: all
all: image

$(GEN_DIRS):
	mkdir -p $@

.PHONY: image
image: $(OUT_DIR)/pam-example

$(OUT_DIR)/Dockerfile: Dockerfile | $(OUT_DIR)
	cp $< $@

$(OUT_DIR)/pam-example: $(OUT_DIR)/Dockerfile  Makefile | $(OUT_DIR)
	docker build --rm $(DOCKER_BUILDFLAGS) --iidfile $@ --file $< --tag pam-example .

.PHONY: run
run: image
	docker run --rm -it --user "$(USERID)" pam-example bash

.PHONY: clean
clean:
	rm -rf $(GEN_DIRS)


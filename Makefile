SHELL := /bin/bash
QMK_HOME := $(shell echo $$(pwd)/third_party/qmk_firmware)

.PHONY: help
help: ## Shows all targets and help from the Makefile (this message).
	@echo "Makefile"
	@echo "Usage: make [COMMAND]"
	@echo "Requires qmk and wally-cli tools to be installed."
	@echo ""
	@grep --no-filename -E '^([/a-z.A-Z0-9_%-]+:.*?|)##' $(MAKEFILE_LIST) | \
		awk 'BEGIN {FS = "(:.*?|)## ?"}; { \
			if (length($$1) > 0) { \
				printf "  \033[36m%-20s\033[0m %s\n", $$1, $$2; \
			} else { \
				if (length($$2) > 0) { \
					printf "%s\n", $$2; \
				} \
			} \
		}'

$(QMK_HOME):
	mkdir -p $$(dirname $(QMK_HOME))
	qmk setup -y -H $(QMK_HOME) -b firmware21 zsa/qmk_firmware

$(QMK_HOME)/keyboards/moonlander/keymaps/dvorak_ianlewis: $(QMK_HOME)
	mkdir -p $@
	cp -r ./moonlander/* $@/

$(QMK_HOME)/moonlander_dvorak_ianlewis.bin: $(QMK_HOME)/keyboards/moonlander/keymaps/dvorak_ianlewis
	cd $<
	qmk compile -kb moonlander -km dvorak_ianlewis

.PHONY: moonlander-compile
moonlander-compile: $(QMK_HOME)/moonlander_dvorak_ianlewis.bin ## compile moonlander firmware

.PHONY: moonlander-flash
moonlander-flash: $(QMK_HOME)/moonlander_dvorak_ianlewis.bin ## flash moonlander firmware
	wally-cli $<

.PHONY: clean
clean: ## clean build artifacts
	cd $(QMK_HOME)
	qmk clean -a

.PHONY: full-clean
full-clean: ## full directory clean
	rm -rf third_party

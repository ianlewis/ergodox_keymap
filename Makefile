# Copyright 2025 Ian Lewis
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
#      http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.

SHELL := /bin/bash
OUTPUT_FORMAT ?= $(shell if [ "${GITHUB_ACTIONS}" == "true" ]; then echo "github"; else echo ""; fi)
REPO_NAME = $(shell basename "$$(pwd)")
REPO_ROOT := $(shell echo $$(pwd))
QMK_HOME := $(shell echo $$(pwd)/third_party/qmk_firmware)

# The help command prints targets in groups. Help documentation in the Makefile
# uses comments with double hash marks (##). Documentation is printed by the
# help target in the order in appears in the Makefile.
#
# Make targets can be documented with double hash marks as follows:
#
#	target-name: ## target documentation.
#
# Groups can be added with the following style:
#
#	## Group name

.PHONY: help
help: ## Shows all targets and help from the Makefile (this message).
	@echo "$(REPO_NAME) Makefile"
	@echo "Usage: make [COMMAND]"
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

package-lock.json:
	@npm install

node_modules/.installed: package.json package-lock.json
	@npm ci
	@touch node_modules/.installed

.venv/bin/activate:
	@python -m venv .venv

.venv/.installed: .venv/bin/activate requirements.txt
	@./.venv/bin/pip install -r requirements.txt --require-hashes
	@touch .venv/.installed

## Build
#####################################################################

$(QMK_HOME): .venv/.installed
	mkdir -p $$(dirname $(QMK_HOME))
	$(REPO_ROOT)/.venv/bin/qmk \
		--config-file qmk.ini \
		setup \
			--yes \
			--home $(QMK_HOME) \
			--branch firmware24 \
			zsa/qmk_firmware

$(QMK_HOME)/keyboards/zsa/moonlander/keymaps/dvorak_ianlewis/%: ./moonlander/%
	mkdir -p $$(dirname $@)
	cp -r ./moonlander/$* $@

$(QMK_HOME)/moonlander_dvorak_ianlewis.bin: $(QMK_HOME) $(QMK_HOME)/keyboards/zsa/moonlander/keymaps/dvorak_ianlewis/keymap.c $(QMK_HOME)/keyboards/zsa/moonlander/keymaps/dvorak_ianlewis/rules.mk
	cd $$(dirname $<)
	$(REPO_ROOT)/.venv/bin/qmk \
		--config-file qmk.ini \
		compile \
			--keyboard zsa/moonlander \
			--keymap dvorak_ianlewis

.PHONY: moonlander-compile
moonlander-compile: $(QMK_HOME)/moonlander_dvorak_ianlewis.bin ## compile moonlander firmware

.PHONY: moonlander-flash
moonlander-flash: $(QMK_HOME)/moonlander_dvorak_ianlewis.bin ## flash moonlander firmware
	wally-cli $<

# TODO: Add ergodox-ez compile & flash targets.

## Tools
#####################################################################

.PHONY: license-headers
license-headers: ## Update license headers.
	@set -euo pipefail; \
		files=$$( \
			git ls-files --deduplicate \
				'*.c' \
				'*.h' \
				'*.go' \
				'*.ts' \
				'*.js' \
				'*.py' \
				'*.yaml' \
				'*.yml' \
				'Makefile' \
		); \
		name=$$(git config user.name); \
		if [ "$${name}" == "" ]; then \
			>&2 echo "git user.name is required."; \
			>&2 echo "Set it up using:"; \
			>&2 echo "git config user.name \"John Doe\""; \
		fi; \
		for filename in $${files}; do \
			if ! ( head "$${filename}" | grep -iL "Copyright" > /dev/null ); then \
				autogen -i --no-code --no-tlc -c "$${name}" -l apache "$${filename}"; \
			fi; \
		done; \
		if ! ( head Makefile | grep -iL "Copyright" > /dev/null ); then \
			autogen -i --no-code --no-tlc -c "$${name}" -l apache Makefile; \
		fi;

## Formatting
#####################################################################

.PHONY: format
format: md-format yaml-format ## Format all files

.PHONY: md-format
md-format: node_modules/.installed ## Format Markdown files.
	@set -euo pipefail; \
		files=$$( \
			git ls-files --deduplicate \
				'*.md' \
		); \
		npx prettier --write --no-error-on-unmatched-pattern $${files}

.PHONY: yaml-format
yaml-format: node_modules/.installed ## Format YAML files.
	@set -euo pipefail; \
		files=$$( \
			git ls-files --deduplicate \
				'*.yml' \
				'*.yaml' \
		); \
		npx prettier --write --no-error-on-unmatched-pattern $${files}

## Linting
#####################################################################

.PHONY: lint
lint: actionlint markdownlint textlint yamllint zizmor ## Run all linters.

.PHONY: actionlint
actionlint: ## Runs the actionlint linter.
	@# NOTE: We need to ignore config files used in tests.
	@set -euo pipefail;\
		files=$$( \
			git ls-files --deduplicate \
				'.github/workflows/*.yml' \
				'.github/workflows/*.yaml' \
		); \
		if [ "$(OUTPUT_FORMAT)" == "github" ]; then \
			actionlint -format '{{range $$err := .}}::error file={{$$err.Filepath}},line={{$$err.Line}},col={{$$err.Column}}::{{$$err.Message}}%0A```%0A{{replace $$err.Snippet "\\n" "%0A"}}%0A```\n{{end}}' -ignore 'SC2016:' $${files}; \
		else \
			actionlint $${files}; \
		fi

.PHONY: zizmor
zizmor: .venv/.installed ## Runs the zizmor linter.
	@# NOTE: On GitHub actions this outputs SARIF format to zizmor.sarif.json
	@#       in addition to outputting errors to the terminal.
	@set -euo pipefail;\
		files=$$( \
			git ls-files --deduplicate \
				'.github/workflows/*.yml' \
				'.github/workflows/*.yaml' \
		); \
		if [ "$(OUTPUT_FORMAT)" == "github" ]; then \
			.venv/bin/zizmor --quiet --pedantic --format sarif $${files} > zizmor.sarif.json || true; \
		fi; \
		.venv/bin/zizmor --quiet --pedantic --format plain $${files}

.PHONY: markdownlint
markdownlint: node_modules/.installed ## Runs the markdownlint linter.
	@# NOTE: Issue and PR templates are handled specially so we can disable
	@# MD041/first-line-heading/first-line-h1 without adding an ugly html comment
	@# at the top of the file.
	@set -euo pipefail;\
		files=$$( \
			git ls-files --deduplicate \
				'*.md' \
				':!:.github/pull_request_template.md' \
				':!:.github/ISSUE_TEMPLATE/*.md' \
		); \
		if [ "$(OUTPUT_FORMAT)" == "github" ]; then \
			exit_code=0; \
			while IFS="" read -r p && [ -n "$$p" ]; do \
				file=$$(echo "$$p" | jq -c -r '.fileName // empty'); \
				line=$$(echo "$$p" | jq -c -r '.lineNumber // empty'); \
				endline=$${line}; \
				message=$$(echo "$$p" | jq -c -r '.ruleNames[0] + "/" + .ruleNames[1] + " " + .ruleDescription + " [Detail: \"" + .errorDetail + "\", Context: \"" + .errorContext + "\"]"'); \
				exit_code=1; \
				echo "::error file=$${file},line=$${line},endLine=$${endline}::$${message}"; \
			done <<< "$$(npx markdownlint --config .markdownlint.yaml --dot --json $${files} 2>&1 | jq -c '.[]')"; \
			if [ "$${exit_code}" != "0" ]; then \
				exit "$${exit_code}"; \
			fi; \
		else \
			npx markdownlint --config .markdownlint.yaml --dot $${files}; \
		fi; \
		files=$$( \
			git ls-files --deduplicate \
				'.github/pull_request_template.md' \
				'.github/ISSUE_TEMPLATE/*.md' \
		); \
		if [ "$(OUTPUT_FORMAT)" == "github" ]; then \
			exit_code=0; \
			while IFS="" read -r p && [ -n "$$p" ]; do \
				file=$$(echo "$$p" | jq -c -r '.fileName // empty'); \
				line=$$(echo "$$p" | jq -c -r '.lineNumber // empty'); \
				endline=$${line}; \
				message=$$(echo "$$p" | jq -c -r '.ruleNames[0] + "/" + .ruleNames[1] + " " + .ruleDescription + " [Detail: \"" + .errorDetail + "\", Context: \"" + .errorContext + "\"]"'); \
				exit_code=1; \
				echo "::error file=$${file},line=$${line},endLine=$${endline}::$${message}"; \
			done <<< "$$(npx markdownlint --config .github/template.markdownlint.yaml --dot --json $${files} 2>&1 | jq -c '.[]')"; \
			if [ "$${exit_code}" != "0" ]; then \
				exit "$${exit_code}"; \
			fi; \
		else \
			npx markdownlint  --config .github/template.markdownlint.yaml --dot $${files}; \
		fi

.PHONY: textlint
textlint: node_modules/.installed ## Runs the textlint linter.
	@set -e;\
		files=$$( \
			git ls-files --deduplicate \
				'*.md' \
				'*.txt' \
		); \
		if [ "$(OUTPUT_FORMAT)" == "github" ]; then \
			exit_code=0; \
			while IFS="" read -r p && [ -n "$$p" ]; do \
				filePath=$$(echo "$$p" | jq -c -r '.filePath // empty'); \
				file=$$(realpath --relative-to="." "$${filePath}"); \
				while IFS="" read -r m && [ -n "$$m" ]; do \
					line=$$(echo "$$m" | jq -c -r '.loc.start.line'); \
					endline=$$(echo "$$m" | jq -c -r '.loc.end.line'); \
					message=$$(echo "$$m" | jq -c -r '.message'); \
					echo "::error file=$${file},line=$${line},endLine=$${endline}::$${message}"; \
				done <<<"$$(echo "$$p" | jq -c -r '.messages[] // empty')"; \
			done <<< "$$(./node_modules/.bin/textlint -c .textlintrc.json --format json $${files} 2>&1 | jq -c '.[]')"; \
			exit "$${exit_code}"; \
		else \
			./node_modules/.bin/textlint -c .textlintrc.json $${files}; \
		fi

.PHONY: yamllint
yamllint: .venv/.installed ## Runs the yamllint linter.
	@set -euo pipefail;\
		extraargs=""; \
		files=$$( \
			git ls-files --deduplicate \
				'*.yml' \
				'*.yaml' \
		); \
		if [ "$(OUTPUT_FORMAT)" == "github" ]; then \
			extraargs="-f github"; \
		fi; \
		.venv/bin/yamllint --strict -c .yamllint.yaml $${extraargs} $${files}

## Maintenance
#####################################################################

.PHONY: qmk-clean
qmk-clean: .venv/.installed ## Clean qmk files.
	cd $(QMK_HOME) &&
		.venv/bin/qmk clean -a

.PHONY: clean
clean: ## Delete temporary files.
	@set -euo pipefail;\
		rm -rf \
			.venv \
			node_modules \
			*.sarif.json \
			third_party; \

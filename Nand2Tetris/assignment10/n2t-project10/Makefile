.PHONY: help
.DEFAULT_GOAL := help

help:
	@fgrep -h "##" $(MAKEFILE_LIST) | fgrep -v fgrep | sort | awk 'BEGIN {FS = ":.*?## "}; {printf "\033[36m%-30s\033[0m %s\n", $$1, $$2}'

install: ## Install requirements
	pip install -r requirements.txt

fmt format: ## Run code formatters
	isort app jack_analyzer.py
	black app jack_analyzer.py

lint: ## Run code linters
	isort --check app jack_analyzer.py
	black --check app jack_analyzer.py
	flake8 app jack_analyzer.py
	mypy app jack_analyzer.py

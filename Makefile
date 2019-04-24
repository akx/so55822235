.PHONY: experiment

experiment:
	python3 setup.py build
	cp build/lib*/*.so .
	python3 so55822235.py
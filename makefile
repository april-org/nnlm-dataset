all:
	g++ -O3 -fPIC -shared -o nnlmdataset.so nnlm-dataset.cc $(shell pkg-config --cflags --libs april-ann)

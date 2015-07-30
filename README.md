NNLM DataSet
------------

DataSetToken implementation for NNLMs and related stuff.

Execution
=========

```
$ make && sudo make install
$ lua test/test.lua
<s>	<s>	=>	the
<s>	the	=>	u.s.
the	u.s.	=>	centers
u.s.	centers	=>	for
centers	for	=>	disease
for	disease	=>	control
disease	control	=>	and
control	and	=>	prevention
and	prevention	=>	in
prevention	in	=>	atlanta
...
```

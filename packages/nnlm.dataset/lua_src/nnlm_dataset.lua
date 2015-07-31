nnlm = nnlm or {}
nnlm.dataset = nnlm.dataset or {}
nnlm.dataset.both = function(t)
  local params = get_table_fields({
      corpora = { isa_match=nnlm.corpora, mandatory=true },
      order = { type_match="number", mandatory=true },
      initial_word = { type_match="string", mandatory=true, default="<s>" },
      final_word = { type_match="string", mandatory=true, default="</s>" },
                                  }, t)
  local in_ds = nnlm.dataset{
    offset       = -params.order + 1,
    length       =  params.order - 1,
    corpora      = params.corpora,
    initial_word = params.initial_word,
    final_word   = params.final_word,
  }
  local out_ds = nnlm.dataset{
    offset       = 0,
    length       = 1,
    corpora      = params.corpora,
    initial_word = params.initial_word,
    final_word   = params.final_word,
  }
  return in_ds,out_ds
end

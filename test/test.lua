require "nnlmutils"

local dir     = arg[0]:dirname()
local lex     = lexClass.load(io.open(dir.."/voc.398"))
local corpora = nnlm.corpora(dir.."/sample.txt",lex.cobj,"<unk>")
local in_ds   = nnlm.dataset{
  corpora = corpora,
  length  =  2, -- bigrams
  offset  = -2, -- input requires negative offset
  initial_word = "<s>",
  final_word   = "</s>",
}
local out_ds   = nnlm.dataset{
  corpora = corpora,
  length  = 1, -- unigram
  offset  = 0, -- output shouldn't requires offset
  initial_word = "<s>",
  final_word   = "</s>",
}

local function words_of(tk)
  return lex:getWordFromWordId(select(2,tk:to_dense():max()))
end

for i,in_pat in in_ds:patterns() do
  local out_pat = out_ds:getPattern(i)
  print(words_of(in_pat[1]), words_of(in_pat[2]), "=>", words_of(out_pat))
end

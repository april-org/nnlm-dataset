require "aprilann" -- it is not necessary but its clear
require "nnlmutils"

local dir     = arg[0]:dirname()
local lex     = lexClass.load(io.open(dir.."/voc.398"))
local corpora = nnlm.corpora(dir.."/sample.txt",lex)
local in_ds,out_ds = nnlm.dataset.both{
  corpora = corpora,
  order   =  3, -- trigrams
}

local function words_of(tk)
  local idx = select(2,tk:to_dense():max(2)):squeeze()
  local t = {}
  for i=1,#idx do
    t[i] = lex:getWordFromWordId(idx[i])
  end
  return t
end

for i,in_pat in in_ds:patterns() do
  local out_pat = out_ds:getPattern(i)
  print(iterator.zip(iterator(words_of(in_pat[1])),
                     iterator(words_of(in_pat[2])),
                     iterator.duplicate("=>"),
                     iterator(words_of(out_pat))):concat(" ", "\n"))
end

print("*******************************************************************")

local in_pat  = in_ds:getPatternBunch(iterator.range(in_ds:numPatterns()):table())
local out_pat = out_ds:getPatternBunch(iterator.range(in_ds:numPatterns()):table())

print(iterator.zip(iterator(words_of(in_pat[1])),
                   iterator(words_of(in_pat[2])),
                   iterator.duplicate("=>"),
                   iterator(words_of(out_pat))):concat(" ", "\n"))

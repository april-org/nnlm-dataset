#include <algorithm>
#include <utility>
#include <vector>

#include "april-ann.h" // includes all APRIL-ANN headers
#include "nnlm_dataset.h"

using namespace std;

using AprilMath::FloatGPUMirroredMemoryBlock;
using AprilMath::Int32GPUMirroredMemoryBlock;
using AprilUtils::SharedPtr;
using Basics::SparseMatrixFloat;
using Basics::Token;
using Basics::TokenBunchVector;
using Basics::TokenSparseMatrixFloat;

#define MAX_WORD_LEN 256u

////////////////////////////////////////////////////////////////////////////

typedef std::vector<pair<int,int> > PairVector;

namespace LanguageModels {
  
  // TODO: Implement in a generic way which allow to ignore context cues,
  // for instance, giving initial_word=0 and/or final_word=0
  NNLMDataSetToken::NNLMDataSetToken(const size_t offset, const size_t length,
                                     const uint32_t initial_word,
                                     const uint32_t final_word,
                                     SharedPtr<NNLMCorpora> corpora) :
    DataSetToken(),
    offset(offset), length(length), lex_size(corpora->getVocabSize()),
    initial_word(initial_word), final_word(final_word),
    corpora(corpora),
    first_word(corpora->getNumberOfSentences()+1) {
    first_word[0] = 0;
    for (unsigned int i=0; i<corpora->getNumberOfSentences(); ++i) {
      // +1 because of end context cue
      first_word[i+1] = first_word[i] + corpora->getSentenceLength(i) + 1;
    }
  }

  NNLMDataSetToken::~NNLMDataSetToken() {
  }
  
  int NNLMDataSetToken::numPatterns() {
    return static_cast<int>(first_word.back());
  }

  int NNLMDataSetToken::patternSize() {
    return static_cast<int>(lex_size * length);
  }
  
  pair<int,int> NNLMDataSetToken::getSentenceWordPair(int index) const {
    // binary search of the sentence which contains the given pattern index
    auto sentence_it = upper_bound(first_word.begin(), first_word.end(),
                                   static_cast<uint32_t>(index)) - 1u;
    // index of the sentence
    int s_index = static_cast<int>(sentence_it - first_word.begin());
    // index of the word inside the sentence: it can be w_index<0 or
    // w_index>=sentence_size, and this cases will be filled with context cues.
    int w_index = index - static_cast<int>(*sentence_it) + offset;
    return make_pair(s_index,w_index);
  }
  
  TokenSparseMatrixFloat *NNLMDataSetToken::
  buildSparseMatrixFloatToken(int i, const PairVector &sentence_word_pairs) const {
    unsigned int bunch_size = sentence_word_pairs.size();
    SharedPtr<FloatGPUMirroredMemoryBlock> ones = new FloatGPUMirroredMemoryBlock(bunch_size);
    SharedPtr<Int32GPUMirroredMemoryBlock> indices = new Int32GPUMirroredMemoryBlock(bunch_size);
    SharedPtr<Int32GPUMirroredMemoryBlock> first_index = new Int32GPUMirroredMemoryBlock(bunch_size+1u);
    (*first_index)[0] = 0;
    for (int j=0; j<static_cast<int>(bunch_size); ++j) {
      auto s_it = corpora->getSentence(sentence_word_pairs[j].first).begin();
      int w_pos = sentence_word_pairs[j].second + i;
      (*ones)[j] = 1.0f;
      (*first_index)[j+1] = j+1;
      if (w_pos < 0) (*indices)[j] = initial_word - 1;
      else if (w_pos >= static_cast<int>(corpora->getSentenceLength(sentence_word_pairs[j].first))) (*indices)[j] = final_word - 1;
      else (*indices)[j] = s_it[w_pos] - 1;
    }
    SparseMatrixFloat *mat = new SparseMatrixFloat(bunch_size,
                                                   static_cast<int>(lex_size),
                                                   ones.get(), indices.get(),
                                                   first_index.get());
    return new TokenSparseMatrixFloat(mat);
  }

  Token *NNLMDataSetToken::getPattern(int index) {
    SharedPtr<TokenBunchVector> pat = new TokenBunchVector(length);
    auto sentence_word_pair = getSentenceWordPair(index);
    auto s_it = corpora->getSentence(sentence_word_pair.first).begin();
    for (int i=0; i<static_cast<int>(length); ++i) {
      Token *tk = buildSparseMatrixFloatToken(i, PairVector(1u, sentence_word_pair));
      // WARNING flow break here
      if (length == 1) return tk;
      (*pat)[i] = tk;
    }
    return pat.weakRelease();
  }

  Token *NNLMDataSetToken::getPatternBunch(const int *indexes,
                                           unsigned int bunch_size) {
    std::vector<pair<int,int> > sentence_word_pairs(bunch_size);
    for (auto i=0u; i<bunch_size; ++i) {
      sentence_word_pairs[i] = getSentenceWordPair(indexes[i]);
    }
    SharedPtr<TokenBunchVector> pat = new TokenBunchVector(length);    
    for (int i=0; i<static_cast<int>(length); ++i) {
      Token *tk = buildSparseMatrixFloatToken(i, sentence_word_pairs);
      // WARNING flow break here
      if (length == 1) return tk;
      (*pat)[i] = tk;
    }
    return pat.weakRelease();
  }

  void NNLMDataSetToken::putPattern(int index, Token *pat) {
    UNUSED_VARIABLE(index);
    UNUSED_VARIABLE(pat);
    ERROR_EXIT(1, "Not implemented!!!\n");
  }

  void NNLMDataSetToken::putPatternBunch(const int *indexes,
                                         unsigned int bunch_size,
                                         Token *pat) {
    UNUSED_VARIABLE(indexes);
    UNUSED_VARIABLE(bunch_size);
    UNUSED_VARIABLE(pat);
    ERROR_EXIT(2, "Not implemented!!!\n");
  }
  
} // namespace LanguageModels

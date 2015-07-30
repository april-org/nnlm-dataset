#ifndef NNLM_DATASET_H
#define NNLM_DATASET_H

#include <utility>
#include <vector>

#include "april-ann.h"

#include "nnlm_corpora.h"

namespace LanguageModels {

  class NNLMDataSetToken : public Basics::DataSetToken {
  public:
    NNLMDataSetToken(const size_t offset, const size_t length,
                     const uint32_t initial_word, const uint32_t final_word,
                     AprilUtils::SharedPtr<NNLMCorpora> corpora);
    ~NNLMDataSetToken();
    
    virtual Basics::Token *getPattern(int index);
    virtual Basics::Token *getPatternBunch(const int *indexes,
                                           unsigned int bunch_size);
    virtual void putPattern(int index, Basics::Token *pat);
    virtual void putPatternBunch(const int *indexes, unsigned int bunch_size,
                                 Basics::Token *pat);
    virtual int numPatterns();
    virtual int patternSize();
    
  private:
    const size_t offset, length, lex_size;
    const uint32_t initial_word, final_word;
    AprilUtils::SharedPtr<NNLMCorpora> corpora;
    std::vector<uint32_t> first_word; // corpora.getNumberOfSentences() + 1

    std::pair<int,int> getSentenceWordPair(int index) const;
  };
  
} // namespace LanguageModels

DECLARE_LUA_TABLE_BIND_SPECIALIZATION(LanguageModels::NNLMDataSetToken);

#endif // NNLM_DATASET_H

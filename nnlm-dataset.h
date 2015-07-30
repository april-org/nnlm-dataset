#ifndef NNLM_DATASET_H
#define NNLM_DATASET_H

#include "april-ann.h"

namespace LanguageModels {
  
  class NNLMCorpora : public Referenced {
  public:
    NNLMCorpora(const char *filename,
                AprilUtils::LexClass *lex,
                const uint32_t unk_id);
    ~NNLMCorpora();
    
    const AprilUtils::vector<uint32_t> &getSentence(size_t i) const;
    size_t getNumberOfSentences() const;
    
  private:
    const char *filename;
    AprilUtils::SharedPtr<AprilUtils::LexClass> lex;
    const uint32_t unk_id;
    AprilUtils::vector< AprilUtils::vector<uint32_t> > sentences;
  };
  
  class NNLMDataSetToken : Basics::DataSetToken {
  public:
    NNLMDataSetToken(const int offset, const int order,
                     AprilUtils::SharedPtr<NNLMCorpora> corpora);
    ~NNLMDataSetToken();
    
    virtual Basics::Token *getPattern(int index);
    virtual Basics::Token *getPatternBunch(const int *index, unsigned int bunch_size);
    virtual void putPattern(int index, Token *pat);
    virtual void putPatternBunch(const int *indexes, unsigned int bunch_size, Token *pat);
    virtual int numPatterns();
    virtual int patternSize();
  };
  
} // namespace LanguageModels

#endif // NNLM_DATASET_H

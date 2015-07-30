#ifndef NNLM_DATASET_H
#define NNLM_DATASET_H

#include "april-ann.h"

namespace LanguageModels {

  /**
   * @brief This class loads a textual file and converts it into numeric values.
   * Once it is loaded, the class allows to index sentences as vectors.
   */
  class NNLMCorpora : public Referenced {
  public:
    NNLMCorpora(const char *filename,
                AprilUtils::LexClass *lex,
                const uint32_t unk_id);
    ~NNLMCorpora();
    
    const AprilUtils::vector<uint32_t> &getSentence(size_t i) const;
    size_t getSentenceLength(size_t i) const;
    size_t getNumberOfSentences() const;
    
  private:
    const char *filename;
    AprilUtils::SharedPtr<AprilUtils::LexClass> lex;
    const uint32_t unk_id;
    AprilUtils::vector< AprilUtils::vector<uint32_t> > sentences;
  };
  
  class NNLMDataSetToken : Basics::DataSetToken {
  public:
    NNLMDataSetToken(const size_t offset, const size_t order,
                     AprilUtils::SharedPtr<NNLMCorpora> corpora);
    ~NNLMDataSetToken();
    
    virtual Basics::Token *getPattern(int index);
    virtual Basics::Token *getPatternBunch(const int *index, unsigned int bunch_size);
    virtual void putPattern(int index, Basics::Token *pat);
    virtual void putPatternBunch(const int *indexes, unsigned int bunch_size,
                                 Basics::Token *pat);
    virtual int numPatterns();
    virtual int patternSize();
    
  private:
    const size_t offset, order;
    AprilUtils::SharedPtr<NNLMCorpora> corpora;
    AprilUtils::vector<uint32_t> first_word; // corpora.getNumberOfSentences() + 1
  };
  
} // namespace LanguageModels

#endif // NNLM_DATASET_H

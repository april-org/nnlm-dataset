#ifndef NNLM_CORPORA_H
#define NNLM_CORPORA_H

#include <vector>

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
    
    const std::vector<uint32_t> &getSentence(size_t i) const;
    size_t getSentenceLength(size_t i) const;
    size_t getNumberOfSentences() const;
    size_t getVocabSize();
    uint32_t getWordId(const char *word);
    
  private:
    const char *filename;
    AprilUtils::SharedPtr<AprilUtils::LexClass> lex;
    const uint32_t unk_id;
    std::vector< std::vector<uint32_t> > sentences;
  };

} // namespace LanguageModels

DECLARE_LUA_TABLE_BIND_SPECIALIZATION(LanguageModels::NNLMCorpora);

#endif // NNLM_CORPORA_H

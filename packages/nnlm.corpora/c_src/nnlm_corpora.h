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
    class Sentence; // forward declaration
  public:
    NNLMCorpora(const char *filename,
                AprilUtils::LexClass *lex,
                const uint32_t unk_id);
    ~NNLMCorpora();
    
    NNLMCorpora::Sentence getSentence(size_t i) const;
    size_t getSentenceLength(size_t i) const;
    size_t getNumberOfSentences() const;
    size_t getVocabSize();
    uint32_t getWordId(const char *word);
    
  private:
    const char *filename;
    AprilUtils::SharedPtr<AprilUtils::LexClass> lex;
    const uint32_t unk_id;
    std::vector<size_t> first_sentence_word;
    size_t num_words, words_size;
    uint32_t *words; // mmapped
    
    /// This class is an array wrapper
    class Sentence {
    public:
      Sentence(uint32_t *b, uint32_t *e);
      Sentence(const Sentence &other);
      const uint32_t *begin() const;
      const uint32_t *end() const;
      size_t size() const;
      const uint32_t &operator[](size_t i) const;
      Sentence &operator=(const Sentence &other); 
    private:
      const uint32_t *b, *e; // begin and end pointers
    };
  };

} // namespace LanguageModels

DECLARE_LUA_TABLE_BIND_SPECIALIZATION(LanguageModels::NNLMCorpora);

#endif // NNLM_CORPORA_H

#include <cerrno>
#include <vector>

#include "april-ann.h" // includes all APRIL-ANN headers
#include "nnlm_corpora.h"

using namespace std;

using AprilUtils::constString;

#define MAX_WORD_LEN 256u

////////////////////////////////////////////////////////////////////////////

namespace LanguageModels {

  NNLMCorpora::Sentence::Sentence(uint32_t *b, uint32_t *e) : b(b), e(e) {
  }

  NNLMCorpora::Sentence::Sentence(const NNLMCorpora::Sentence &other) :
    b(other.b), e(other.e) {
  }
  
  const uint32_t *NNLMCorpora::Sentence::begin() const {
    return b;
  }
  
  const uint32_t *NNLMCorpora::Sentence::end() const {
    return e;
  }
  
  size_t NNLMCorpora::Sentence::size() const {
    return e - b;
  }
  
  const uint32_t &NNLMCorpora::Sentence::operator[](size_t i) const {
    return b[i];
  }

  NNLMCorpora::Sentence &NNLMCorpora::Sentence::operator=(const Sentence &other) {
    this->b = other.b;
    this->e = other.e;
    return *this;
  }

  /////////////////////////////////////////
  
  NNLMCorpora::NNLMCorpora(const char *filename,
                           AprilUtils::LexClass *lex,
                           const uint32_t unk_id) :
    Referenced(),
    filename(filename), lex(lex), unk_id(unk_id) {
    int file_descriptor;
    size_t file_size;
    char *file_mmapped;
    file_descriptor = open(filename, O_RDONLY);
    if (file_descriptor < 0) {
      ERROR_EXIT1(1, "Unable to open file %s\n", filename);
    }
    struct stat statbuf;
    if (fstat(file_descriptor,&statbuf) < 0) {
      ERROR_EXIT1(2, "Unable to compute file size for %s\n", filename);
    }
    file_size = statbuf.st_size;
    if ((file_mmapped = (char*)mmap(NULL, file_size,
                                    PROT_READ, MAP_SHARED,
                                    file_descriptor, 0)) == MAP_FAILED) {
      ERROR_EXIT2(1, "Error reading mmapped file %s: %s\n",
                  filename, strerror(errno));
    }
    constString line, word;
    // Two traversals: 1. counting sentences and words, 2. extracting tokens.
    //
    // 1. COUNTING SENTENCES AND WORDS
    num_words = 0;
    first_sentence_word.push_back(0u);
    constString input = constString(file_mmapped, file_size);
    while( (line = input.extract_line()) ) {
      while( (word = line.extract_token("\n\r\t ")) ) ++num_words;
      first_sentence_word.push_back(num_words);
    }
    //
    // 2. EXTRACTING TOKENS
    words_size = sizeof(uint32_t) * num_words;
    if ((words = (uint32_t*)mmap(NULL, words_size,
                                 PROT_READ | PROT_WRITE, MAP_ANON | MAP_SHARED,
                                 -1, 0)) == MAP_FAILED) {
      ERROR_EXIT2(1, "Error creating anonymous mmap of size %lu: %s\n",
                  words_size, strerror(errno));
    }
    input = constString(file_mmapped, file_size);
    char word_str[MAX_WORD_LEN + 1];
    size_t i = 0;
    while( (line = input.extract_line()) ) {
      while( (word = line.extract_token("\n\r\t ")) ) {
        strncpy(word_str, (const char *)word, word.len());
        word_str[word.len()] = '\0';
        uint32_t wid;
        if (!lex->getWordId(word_str, wid)) wid = unk_id;
        words[i++] = wid;
      }
    }
    close(file_descriptor);
    munmap(file_mmapped, file_size);
  }
  
  NNLMCorpora::~NNLMCorpora() {
    munmap(words, words_size);
  }

  NNLMCorpora::Sentence NNLMCorpora::getSentence(size_t i) const {
    return Sentence(words + first_sentence_word[i],
                    words + first_sentence_word[i+1]);
  }

  size_t NNLMCorpora::getSentenceLength(size_t i) const {
    return first_sentence_word[i+1] - first_sentence_word[i];
  }
  
  size_t NNLMCorpora::getNumberOfSentences() const {
    return first_sentence_word.size() - 1u;
  }

  size_t NNLMCorpora::getVocabSize() {
    return lex->wordTblSize();
  }

  uint32_t NNLMCorpora::getWordId(const char *word) {
    uint32_t wid=0;
    if (!lex->getWordId(word, wid)) {
      ERROR_EXIT1(3, "Unable to locate word %s\n", word);
    }
    return wid;
  }
  
} // namespace LanguageModels

#include <cerrno>
#include <vector>

#include "april-ann.h" // includes all APRIL-ANN headers
#include "nnlm_corpora.h"

using namespace std;

using AprilUtils::constString;

#define MAX_WORD_LEN 256u

////////////////////////////////////////////////////////////////////////////

namespace LanguageModels {
  
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
    constString input = constString(file_mmapped, file_size);
    constString line;
    unsigned int i = 0;
    char word_str[MAX_WORD_LEN + 1];
    while( (line = input.extract_line()) ) {
      constString word;
      sentences.emplace_back();
      while( (word = line.extract_token("\n\r\t ")) ) {
        strncpy(word_str, (const char *)word, word.len());
        word_str[word.len()] = '\0';
        uint32_t wid;
        if (!lex->getWordId(word_str, wid)) wid = unk_id;
        sentences[i].push_back(wid);
      }
      ++i;
    }
    close(file_descriptor);
    munmap(file_mmapped, file_size);
  }
  
  NNLMCorpora::~NNLMCorpora() {
  }

  const std::vector<uint32_t> &NNLMCorpora::getSentence(size_t i) const {
    return sentences[i];
  }

  size_t NNLMCorpora::getSentenceLength(size_t i) const {
    return sentences[i].size();
  }
  
  size_t NNLMCorpora::getNumberOfSentences() const {
    return sentences.size();
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

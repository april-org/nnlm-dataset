#include <cerrno>

#include "april-ann.h" // includes all APRIL-ANN headers
#include "nnlm-dataset.h"

using AprilUtils::constString;
using AprilUtils::LuaTable;
using AprilUtils::SharedPtr;
using Basics::SparseMatrixFloat;

#define MAX_WORD_LEN 256u

////////////////////////////////////////////////////////////////////////////

namespace LanguageModels {
  
  NNLMCorpora::NNLMCorpora(const char *filename,
                           AprilUtils::LexClass *lex,
                           const uint32_t unk_id) :
    filename(filename), lex(lex), unk_id(unk_id) {
    int file_descriptor;
    size_t file_size;
    char *file_mmapped;
    april_assert((file_descriptor = open(filename, O_RDONLY)) >= 0);
    struct stat statbuf;
    assert(fstat(file_descriptor,&statbuf) >= 0);
    file_size = statbuf.st_size;
    if ((file_mmapped = (char*)mmap(NULL, file_size,
                                    PROT_READ, MAP_SHARED,
                                    file_descriptor, 0)) == MAP_FAILED) {
      ERROR_EXIT2(1, "Error reading mmapped file %s: %s\n",
                  filename, strerror(errno));
    }
    constString input = constString(file_mmapped, file_size);
    constString line;
    size_t i = 0;
    char word_str[MAX_WORD_LEN + 1];
    while( (line = input.extract_line()) ) {
      constString word;
      sentences.push_back( AprilUtils::vector<uint32_t>() );
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

  const vector<uint32_t> &NNLMCorpora::getSentence(size_t i) const {
    return sentences[i];
  }
  
  size_t NNLMCorpora::getNumberOfSentences() const {
    return sentences.size();
  }
  
} // namespace LanguageModels

////////////////////////////////////////////////////////////////////////////

static int get(lua_State *L) {
  SharedPtr<MatrixFloat> m1 = new MatrixFloat(2, 10, 20);
  SharedPtr<MatrixFloat> m2 = new MatrixFloat(2, 20, 10);
  matFill(m1.get(), 20.0f);
  matFill(m2.get(), 10.0f);
  SharedPtr<MatrixFloat> m3 = new MatrixFloat(2, 10, 10);
  matGemm(m3.get(), CblasNoTrans, CblasNoTrans,
          1.0f, m1.get(), m2.get(), 0.0f);
  // using LuaTable you can push APRIL-ANN objects in Lua stack (be careful, not
  // all objects can be pushed)
  LuaTable::pushInto(L, m3.get());
  return 1;
}

extern "C" {
  int luaopen_example(lua_State *L) {
    static const luaL_Reg funcs[] = {
      {"get", get},
      {NULL, NULL}
    };
    luaL_requiref(L, "aprilann", luaopen_aprilann, 1);
    lua_pop(L, 1);
    luaL_newlib(L, funcs);
    return 1;
  }
}

//BIND_HEADER_H
#include "nnlm_corpora.h"
using namespace LanguageModels;
//BIND_END

//BIND_HEADER_C
#include "april-ann.h"

using AprilUtils::LexClass;
using AprilUtils::LuaTable;

IMPLEMENT_LUA_TABLE_BIND_SPECIALIZATION(NNLMCorpora);
//BIND_END

/////////////////////////////////////////////////////////////////////////////

//BIND_LUACLASSNAME NNLMCorpora nnlm.corpora
//BIND_CPP_CLASS NNLMCorpora

//BIND_CONSTRUCTOR NNLMCorpora
{
  const char *filename = lua_tostring(L,1);
  LexClass *lex        = LuaTable::convertTo<LexClass*>(L,2);
  const char *unk      = lua_tostring(L,3);
  uint32_t unk_id=0;
  if (!lex->getWordId(unk, unk_id)) {
    LUABIND_FERROR1("Unable to locate word %s", unk);
  }
  obj = new NNLMCorpora(filename, lex, unk_id);
  LUABIND_RETURN(NNLMCorpora, obj);
}
//BIND_END

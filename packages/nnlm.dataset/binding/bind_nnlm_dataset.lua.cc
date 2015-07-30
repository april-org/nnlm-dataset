1//BIND_HEADER_H
#include "bind_nnlm_corpora.h"
#include "nnlm_dataset.h"
using namespace LanguageModels;
//BIND_END

//BIND_HEADER_C
#include "april-ann.h"

using AprilUtils::LexClass;
using AprilUtils::LuaTable;
using Basics::DataSetToken;

IMPLEMENT_LUA_TABLE_BIND_SPECIALIZATION(NNLMDataSetToken);

extern DataSetToken *lua_toDataSetToken(lua_State*,int);
//BIND_END

/////////////////////////////////////////////////////////////////////////////

//BIND_LUACLASSNAME DataSetToken dataset.token
//BIND_LUACLASSNAME NNLMDataSetToken nnlm.dataset
//BIND_CPP_CLASS NNLMDataSetToken
//BIND_SUBCLASS_OF NNLMDataSetToken DataSetToken

//BIND_CONSTRUCTOR NNLMDataSetToken
{
  LuaTable t(L,1);
  unsigned int offset  = t["offset"].get<uint>();
  unsigned int length  = t["length"].get<uint>();
  NNLMCorpora *corpora = t["corpora"].get<NNLMCorpora*>();
  lua_getfield(L,1,"initial_word");
  lua_getfield(L,1,"final_word");
  const char *initial_word = lua_tostring(L,-2);
  const char *final_word   = lua_tostring(L,-1);
  uint32_t initial_word_id = corpora->getWordId(initial_word);
  uint32_t final_word_id   = corpora->getWordId(final_word);
  lua_pop(L,2);
  obj = new NNLMDataSetToken(offset, length,
                             initial_word_id, final_word_id,
                             corpora);
  LUABIND_RETURN(NNLMDataSetToken, obj);
}
//BIND_END

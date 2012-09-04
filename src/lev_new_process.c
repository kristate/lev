/*
 *  Copyright 2012 The lev Authors. All Rights Reserved.
 *
 *  Licensed under the Apache License, Version 2.0 (the "License");
 *  you may not use this file except in compliance with the License.
 *  You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 *  Unless required by applicable law or agreed to in writing, software
 *  distributed under the License is distributed on an "AS IS" BASIS,
 *  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *  See the License for the specific language governing permissions and
 *  limitations under the License.
 *
 */

 #include "lev_new_base.h"

#include <stdlib.h>
#include <string.h>
#include <assert.h>

#include <lua.h>
#include <lauxlib.h>
#include "luv_debug.h"

#ifndef WIN32
#include <ctype.h>
#include <unistd.h>
#include <sys/utsname.h>
#endif

/*

  X:TODO Beginning of PROCESS Module...
  
*/

#define UNWRAP(h) \
  process_obj* self = container_of((h), process_obj, handle); \
  lua_State* L = self->handle.loop->data;

typedef struct {
  LEVBASE_REF_FIELDS
} process_obj;

static int process_new(lua_State* L) {
  uv_loop_t* loop;
  process_obj* self;

  loop = uv_default_loop();
  assert(L == loop->data);

  self = (process_obj*)create_obj_init_ref(L, sizeof *self, "lev.process");

  return 1;
}

static int process_cwd(lua_State* L) {
  size_t size = 2*PATH_MAX-1;
  char path[2*PATH_MAX];
  uv_err_t err;

  err = uv_cwd(path, size);
  if (err.code != UV_OK) {
    return luaL_error(L, "uv_cwd: %s", uv_strerror(err));
  }
  lua_pushstring(L, path);
  return 1;
}

static luaL_reg methods[] = {
   /*{ "method_name",     ...      }*/
  { NULL,         NULL            }
};


static luaL_reg functions[] = {
   { "new", process_new }
  ,{ "cwd", process_cwd }
  ,{ NULL, NULL }
};


#define PROPERTY_COUNT 1

static int process_platform(lua_State* L) {
#ifdef WIN32
  lua_pushstring(L, "win32");
#else
  struct utsname info;
  char *p;

  uname(&info);
  for (p = info.sysname; *p; p++)
    *p = (char)tolower((unsigned char)*p);
  lua_pushstring(L, info.sysname);
#endif
  return 1;
}

void luaopen_lev_process(lua_State *L) {
  luaL_newmetatable(L, "lev.process");
  luaL_register(L, NULL, methods);
  lua_setfield(L, -1, "__index");

  lua_createtable(L, 0, ARRAY_SIZE(functions) + PROPERTY_COUNT - 1);
  luaL_register(L, NULL, functions);

  /* set properties */
  process_platform(L);
  lua_setfield(L, -2, "platform");

  lua_setfield(L, -2, "process");
}
